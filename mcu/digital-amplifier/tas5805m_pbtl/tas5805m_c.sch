EESchema Schematic File Version 4
LIBS:tas5805m-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L my:tas5805m U?
U 1 1 5EB62641
P 6200 3150
AR Path="/5EB62641" Ref="U?"  Part="1" 
AR Path="/5EB2DBEF/5EB62641" Ref="U?"  Part="1" 
AR Path="/5EB27D00/5EB62641" Ref="U?"  Part="1" 
F 0 "U?" H 6150 4065 50  0000 C CNN
F 1 "tas5805m" H 6150 3974 50  0000 C CNN
F 2 "Package_SO:HTSSOP-28-1EP_4.4x9.7mm_P0.65mm_EP3.4x9.5mm" H 5900 3600 50  0001 C CNN
F 3 "" H 5900 3600 50  0001 C CNN
	1    6200 3150
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 5EB62648
P 7250 1800
AR Path="/5EB62648" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB62648" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB62648" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7250 1650 50  0001 C CNN
F 1 "VCC" H 7267 1973 50  0000 C CNN
F 2 "" H 7250 1800 50  0001 C CNN
F 3 "" H 7250 1800 50  0001 C CNN
	1    7250 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2600 6900 2600
Wire Wire Line
	6700 2700 6900 2700
Wire Wire Line
	6900 2700 6900 2600
Connection ~ 6900 2600
$Comp
L Device:C_Small C?
U 1 1 5EB62652
P 7650 2050
AR Path="/5EB62652" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62652" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62652" Ref="C?"  Part="1" 
F 0 "C?" H 7742 2096 50  0000 L CNN
F 1 "22uf" H 7742 2005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7650 2050 50  0001 C CNN
F 3 "~" H 7650 2050 50  0001 C CNN
	1    7650 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB62659
P 8000 2050
AR Path="/5EB62659" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62659" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62659" Ref="C?"  Part="1" 
F 0 "C?" H 8092 2096 50  0000 L CNN
F 1 "0.1uf" H 8092 2005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8000 2050 50  0001 C CNN
F 3 "~" H 8000 2050 50  0001 C CNN
	1    8000 2050
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C?
U 1 1 5EB62660
P 7250 2050
AR Path="/5EB62660" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62660" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62660" Ref="C?"  Part="1" 
F 0 "C?" H 7341 2096 50  0000 L CNN
F 1 "390uf" H 7341 2005 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D7.5mm_P2.50mm" H 7250 2050 50  0001 C CNN
F 3 "~" H 7250 2050 50  0001 C CNN
	1    7250 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 1800 7250 1950
Wire Wire Line
	7250 1950 7650 1950
Connection ~ 7250 1950
Wire Wire Line
	7650 1950 8000 1950
Connection ~ 7650 1950
Wire Wire Line
	6900 1950 7250 1950
Wire Wire Line
	6900 1950 6900 2600
$Comp
L power:GND #PWR?
U 1 1 5EB6266E
P 7250 2350
AR Path="/5EB6266E" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB6266E" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB6266E" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7250 2100 50  0001 C CNN
F 1 "GND" H 7255 2177 50  0000 C CNN
F 2 "" H 7250 2350 50  0001 C CNN
F 3 "" H 7250 2350 50  0001 C CNN
	1    7250 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7250 2350 7250 2150
Wire Wire Line
	7650 2150 8000 2150
Wire Wire Line
	7250 2150 7650 2150
Connection ~ 7250 2150
Connection ~ 7650 2150
$Comp
L power:VCC #PWR?
U 1 1 5EB62679
P 7400 4350
AR Path="/5EB62679" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB62679" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB62679" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7400 4200 50  0001 C CNN
F 1 "VCC" H 7417 4523 50  0000 C CNN
F 2 "" H 7400 4350 50  0001 C CNN
F 3 "" H 7400 4350 50  0001 C CNN
	1    7400 4350
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB6267F
P 7800 4600
AR Path="/5EB6267F" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB6267F" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB6267F" Ref="C?"  Part="1" 
F 0 "C?" H 7892 4646 50  0000 L CNN
F 1 "22uf" H 7892 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7800 4600 50  0001 C CNN
F 3 "~" H 7800 4600 50  0001 C CNN
	1    7800 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB62686
