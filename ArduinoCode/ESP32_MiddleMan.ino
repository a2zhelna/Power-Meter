//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

#include <SPI.h>
#include "ads8331_fns.h"
#include "pga113_fns.h"
#include "WIFI_fns.h"

//ADC reference voltage
static float v_ref = 4.096;
//Graph domain (ms)
static int domain = 5000;
//Sample rate (Hz)
static short sampleRate = 200;
//Current range 0 - 8A; 1 - 20mA
static bool currentRange = 0;
//Data mode 0 - Power; 1 - Voltage; 2 - Current
static char dataMode = 0;
//Is the overcurrent warning active?
static bool warningActive = false;
//Resistance of the leads connected to the unit under test
static float leadResistance = 0;

//Create task handles
static TaskHandle_t wifiloopt_handle;
static TaskHandle_t wifisendt_handle;
static TaskHandle_t readt_handle;
static TaskHandle_t notifyt_handle;

//Global readings buffer (amount of samples sent at once)
//If this is altered, then buffers will have to be deallocated and new ones will need to be allocated.
const char buf_len = 100;
//Storing in static memory since its address should never change.
static float readings_buf[buf_len] = {};
static int time_buf[buf_len] = {};
//The buffers' index that data is being written to
static char buf_idx = 0;

//Mutex lock for writing/reading the mesaurement buffer (shared resource)
static SemaphoreHandle_t buf_access_mtx;
//Mutex used to make sure ONE wifi "process" runs at a time
static SemaphoreHandle_t wifi_mtx;
//Binary semaphore which is "1" when the buffer is full (and should be broadcasted)
static SemaphoreHandle_t buf_full;
//Binary semaphore which is "1" when there's a notification to be sent to the client.
//EX: the "read task" detects an overcurrent, and notifies the "notify task" on another core
//that there's a problem.
static SemaphoreHandle_t notify_bin;
//Mutex which locks warningActive shared resource
static SemaphoreHandle_t notify_mtx;
//Mutex which makes sure that the dataMode, currentRange, and sampleRate shared resources aren't
//read while they are being set by the user
static SemaphoreHandle_t config_mtx;


//Callbacks:

//Specify what data is to be sent to the website
void setDataMode(String s) {
  //Configuration variables are being set, so make sure that
  //they aren't being read by another task at the same time.
  xSemaphoreTake(config_mtx, portMAX_DELAY);
  if (s == "power") {
    //Serial.println("READING POWER");
    dataMode = 0;
  } else if (s == "voltage") {
    //Serial.println("READING VOLTAGE");
    dataMode = 1;
  } else if (s == "current") {
    ///Serial.println("READING CURRENT");
    dataMode = 2;
  }
  xSemaphoreGive(config_mtx);
  
  //Reset writing to the data buffers since they need to now
  //store different data
  xSemaphoreTake(buf_access_mtx, portMAX_DELAY);
  buf_idx = 0;
  xSemaphoreGive(buf_access_mtx);
}

//Specify what current input is being read
void setCurrentRange(String s) {
  xSemaphoreTake(config_mtx, portMAX_DELAY);
  if (s == "20mA") {
    //Serial.println("20mA");
    currentRange = 1;
  } else if (s == "8A") {
    //Serial.println("8A");
    currentRange = 0;
  }
  xSemaphoreGive(config_mtx);

  //Reset writing to the data buffers
  xSemaphoreTake(buf_access_mtx, portMAX_DELAY);
  buf_idx = 0;
  xSemaphoreGive(buf_access_mtx);
}

//Specify the width of the displayed domain
//Since displaying too many samples (around >5000) causes large
//performance drops on the website, with a smaller domain width,
//higher sampling rates can be used.
void setDomain(int d) {
  xSemaphoreTake(config_mtx, portMAX_DELAY);

  domain = d;

  if (d == 1000 || d == 2000) {
    sampleRate = 1000;
  } else if (d == 3000 || d == 4000) {
    sampleRate = 500;
  } else if (d == 5000 || d == 6000) {
    sampleRate = 250;
  } else if (d == 7000 || d == 8000) {
    sampleRate = 200;
  } else if (d == 9000 || d == 10000) {
    sampleRate = 100;
  }

  xSemaphoreGive(config_mtx);
}

