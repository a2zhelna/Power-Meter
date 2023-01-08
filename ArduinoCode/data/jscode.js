var array_length = 10;
var xValues = [];
for (let i = 0; i < array_length; i++) {
  xValues[i] = 1;
}

var min_xval = 0;
var min_yval = 0;
var max_xval = 10000;
var max_yval = 200;

var x_range = 5000;

//Amount of samples that are sent 
var samp_count = 100;

var yValues = [];

var rang2a = 0;
var rang2b = 0;

//0 - 8A
//1 - 20mA
var current_range = 0;
//0 - Power
//1 - Voltage
//2 - Current
var data_mode = 0;

var ovcurr = 0;

var avg_arr = [];
var avg = 0;

const rows = [ ["Time (ms)", "Value"] ];
var writingCSV = false;


//Refer to this repository for "rangeable" sliders: https://github.com/Mobius1/Rangeable

const rangeable = new Rangeable("input.slider1", {
	step: "1000",
  min:1000,
  max:10000,
  value: 5000,
  type: "single" 
});


function createRangeable2( max, limit_offset, step ) {
  rangeable2 = new Rangeable("input.slider2", {
    max: max,
    min: 0,
    step: step,
    value: [0, max],
    type: "double",
    multiple: true,
    controls: [{
      min: 0,
      max: max-limit_offset
    }, {
      min: limit_offset,
      max: max
    }],
    onChange: function(value) {
      if (rang2a != value[0] || rang2b != value[1]) {
        rang2a = value[0];
        rang2b = value[1];
      }
    }
  });
}

const myChart = new Chart("myChart", {
  type: 'scatter',
  borderColor: "#3bc1ed",
  tension: 0.1,
  options: {
    scales: {
      yAxes: [{
        ticks: {
          beginAtZero: true,
          min: min_yval,
          max: max_yval
      }}],
      xAxes: [{
        ticks: {
          beginAtZero: true,
          min: min_xval,
          max: max_xval
      }}]
    },
    animation: {
      duration: 0
    },
    elements: {
      point:{
        radius: 0
      }
    }
  },
  data: {
    datasets: [{
      label: 'Scatter Dataset',
      data: [
      ],
      showLine: true,
      fill: false,
      borderColor: 'rgba(70, 225, 105, 1)'
    }]
  }
});

function recCSV() {
  if (writingCSV == true) {
    writingCSV = false;
    document.getElementById("recordBTN").textContent = "Record";
    
  } else {
    writingCSV = true;
    rows.length = 0;
    rows.push(["Time (ms)", "Value"]);
    document.getElementById("recordBTN").textContent = "Stop";
    document.getElementById("csv_samples").innerHTML = 0;
  }
}

function stopWritingCSV() {
  writingCSV = false;
  document.getElementById("recordBTN").textContent = "Record";
}

function saveCSV() {
  let csvContent = "data:text/csv;charset=utf-8," 
    + rows.map(e => e.join(",")).join("\n");

  var encodedUri = encodeURI(csvContent);
  window.open(encodedUri);
}


function removeWarning() {
  console.log("Removing warning");
  overcurrentNoti("GOOD");
  var msg = { type: 'removeWarning', value: 0};
  Socket.send(JSON.stringify(msg)); 
}

function overcurrentNoti(e) {
  //Change overcurrent status
  if (e == "BAD") {
    document.getElementById("status_box").style.backgroundColor = "#D01414";
    document.getElementById("overcurrent_status").textContent = "BAD";
  }
  else if (e == "WARNING") {
    document.getElementById("status_box").style.backgroundColor = "#EB750E";
    document.getElementById("overcurrent_status").textContent = "WARNING";
    document.getElementById('warningBTN').classList.remove('greenResetBTN');
    document.getElementById('warningBTN').classList.add('orangeResetBTN');
    document.getElementById('warningBTN').onclick = removeWarning;
  }
  else {
    document.getElementById("status_box").style.backgroundColor = "#02A01D";    
    document.getElementById("overcurrent_status").textContent = "OK";
    document.getElementById('warningBTN').classList.remove('orangeResetBTN');
    document.getElementById('warningBTN').classList.add('greenResetBTN');
    document.getElementById('warningBTN').onclick = null;
  }
}

function changeDetails(uploadRate, sps, id) {
  document.getElementById("uploadRate").textContent = uploadRate + "Hz";
  document.getElementById("sps").textContent = sps;
  document.getElementById("deviceId").textContent = id;
}