P 8150 4600
AR Path="/5EB62686" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62686" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62686" Ref="C?"  Part="1" 
F 0 "C?" H 8242 4646 50  0000 L CNN
F 1 "0.1uf" H 8242 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8150 4600 50  0001 C CNN
F 3 "~" H 8150 4600 50  0001 C CNN
	1    8150 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C?
U 1 1 5EB6268D
P 7400 4600
AR Path="/5EB6268D" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB6268D" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB6268D" Ref="C?"  Part="1" 
F 0 "C?" H 7491 4646 50  0000 L CNN
F 1 "390uf" H 7491 4555 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D7.5mm_P2.50mm" H 7400 4600 50  0001 C CNN
F 3 "~" H 7400 4600 50  0001 C CNN
	1    7400 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 4350 7400 4500
Wire Wire Line
	7400 4500 7800 4500
Connection ~ 7400 4500
Wire Wire Line
	7800 4500 8150 4500
Connection ~ 7800 4500
$Comp
L power:GND #PWR?
U 1 1 5EB62699
P 7400 4900
AR Path="/5EB62699" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB62699" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB62699" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 7400 4650 50  0001 C CNN
F 1 "GND" H 7405 4727 50  0000 C CNN
F 2 "" H 7400 4900 50  0001 C CNN
F 3 "" H 7400 4900 50  0001 C CNN
	1    7400 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 4900 7400 4700
Wire Wire Line
	7800 4700 8150 4700
Wire Wire Line
	7400 4700 7800 4700
Connection ~ 7400 4700
Connection ~ 7800 4700
Wire Wire Line
	6900 3800 6900 3900
Connection ~ 6900 3900
Wire Wire Line
	6900 4500 7400 4500
Wire Wire Line
	6700 3800 6900 3800
Wire Wire Line
	6700 3900 6900 3900
$Comp
L power:VDD #PWR?
U 1 1 5EB626A9
P 4750 1700
AR Path="/5EB626A9" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB626A9" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB626A9" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4750 1550 50  0001 C CNN
F 1 "VDD" H 4767 1873 50  0000 C CNN
F 2 "" H 4750 1700 50  0001 C CNN
F 3 "" H 4750 1700 50  0001 C CNN
	1    4750 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB626AF
P 3800 2000
AR Path="/5EB626AF" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626AF" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626AF" Ref="C?"  Part="1" 
F 0 "C?" H 3892 2046 50  0000 L CNN
F 1 "4.7uf" H 3892 1955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3800 2000 50  0001 C CNN
F 3 "~" H 3800 2000 50  0001 C CNN
	1    3800 2000
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB626B6
P 4250 2000
AR Path="/5EB626B6" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626B6" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626B6" Ref="C?"  Part="1" 
F 0 "C?" H 4342 2046 50  0000 L CNN
F 1 "0.1uf" H 4342 1955 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4250 2000 50  0001 C CNN
F 3 "~" H 4250 2000 50  0001 C CNN
	1    4250 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 1700 4750 1900
Wire Wire Line
	3800 2100 4100 2100
Wire Wire Line
	5200 2700 5200 1900
Wire Wire Line
	5200 2700 5600 2700
Wire Wire Line
	6700 3000 6750 3000
Wire Wire Line
	6700 3500 6750 3500
$Comp
L power:GND #PWR?
U 1 1 5EB626C3
P 4100 2350
AR Path="/5EB626C3" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB626C3" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB626C3" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 4100 2100 50  0001 C CNN
F 1 "GND" H 4105 2177 50  0000 C CNN
F 2 "" H 4100 2350 50  0001 C CNN
F 3 "" H 4100 2350 50  0001 C CNN
	1    4100 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2350 4100 2100
Connection ~ 4100 2100
Wire Wire Line
	4100 2100 4250 2100
$Comp
L Device:C_Small C?
U 1 1 5EB626CC
P 7700 2900
AR Path="/5EB626CC" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626CC" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626CC" Ref="C?"  Part="1" 
F 0 "C?" V 7750 2750 50  0000 C CNN
F 1 "0.22uf" V 7750 3050 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7700 2900 50  0001 C CNN
F 3 "~" H 7700 2900 50  0001 C CNN
	1    7700 2900
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB626D3
P 7700 3200
AR Path="/5EB626D3" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626D3" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626D3" Ref="C?"  Part="1" 
F 0 "C?" V 7650 3050 50  0000 C CNN
F 1 "0.22uf" V 7750 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7700 3200 50  0001 C CNN
F 3 "~" H 7700 3200 50  0001 C CNN
	1    7700 3200
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB626DA
P 7700 3600
AR Path="/5EB626DA" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626DA" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626DA" Ref="C?"  Part="1" 
F 0 "C?" V 7600 3500 50  0000 C CNN
F 1 "0.22uf" V 7600 3800 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7700 3600 50  0001 C CNN
F 3 "~" H 7700 3600 50  0001 C CNN
	1    7700 3600
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB626E1
P 8450 3300
AR Path="/5EB626E1" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB626E1" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB626E1" Ref="C?"  Part="1" 
F 0 "C?" V 8350 3150 50  0000 C CNN
F 1 "0.22uf" V 8350 3450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8450 3300 50  0001 C CNN
F 3 "~" H 8450 3300 50  0001 C CNN
	1    8450 3300
	0    1    1    0   
