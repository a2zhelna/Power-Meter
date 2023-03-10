SamacSys ECAD Model
13646505/176765/2.49/4/3/Bridge Rectifier

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r185_85"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.85) (shapeHeight 1.85))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "SOP250P700X290-4N" (originalName "SOP250P700X290-4N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r185_85) (pt -3.025, 1.25) (rotation 90))
			(pad (padNum 2) (padStyleRef r185_85) (pt -3.025, -1.25) (rotation 90))
			(pad (padNum 3) (padStyleRef r185_85) (pt 3.025, -1.25) (rotation 90))
			(pad (padNum 4) (padStyleRef r185_85) (pt 3.025, 1.25) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4.2 2.725) (pt 4.2 2.725) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4.2 2.725) (pt 4.2 -2.725) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4.2 -2.725) (pt -4.2 -2.725) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4.2 -2.725) (pt -4.2 2.725) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.925 2.362) (pt 1.925 2.362) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.925 2.362) (pt 1.925 -2.362) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.925 -2.362) (pt -1.925 -2.362) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.925 -2.362) (pt -1.925 2.362) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.925 -0.138) (pt 0.575 2.362) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.75 2.362) (pt 1.75 2.362) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.75 2.362) (pt 1.75 -2.362) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.75 -2.362) (pt -1.75 -2.362) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.75 -2.362) (pt -1.75 2.362) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.95 2.025) (pt -2.1 2.025) (width 0.2))
		)
	)
	(symbolDef "KMB28STR" (originalName "KMB28STR")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 900 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 900 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 700 mils 100 mils) (width 6 mils))
		(line (pt 700 mils 100 mils) (pt 700 mils -200 mils) (width 6 mils))
		(line (pt 700 mils -200 mils) (pt 200 mils -200 mils) (width 6 mils))
		(line (pt 200 mils -200 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 750 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "KMB28STR" (originalName "KMB28STR") (compHeader (numPins 4) (numParts 1) (refDesPrefix BR)
		)
		(compPin "1" (pinName "+") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "-") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "4" (pinName "~_2") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "3" (pinName "~_1") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "KMB28STR"))
		(attachedPattern (patternNum 1) (patternName "SOP250P700X290-4N")
			(numPads 4)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
			)
		)
		(attr "Mouser Part Number" "")
		(attr "Mouser Price/Stock" "")
		(attr "Manufacturer_Name" "SMC Diode Solutions")
		(attr "Manufacturer_Part_Number" "KMB28STR")
		(attr "Description" "BRIDGE RECT 1PHASE 80V 2A MBS")
		(attr "Datasheet Link" "http://www.smc-diodes.com/propdf/KMB22S%20THRU%20KMB225S%20N1952%20REV.A.pdf")
		(attr "Height" "2.9 mm")
	)

)