var Socket;

function init() {
  createRangeable2( 200, 5, "5");
  document.getElementById("lead_resistance").value = 0;
  document.getElementById('warningBTN').onclick = null;
  document.getElementById('warningBTN').classList.add('greenResetBTN');

  Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  Socket.onmessage = function(event) {
    processCommand(event);
  };
  
}

function slider_changed () {
  var l_LED_intensity = slider.value;
  console.log(l_LED_intensity);
  var msg = { type: 'LED_intensity', value: l_LED_intensity};
  Socket.send(JSON.stringify(msg)); 
}

function changeLRes() {
  var leadRes = document.getElementById("lead_resistance").value;
  var msg = { type: 'LeadResistance', value: null, res: leadRes };
  Socket.send(JSON.stringify(msg));
}

function changeCurrentRange() {
  var option = document.getElementById("current_select").value;;

  if (option == 1) {
    console.log("SELECTED 20mA range");
    var msg = { type: 'CurrentRange', value: '20mA'};
    Socket.send(JSON.stringify(msg));
    current_range = 1;
    document.getElementById("currentModeText").innerHTML = "Current (mA)";
    changeDataMode();
  }
  else if (option == 0) {
    console.log("selected 8A RANGE");
    var msg = { type: 'CurrentRange', value: '8A'};
    Socket.send(JSON.stringify(msg));
    current_range = 0;
    document.getElementById("currentModeText").innerHTML = "Current (A)";    
    changeDataMode();
  }
}

function changeDataMode() {
  var option = document.getElementById("mode_select").value;

  if (option == 2) {
    console.log("Current");
    var msg = { type: 'DataMode', value: 'current'};
    Socket.send(JSON.stringify(msg));
    if (current_range == 0) {
      rangeable2.destroy();
      createRangeable2(10, 1, "1");
      document.getElementById("measurement_label").innerHTML = "Measurement (A):";
    }
    else {
      rangeable2.destroy();
      createRangeable2(20, 1, "1");
      document.getElementById("measurement_label").innerHTML = "Measurement (mA):";
    }
    changeYAxis();
    dataMode = 2;
  }
  else if (option == 1) {
    console.log("Voltage");
    var msg = { type: 'DataMode', value: 'voltage'};
    Socket.send(JSON.stringify(msg));
    rangeable2.destroy();
    createRangeable2(25, 1, "1");
    changeYAxis();
    dataMode = 1;
    document.getElementById("measurement_label").innerHTML = "Measurement (V):";
  }
  else if (option == 0) {
    console.log("POWER");
    var msg = { type: 'DataMode', value: 'power'};
    Socket.send(JSON.stringify(msg));
    if (current_range == 0) {
      rangeable2.destroy();
      createRangeable2(200, 5, "5");
    }
    else {
      rangeable2.destroy();
      createRangeable2(1, 1, "1");
    }
    document.getElementById("measurement_label").innerHTML = "Measurement (W):";
    changeYAxis();
    dataMode = 0;
  }
  var arr = myChart.data.datasets[0].data;
  while (arr.length != 0) {
    arr.shift();
  }
}

function changeYAxis() {
  let arr = rangeable2.getValue();

  let low = parseInt(arr[0]);
  let high = parseInt(arr[1]);

  myChart.options.scales.yAxes[0].ticks.min = low;
  myChart.options.scales.yAxes[0].ticks.max = high;

  myChart.update();
}

function changeXAxis() {
  let x = rangeable.getValue();

  console.log(x);
  switch(x) {
    //This doesn't change the xAxis the way I want!!!
    case "1000":
      x_range = 1000;
      //Sps 1000
      changeDetails(1000/samp_count, 1000, "");
      break;
    case "2000":
      x_range = 2000;
      //Sps 1000
      changeDetails(1000/samp_count, 1000, "");
      break;
    case "3000":
      x_range = 3000;
      //Sps 500
      changeDetails(500/samp_count, 500, "");
      break;
    case "4000": 
      x_range = 4000;
      //Sps 500
      changeDetails(500/samp_count, 500, "");
      break;
    case "5000":   
      x_range = 5000;
      //Sps 250
      changeDetails(250/samp_count, 250, "");
      break;
    case "6000":
      x_range = 6000;
      //Sps 250
      changeDetails(250/samp_count, 250, "");
      break;
    case "7000":
      x_range = 7000;
      //Sps 200
      changeDetails(200/samp_count, 200, "");
      break;
    case "8000":
      x_range = 8000;
      //Sps 200
      changeDetails(200/samp_count, 200, "");
      break;
    case "9000":
      x_range = 9000;
      //Sps 100
      changeDetails(100/samp_count, 100, "");
      break;
    case "10000":
      x_range = 10000;
      changeDetails(100/samp_count, 100, "");
      //Sps 100
      break;
    default:
      x_range = 10000;
      //Sps 100
      changeDetails(100/samp_count, 100, "");
      break;
  }
  //Tell ESP32 that the domain has changed
  var msg = { type: 'Domain', value: x_range.toString()};
  Socket.send(JSON.stringify(msg));

  //myChart.options.scales.xAxes[0].ticks.max = max_xval;
  //myChart.options.scales.xAxes[0].ticks.min = max_xval - x_range;

  myChart.update();
}