$EndComp
Wire Wire Line
	6700 3400 8700 3400
Wire Wire Line
	6700 3100 7950 3100
Wire Wire Line
	6700 2800 7950 2800
Wire Wire Line
	6700 2900 7600 2900
Wire Wire Line
	7800 2900 7950 2900
Wire Wire Line
	7950 2900 7950 2800
Connection ~ 7950 2800
Wire Wire Line
	7950 2800 9050 2800
Wire Wire Line
	6700 3200 7600 3200
Wire Wire Line
	7800 3200 7950 3200
Wire Wire Line
	7950 3200 7950 3100
Connection ~ 7950 3100
Wire Wire Line
	7950 3100 9050 3100
Wire Wire Line
	6700 3300 8350 3300
Wire Wire Line
	8550 3300 8700 3300
Wire Wire Line
	8700 3300 8700 3400
Connection ~ 8700 3400
Wire Wire Line
	8700 3400 9050 3400
Wire Wire Line
	6700 3600 7600 3600
Wire Wire Line
	6700 3700 7950 3700
Wire Wire Line
	7800 3600 7950 3600
Wire Wire Line
	7950 3600 7950 3700
Connection ~ 7950 3700
Wire Wire Line
	7950 3700 9050 3700
$Comp
L Device:C_Small C?
U 1 1 5EB62700
P 5350 4550
AR Path="/5EB62700" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62700" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62700" Ref="C?"  Part="1" 
F 0 "C?" H 5442 4596 50  0000 L CNN
F 1 "1uf" H 5442 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5350 4550 50  0001 C CNN
F 3 "~" H 5350 4550 50  0001 C CNN
	1    5350 4550
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5EB62707
P 4750 4550
AR Path="/5EB62707" Ref="C?"  Part="1" 
AR Path="/5EB2DBEF/5EB62707" Ref="C?"  Part="1" 
AR Path="/5EB27D00/5EB62707" Ref="C?"  Part="1" 
F 0 "C?" H 4842 4596 50  0000 L CNN
F 1 "1uf" H 4842 4505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4750 4550 50  0001 C CNN
F 3 "~" H 4750 4550 50  0001 C CNN
	1    4750 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 4650 5100 4650
$Comp
L power:GND #PWR?
U 1 1 5EB6270F
P 5100 4950
AR Path="/5EB6270F" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB6270F" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB6270F" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 5100 4700 50  0001 C CNN
F 1 "GND" H 5105 4777 50  0000 C CNN
F 2 "" H 5100 4950 50  0001 C CNN
F 3 "" H 5100 4950 50  0001 C CNN
	1    5100 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5100 4950 5100 4650
Connection ~ 5100 4650
Wire Wire Line
	5100 4650 5350 4650
Wire Wire Line
	5350 4450 5350 3800
Wire Wire Line
	5350 3800 5600 3800
$Comp
L Device:R R?
U 1 1 5EB6271A
P 4750 2200
AR Path="/5EB6271A" Ref="R?"  Part="1" 
AR Path="/5EB2DBEF/5EB6271A" Ref="R?"  Part="1" 
AR Path="/5EB27D00/5EB6271A" Ref="R?"  Part="1" 
F 0 "R?" H 4820 2246 50  0000 L CNN
F 1 "15k" H 4820 2155 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4680 2200 50  0001 C CNN
F 3 "~" H 4750 2200 50  0001 C CNN
	1    4750 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EB62721
P 5500 4950
AR Path="/5EB62721" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB62721" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB62721" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 5500 4700 50  0001 C CNN
F 1 "GND" H 5505 4777 50  0000 C CNN
F 2 "" H 5500 4950 50  0001 C CNN
F 3 "" H 5500 4950 50  0001 C CNN
	1    5500 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 3900 5500 3900