//Specify the resistance of the leads connecting the
//power meter device to the unit
void setLeadResistance(float lr) {
  xSemaphoreTake(config_mtx, portMAX_DELAY);
  leadResistance = lr;
  xSemaphoreGive(config_mtx);
}

//Give the website all of the user's current configuration data.
//This is so that when the user reloads the webpage, the displayed config
//settings reflect how the actual firmware is behaving
void sendAllConfig() {
  xSemaphoreTake(config_mtx, portMAX_DELAY);
  sendAllConfigJson(currentRange, dataMode, domain, leadResistance);
  xSemaphoreGive(config_mtx);
}

//Function runs on core 0 (The "WIFI core")
//warningActive is a shared resource, so a mutex is used
//for safely resetting it back to 0.
void removeWarning() {
  //Serial.println("Removing warning");
  xSemaphoreTake(notify_mtx, portMAX_DELAY);
  warningActive = 0;
  xSemaphoreGive(notify_mtx);
}

//End of callbacks


//Function which selects the gain of the PGA
int mngGain(int v, int g) {
  float v_conv = (v / g / 65535.00) * v_ref;
  if (v_conv < 0.375) {
    g = 10;
  } else if (v_conv < 0.75) {
    g = 5;
  } else if (v_conv < 1.9) {
    g = 2;
  } else {
    g = 1;
  }
  return g;
}

//Task responsible for repeatedly running the wifiLoop function
//and for running the setup function for the wifi setup.
//This is essential for the ESP32 to continuously communicate
//with the client
void wifiloop_task(void *pvParameters) {
  //Setup code
  wifiSetup();

  //Loop code
  while (1) {
    vTaskDelay(5 / portTICK_PERIOD_MS);  //Adding a delay here removes a "watchdog did not reset in time" error
    //Using mutex because data shouldn't be broadcasted while the loop function runs
    xSemaphoreTake(wifi_mtx, portMAX_DELAY);
    wifiLoop();
    xSemaphoreGive(wifi_mtx);
  }
}

//Task for notifying the client of an overcurrent
void notify_task(void *pvParameters) {
  //Loop code
  while (1) {
    xSemaphoreTake(notify_bin, portMAX_DELAY);
    sendJson("overcurrent_update", "1");
  }
}

//Task for actually sending the buffers with all of the measurements
void wifisend_task(void *pvParameters) {
  //Setup code
  //Data-copy arrays
  //The copies of the data arrays are passed to the send function
  //because arrays are passed by reference, and if the original buffers
  //are passed, the task that makes measurements will be blocked from accessing
  //the buffers until the send function completes.
  int time_buf2[buf_len];
  float readings_buf2[buf_len];
  //Loop code
  while (1) {
    //If "buf_len" readings have been made, data can be uploaded
    xSemaphoreTake(buf_full, portMAX_DELAY);
    //Take the lock for accessing the measurement buffers
    xSemaphoreTake(buf_access_mtx, portMAX_DELAY);
    //CRITICAL SECTION:

    //Copy the values from the readings arrays (shared resources)
    for (int i = 0; i < buf_len; i++) {
      time_buf2[i] = time_buf[i];
      readings_buf2[i] = readings_buf[i];
    }

    //END OF CRITICAL SECTION
    xSemaphoreGive(buf_access_mtx);

    //Broadcast the values (using mutex because data shouldn't be sent simultaneously with the wifi loop function)
    xSemaphoreTake(wifi_mtx, portMAX_DELAY);
    sendJsonArrObj(readings_buf2, time_buf2);
    xSemaphoreGive(wifi_mtx);
  }
}