function processCommand(event) {
  var obj = JSON.parse(event.data);
  var type = obj.type;
  if (type.localeCompare("LED_intensity") == 0) { 
    //var l_LED_intensity = parseInt(obj.value); 
    //console.log(l_LED_intensity); 
    //slider.value = l_LED_intensity; 
    //output.innerHTML = l_LED_intensity;
  }
  else if (type.localeCompare("readings_update") == 0) {
    document.getElementById("voltage").innerHTML = obj.voltage;
    document.getElementById("highCurrent").innerHTML = obj.highCurrent;
    document.getElementById("lowCurrent").innerHTML = obj.lowCurrent;
  }
  else if (type.localeCompare("overcurrent_update") == 0) {
    if (obj.value == "0") {
      //All good
      overcurrentNoti("GOOD");
    }
    else if (obj.value == "1") {
      //Close to the current limit
      overcurrentNoti("WARNING");
    }
    else {
      overcurrentNoti("BAD");
    }
  }
  else if (type.localeCompare("client_connected") == 0) {
    rangeable.setValue(obj.domain);
    changeXAxis();
    //Using the unary + operator because the received current range is a boolean (to save memory)
    document.getElementById("current_select").value = +obj.current_range;
    document.getElementById("mode_select").value = parseInt(obj.data_mode);
    document.getElementById("lead_resistance").value = parseFloat(obj.lead_res);
    changeCurrentRange();
  }
  else if (type.localeCompare("graph_update") == 0) { 
    //console.log(obj.value);
    //console.log(obj.time);
    avg = 0;
    for (let i = 0; i < samp_count; i++) {
      myChart.data.datasets[0].data.push({
        x: obj.time[i],
        y: obj.value[i]
      });
      if (i%10 == 0) {
        //Average up every 10th sample
        avg_arr[i/10] = obj.value[i];
      }
      if (writingCSV == true) {
        rows.push([obj.time[i], obj.value[i]]);
        var rowAmt = rows.length;
        if ( (rows.length % 1000) == 0) {
          //Update "Current Amount of Samples Stored"
          //Check if max csv sample amount has been reached
          document.getElementById("csv_samples").innerHTML = rowAmt;
          if (rowAmt >= 100000) {
            //Stop writing to CSV file
            stopWritingCSV();
          }
        }
      }
    }
    for (let i = 0; i < samp_count/10; i++) {
      avg += avg_arr[i];
    }
    avg /= (samp_count/10);
    document.getElementById("measurement").innerHTML = avg.toFixed(3);

    //this large value (currently 10 000) should be set such that
    //there aren't too many data points on the graph
    //otherwise, delays will be introduced. 
    //The choppiness/delay begins to occur a little bit past
    //5000 samples
    if (obj.time[samp_count-1] > x_range) {
      //Increase the x-axis range to be the last time value 
      max_xval = obj.time[samp_count-1];
      //Decrease the domain so that its magnitude is 10 000
      min_xval = obj.time[samp_count-1] - x_range;
      
      var idx = 0;
      //I need to only compare the x values of the data with max_val -10 000
      while (myChart.data.datasets[0].data[idx].x < min_xval) {
          idx++;
      }
      //Temp check
      for (let i = 0; i < idx; i++) {
        myChart.data.datasets[0].data.shift();
      }
    }
      myChart.options.scales.xAxes[0].ticks.max = max_xval;
      myChart.options.scales.xAxes[0].ticks.min = min_xval;
      //myChart.data.datasets[0].data.
    
    //myChart.data.datasets[0].data = obj.value;
    myChart.update();
  }

}
window.onload = function(event) {
  init();
}