Wire Wire Line
	4750 2900 4750 4450
Wire Wire Line
	4750 2900 5600 2900
Wire Wire Line
	5500 3000 5500 3900
Wire Wire Line
	5500 3000 5600 3000
Connection ~ 5500 3900
Wire Wire Line
	5500 3900 5500 4950
Wire Wire Line
	5500 2600 5500 3000
Wire Wire Line
	5500 2600 5600 2600
Connection ~ 5500 3000
Wire Wire Line
	4750 2050 4750 1900
Wire Wire Line
	4750 2350 4750 2800
Wire Wire Line
	6900 3900 6900 4500
Wire Wire Line
	6700 4050 6750 4050
Wire Wire Line
	6750 3000 6750 3500
Connection ~ 6750 3500
Wire Wire Line
	6750 3500 6750 4050
Connection ~ 6750 4050
Wire Wire Line
	6750 4050 6750 4500
$Comp
L power:GND #PWR?
U 1 1 5EB6273A
P 6750 4500
AR Path="/5EB6273A" Ref="#PWR?"  Part="1" 
AR Path="/5EB2DBEF/5EB6273A" Ref="#PWR?"  Part="1" 
AR Path="/5EB27D00/5EB6273A" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 6750 4250 50  0001 C CNN
F 1 "GND" H 6755 4327 50  0000 C CNN
F 2 "" H 6750 4500 50  0001 C CNN
F 3 "" H 6750 4500 50  0001 C CNN
	1    6750 4500
	1    0    0    -1  
$EndComp
Text HLabel 2900 3100 0    50   Input ~ 0
LRCLK
Text HLabel 2900 3200 0    50   Input ~ 0
SCLK
Text HLabel 2900 3300 0    50   Input ~ 0
SDIN
Text HLabel 2900 3500 0    50   Input ~ 0
SDA
Text HLabel 2900 3600 0    50   Input ~ 0
SCL
Text HLabel 2900 3700 0    50   Input ~ 0
PDN
Connection ~ 4250 1900
Wire Wire Line
	3800 1900 4250 1900
Wire Wire Line
	4250 1900 4750 1900
Connection ~ 4750 1900
Wire Wire Line
	4750 2800 5600 2800
Wire Wire Line
	4750 1900 5200 1900
Connection ~ 4750 2800
Text HLabel 2900 2800 0    50   Output ~ 0
ADR
Text HLabel 9050 2800 2    50   Output ~ 0
OUT_A+
Text HLabel 9050 3100 2    50   Output ~ 0
OUT_A-
Text HLabel 9050 3700 2    50   Output ~ 0
OUT_B+
Text HLabel 9050 3400 2    50   Output ~ 0
OUT_B-
Wire Wire Line
	2900 2800 3400 2800
Wire Wire Line
	2900 3100 5600 3100
Wire Wire Line
	2900 3200 5600 3200
Wire Wire Line
	2900 3300 5600 3300
Wire Wire Line
	2900 3500 5600 3500
Wire Wire Line
	2900 3600 5600 3600
Wire Wire Line
	2900 3700 5600 3700
$Comp
L Connector_Generic:Conn_01x01 J?
U 1 1 5EB62759
P 2000 3400
F 0 "J?" H 1920 3175 50  0000 C CNN
F 1 "sdout2" H 1920 3266 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 2000 3400 50  0001 C CNN
F 3 "~" H 2000 3400 50  0001 C CNN
	1    2000 3400
	-1   0    0    1   
$EndComp
Wire Wire Line
	2200 3400 5600 3400
$Comp
L Device:R R?
U 1 1 5EB62761
P 3400 2400
F 0 "R?" H 3500 2450 50  0000 L CNN
F 1 "9.53k" H 3500 2300 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3330 2400 50  0001 C CNN
F 3 "~" H 3400 2400 50  0001 C CNN
	1    3400 2400
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR?
U 1 1 5EB62768
P 3400 2000
F 0 "#PWR?" H 3400 1850 50  0001 C CNN
F 1 "VDD" H 3417 2173 50  0000 C CNN
F 2 "" H 3400 2000 50  0001 C CNN
F 3 "" H 3400 2000 50  0001 C CNN
	1    3400 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 2550 3400 2800
Connection ~ 3400 2800
Wire Wire Line
	3400 2800 4750 2800
Wire Wire Line
	3400 2000 3400 2250
$EndSCHEMATC