//Task for making actual readings (communicating with ADC, PGA; making calculations; noticing overcurrent)
void read_task(void *pvParameters) {
  //Setup code

  //Measured values
  float ps_v = 0;
  float burd_v = 0;
  float hcur_c = 0;
  float lcur_c = 0;

  //Averaging variables
  char avg_len = 8;
  //cyclic buffers containing measured values (which are then averaged)
  float ps_v_arr[avg_len] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  float hcur_arr[avg_len] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  float lcur_arr[avg_len] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  float ps_v_sum = 0;
  float hcur_sum = 0;
  float lcur_sum = 0;
  char avg_idx = 0;
  bool avg_ready = 0;

  //Actual data added to the data buffer
  //(Instead of having buffers for each the current, voltage and power,
  //there's one "data" buffer which is interpreted by the front-end based
  //off of what configuration the client selected)
  float sentData = 0;

  //Raw binary readings from the ADC.
  unsigned short ps_v_bin = 0;
  unsigned short burd_v_bin = 0;
  unsigned short hcur_v_bin = 0;
  unsigned short lcur_v_bin = 0;

  //The gains with which we read each ADC channel
  //There exist gains 1,2,5,10
  unsigned char ps_g = 1;
  unsigned char hcur_g = 1;
  unsigned char lcur_g = 1;
  unsigned char burd_g = 1;

  //Chip select and an adc signal pin
  pinMode(2, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(4, OUTPUT);

  //Overcurrent control pin
  pinMode(27, OUTPUT);
  digitalWrite(27, 1);

  //running the hardware setup functions
  adcSetup();
  pgaSetup();
  
  readChannel(0x0000, 1);

  //Loop code
  while (1) {
    //Read the voltage at the channel specified by the argument of previously run readChannel function
    ps_v_bin = readChannel(0x1000, ps_g);
    hcur_v_bin = readChannel(0x2000, hcur_g);
    lcur_v_bin = readChannel(0x3000, lcur_g);
    burd_v_bin = readChannel(0x0000, burd_g);

    //Get the time of the measurement:
    unsigned long now = millis();
    
    //Critical section for config data
    xSemaphoreTake(config_mtx, portMAX_DELAY);

    hcur_c = ((hcur_v_bin / hcur_g / 65535.00 * v_ref * 3 * (1 / 1.3333))); // 3 is to account for voltage divider,
                                                                            // 1.3333 is the current sensor's sensitivity

    lcur_c = (lcur_v_bin / lcur_g / 65535.00 * v_ref * (1 / 39.434));       // 39.434 is the feedback ammeter's sensitivity

    burd_v = (burd_v_bin / burd_g / 65535.0 * v_ref) * 1.0382;              // 1.0382 is to account for the error from the FLUKE measurements      

    ps_v = (ps_v_bin / ps_g / 65535.00 * v_ref * (6 * 0.997))               // 6 is to account for voltage divider
            - burd_v - (hcur_c * (0.027+0.0185))                            // 0.997 is to account for voltage divider error
            - (leadResistance/1000 * hcur_c);                               // 0.027 and 0.0185 are the resistances of the wires
                                                                            // going from the sockets to the board
    
    //If there is an overcurrent
    if (lcur_c >= 0.020 || hcur_c >= 7.5) {
      xSemaphoreTake(notify_mtx, portMAX_DELAY);
      //This if statement prevents more overcurrent notifications from being sent when
      //there's already a warning active on screen
      if (!warningActive) {
        warningActive = 1;
        xSemaphoreGive(notify_bin);
      }
      xSemaphoreGive(notify_mtx);
    }

    //The "averaging method" used here is almost like the boxcar averaging method. "avg_len" samples are gathered
    //into the measured values buffer, then their average is calculated and corresponded to the time of the newest sample.
    //When the next sample is gathered, the oldest sample is removed from the buffer, the average is calculated,
    //and it is drawn on the graph at the time value of that newly gathered sample... and so on and so fourth.

    //After the sum includes "avg_len" samples (at the start, when avg_ready = 1), we can begin
    //to subtract old samples from the sum and add new ones. To keep track of old samples, we need to use an array.
    if (!avg_ready) {
      ps_v_sum += ps_v;
      ps_v_arr[avg_idx] = ps_v;
      lcur_sum += lcur_c;
      lcur_arr[avg_idx] = lcur_c;
      hcur_sum += hcur_c;
      hcur_arr[avg_idx] = hcur_c;
      avg_idx++;
      if (avg_idx == avg_len) {
        avg_idx = 0;
        avg_ready = 1;
      }
    } else {
      //Replace the oldest sample in the cyclic array with the newest sample
      ps_v_sum += ps_v - ps_v_arr[avg_idx];
      ps_v_arr[avg_idx] = ps_v;
      lcur_sum += lcur_c - lcur_arr[avg_idx];
      lcur_arr[avg_idx] = lcur_c;
      hcur_sum += hcur_c - hcur_arr[avg_idx];
      hcur_arr[avg_idx] = hcur_c;
      if (avg_idx < avg_len - 1) {
        avg_idx++;
      } else {
        avg_idx = 0;
      }
    }

    //Set the data which is sent
    //Current range: 8A
    if (!currentRange) {
      //
      if (dataMode == 0) {
        sentData = (ps_v_sum / avg_len) * (hcur_sum / avg_len);
      } else if (dataMode == 1) {
        //sentData = (ps_v_sum / avg_len);
        sentData = (ps_v_sum / avg_len);
      } else if (dataMode == 2) {
        sentData = (hcur_sum / avg_len);
      }
    }
    //Current range: 20mA
    else {
      if (dataMode == 0) {
        sentData = (ps_v_sum / avg_len) * (lcur_sum / avg_len);
      } else if (dataMode == 1) {
        sentData = (ps_v_sum / avg_len);
      } else if (dataMode == 2) {
        //If only reading low current (in mA)
        sentData = (lcur_sum / avg_len) * 1000;
      }
    }

    //Delay which determines the sampling rate
    vTaskDelay(1000 / sampleRate / portTICK_PERIOD_MS);

    xSemaphoreGive(config_mtx);
    //END CRITICAL SECTION for config data

    //Adjust the gains accordingly
    ps_g = mngGain(ps_v_bin, ps_g);
    hcur_g = mngGain(hcur_v_bin, hcur_g);
    lcur_g = mngGain(lcur_v_bin, lcur_g);
    burd_g = mngGain(burd_v, burd_g);

    //This is where the measurement buffers are written to
    //The lock ensures that they aren't being read.
    //Also the shared resource buf_idx is protected here
    xSemaphoreTake(buf_access_mtx, portMAX_DELAY);
    //CRITICAL SECTION:

    //Storing voltage measurement:
    readings_buf[buf_idx] = sentData;
    time_buf[buf_idx] = now;
    
    if (buf_idx >= (buf_len - 1)) {
      //As soon as "buf_len"" samples have been read, unblock the WIFI task to broadcast the data
      xSemaphoreGive(buf_full);
      buf_idx = 0;
    } else {
      buf_idx++;
    }
    //END OF CRITICAL SECTION
    xSemaphoreGive(buf_access_mtx);
  }
}

//Arduino default setup function
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  wifi_mtx = xSemaphoreCreateMutex();

  if (wifi_mtx == NULL) {
    Serial.println("Couldn't create wifi mutex");
    ESP.restart();
  }

  buf_access_mtx = xSemaphoreCreateMutex();

  if (buf_access_mtx == NULL) {
    Serial.println("Couldn't create buffer access mutex");
    ESP.restart();
  }

  buf_full = xSemaphoreCreateBinary();

  if (buf_full == NULL) {
    Serial.println("Couldn't create binary semaphore");
    ESP.restart();
  }

  notify_bin = xSemaphoreCreateBinary();

  if (notify_bin == NULL) {
    Serial.println("Couldn't create binary semaphore");
    ESP.restart();
  }

  notify_mtx = xSemaphoreCreateMutex();

  if (notify_mtx == NULL) {
    Serial.println("Couldn't create overcurrent-warning-status mutex");
    ESP.restart();
  }

  config_mtx = xSemaphoreCreateMutex();

  if (config_mtx == NULL) {
    Serial.println("Couldn't create config settings mutex");
    ESP.restart();
  }

  //Set all callback functions
  //(ESP32 receives inputs from the client, which should run these arguments functions)
  setCallbacks(&setDataMode, &setCurrentRange, &setDomain, &removeWarning, &sendAllConfig, &setLeadResistance);

  //The stack sizes aren't optimized yet
  xTaskCreatePinnedToCore(
    wifiloop_task,
    "Wifi loop Task",
    10000,  //When given only 1024, the ESP32 would crash
    NULL,
    1,
    &wifiloopt_handle,
    0);
  xTaskCreatePinnedToCore(
    read_task,
    "Read Task",
    5000,  //1024 worked fine
    NULL,
    1,
    &readt_handle,
    1);
  xTaskCreatePinnedToCore(
    wifisend_task,
    "Wifi read Task",
    10000,  //"Stack canary watchpoint triggered" when at 1024
    NULL,
    1,
    &wifisendt_handle,
    0);
  xTaskCreatePinnedToCore(
    notify_task,
    "Wifi notify Task",
    5000,  //"Guru Mediation error" when at 1024
    NULL,
    2,  //Prioritize notification over other core 0 tasks
    &notifyt_handle,
    0);

  //DELETE setup/loop task
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
