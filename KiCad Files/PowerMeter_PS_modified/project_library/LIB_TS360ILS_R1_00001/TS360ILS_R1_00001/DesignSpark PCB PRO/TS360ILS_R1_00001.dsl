SamacSys ECAD Model
16330813/176765/2.49/4/3/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r155_90"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.9) (shapeHeight 1.55))
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
	(patternDef "SOP400P650X145-4N" (originalName "SOP400P650X145-4N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r155_90) (pt -2.975, 2) (rotation 90))
			(pad (padNum 2) (padStyleRef r155_90) (pt -2.975, -2) (rotation 90))
			(pad (padNum 3) (padStyleRef r155_90) (pt 2.975, -2) (rotation 90))
			(pad (padNum 4) (padStyleRef r155_90) (pt 2.975, 2) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4 2.825) (pt 4 2.825) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4 2.825) (pt 4 -2.825) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4 -2.825) (pt -4 -2.825) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4 -2.825) (pt -4 2.825) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.2 2.5) (pt 2.2 2.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.2 2.5) (pt 2.2 -2.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.2 -2.5) (pt -2.2 -2.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.2 -2.5) (pt -2.2 2.5) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.2 -1.5) (pt 1.8 2.5) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.85 2.5) (pt 1.85 2.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.85 2.5) (pt 1.85 -2.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.85 -2.5) (pt -1.85 -2.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.85 -2.5) (pt -1.85 2.5) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.75 2.8) (pt -2.2 2.8) (width 0.2))
		)
	)
	(symbolDef "TS360ILS_R1_00001" (originalName "TS360ILS_R1_00001")

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
	(compDef "TS360ILS_R1_00001" (originalName "TS360ILS_R1_00001") (compHeader (numPins 4) (numParts 1) (refDesPrefix IC)
		)
		(compPin "1" (pinName "+") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "-") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "4" (pinName "~_2") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "3" (pinName "~_1") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "TS360ILS_R1_00001"))
		(attachedPattern (patternNum 1) (patternName "SOP400P650X145-4N")
			(numPads 4)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
			)
		)
		(attr "Mouser Part Number" "241-TS360ILSR100001")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Panjit/TS360ILS_R1_00001?qs=sPbYRqrBIVlciIw6xf5DwA%3D%3D")
		(attr "Manufacturer_Name" "PANJIT")
		(attr "Manufacturer_Part_Number" "TS360ILS_R1_00001")
		(attr "Description" "Schottky Diodes & Rectifiers MICRO SURFACE MOUNT SCHOTTKY BRIDGE 60V 3A TDI")
		(attr "Datasheet Link" "https://www.mouser.co.id/datasheet/2/1057/TS360ILS-1877238.pdf")
		(attr "Height" "1.45 mm")
	)

)
