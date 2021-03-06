EESchema Schematic File Version 4
LIBS:tas5805m-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 8
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	1800 2250 2250 2250
Wire Wire Line
	1800 2350 2250 2350
Wire Wire Line
	1800 2450 2250 2450
Text Label 1950 2250 0    50   ~ 0
LRCLK_2
Text Label 1950 2350 0    50   ~ 0
SCLK_2
Text Label 1950 2450 0    50   ~ 0
SDIN_2
$Comp
L power:GND #PWR?
U 1 1 5EC59412
P 3100 2850
AR Path="/5EC2A7FE/5EC59412" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC59412" Ref="#PWR0124"  Part="1" 
F 0 "#PWR0124" H 3100 2600 50  0001 C CNN
F 1 "GND" H 3105 2677 50  0000 C CNN
F 2 "" H 3100 2850 50  0001 C CNN
F 3 "" H 3100 2850 50  0001 C CNN
	1    3100 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x05 J?
U 1 1 5EC5941A
P 1600 2450
AR Path="/5EC2A7FE/5EC5941A" Ref="J?"  Part="1" 
AR Path="/5EC576F2/5EC5941A" Ref="J9"  Part="1" 
F 0 "J9" H 1520 2025 50  0000 C CNN
F 1 "i2s_bt" H 1520 2116 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 1600 2450 50  0001 C CNN
F 3 "~" H 1600 2450 50  0001 C CNN
	1    1600 2450
	-1   0    0    1   
$EndComp
$Comp
L my:LP5907 U?
U 1 1 5EC59421
P 2000 1500
AR Path="/5EC2A7FE/5EC59421" Ref="U?"  Part="1" 
AR Path="/5EC576F2/5EC59421" Ref="U9"  Part="1" 
F 0 "U9" H 2000 1825 50  0000 C CNN
F 1 "LP5907" H 2000 1734 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 1950 1550 50  0001 C CNN
F 3 "" H 1950 1550 50  0001 C CNN
	1    2000 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 1400 2500 1400
$Comp
L Device:C_Small C?
U 1 1 5EC59429
P 3100 1500
AR Path="/5EC2A7FE/5EC59429" Ref="C?"  Part="1" 
AR Path="/5EC576F2/5EC59429" Ref="C56"  Part="1" 
F 0 "C56" H 3192 1546 50  0000 L CNN
F 1 "1uf" H 3192 1455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3100 1500 50  0001 C CNN
F 3 "~" H 3100 1500 50  0001 C CNN
	1    3100 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2550 2500 2550
Connection ~ 2500 1400
Wire Wire Line
	2500 1400 3100 1400
Wire Wire Line
	1700 1400 1350 1400
Wire Wire Line
	1350 1400 1350 1200
$Comp
L power:+5V #PWR?
U 1 1 5EC59439
P 1350 1200
AR Path="/5EC2A7FE/5EC59439" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC59439" Ref="#PWR0125"  Part="1" 
F 0 "#PWR0125" H 1350 1050 50  0001 C CNN
F 1 "+5V" H 1365 1373 50  0000 C CNN
F 2 "" H 1350 1200 50  0001 C CNN
F 3 "" H 1350 1200 50  0001 C CNN
	1    1350 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 1500 1350 1500
Wire Wire Line
	1350 1500 1350 1750
$Comp
L power:GND #PWR?
U 1 1 5EC59441
P 1350 1750
AR Path="/5EC2A7FE/5EC59441" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC59441" Ref="#PWR0126"  Part="1" 
F 0 "#PWR0126" H 1350 1500 50  0001 C CNN
F 1 "GND" H 1355 1577 50  0000 C CNN
F 2 "" H 1350 1750 50  0001 C CNN
F 3 "" H 1350 1750 50  0001 C CNN
	1    1350 1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 1600 1700 1850
Wire Wire Line
	1700 1850 2100 1850
Text Label 1800 1850 0    50   ~ 0
on_bt
Text HLabel 4950 3300 0    50   Input ~ 0
LRCLK
Text HLabel 4950 3600 0    50   Input ~ 0
SCLK
Text HLabel 6650 3700 2    50   Input ~ 0
SDOUT
Text HLabel 4950 3000 0    50   Input ~ 0
EN_bt
Wire Wire Line
	4950 3000 5450 3000
Wire Wire Line
	6650 3700 6150 3700
Wire Wire Line
	4950 3600 5450 3600
Wire Wire Line
	4950 3300 5450 3300
$Comp
L power:GND #PWR?
U 1 1 5EC437D2
P 5450 3950
AR Path="/5EC2A7FE/5EC437D2" Ref="#PWR?"  Part="1" 
AR Path="/5EC4DC55/5EC437D2" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC437D2" Ref="#PWR0127"  Part="1" 
F 0 "#PWR0127" H 5450 3700 50  0001 C CNN
F 1 "GND" H 5455 3777 50  0000 C CNN
F 2 "" H 5450 3950 50  0001 C CNN
F 3 "" H 5450 3950 50  0001 C CNN
	1    5450 3950
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR?
U 1 1 5EC437DA
P 6150 2600
AR Path="/5EC2A7FE/5EC437DA" Ref="#PWR?"  Part="1" 
AR Path="/5EC4DC55/5EC437DA" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC437DA" Ref="#PWR0128"  Part="1" 
F 0 "#PWR0128" H 6150 2450 50  0001 C CNN
F 1 "VDD" H 6167 2773 50  0000 C CNN
F 2 "" H 6150 2600 50  0001 C CNN
F 3 "" H 6150 2600 50  0001 C CNN
	1    6150 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 2600 6150 3000
Wire Wire Line
	5450 3200 4650 3200
Wire Wire Line
	5450 3500 4650 3500
Wire Wire Line
	6950 3600 6150 3600
Wire Wire Line
	6150 3500 6850 3500
Wire Wire Line
	4750 3400 5450 3400
Wire Wire Line
	4750 3100 5450 3100
Text Label 5150 3200 2    50   ~ 0
LRCLK_2
Text Label 5150 3500 2    50   ~ 0
SCLK_2
Text Label 6650 3600 2    50   ~ 0
SDIN_2
Text Label 4850 3100 0    50   ~ 0
LRCLK_1
Text Label 4850 3400 0    50   ~ 0
SCLK_1
Text Label 6750 3500 2    50   ~ 0
SDIN_1
Text HLabel 6650 3100 2    50   Input ~ 0
EN_usb
Wire Wire Line
	6650 3100 6150 3100
Wire Wire Line
	1700 3850 2150 3850
Wire Wire Line
	1700 3950 2150 3950
Wire Wire Line
	1700 4050 2150 4050
Text Label 1750 3850 0    50   ~ 0
LRCLK_1
Text Label 1750 3950 0    50   ~ 0
SCLK_1
Text Label 1750 4050 0    50   ~ 0
SDIN_1
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5EC4381B
P 1500 4050
AR Path="/5EC2A7FE/5EC4381B" Ref="J?"  Part="1" 
AR Path="/5EC4DC55/5EC4381B" Ref="J?"  Part="1" 
AR Path="/5EC576F2/5EC4381B" Ref="J8"  Part="1" 
F 0 "J8" H 1420 3625 50  0000 C CNN
F 1 "i2s1" H 1420 3716 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 1500 4050 50  0001 C CNN
F 3 "~" H 1500 4050 50  0001 C CNN
	1    1500 4050
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5EC43822
P 1950 4350
AR Path="/5EC2A7FE/5EC43822" Ref="#PWR?"  Part="1" 
AR Path="/5EC4DC55/5EC43822" Ref="#PWR?"  Part="1" 
AR Path="/5EC576F2/5EC43822" Ref="#PWR0129"  Part="1" 
F 0 "#PWR0129" H 1950 4100 50  0001 C CNN
F 1 "GND" H 1955 4177 50  0000 C CNN
F 2 "" H 1950 4350 50  0001 C CNN
F 3 "" H 1950 4350 50  0001 C CNN
	1    1950 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1700 4150 1950 4150
Wire Wire Line
	1950 4150 1950 4350
Wire Wire Line
	5450 3700 5450 3950
$Comp
L tas5805m-rescue:SN74CB3Q3257-my U?
U 1 1 5EDF55FA
P 5800 3200
AR Path="/5EDF55FA" Ref="U?"  Part="1" 
AR Path="/5EC576F2/5EDF55FA" Ref="U10"  Part="1" 
F 0 "U10" H 5800 3715 50  0000 C CNN
F 1 "SN74CB3Q3257" H 5800 3624 50  0000 C CNN
F 2 "Package_SO:TSSOP-16_4.4x5mm_P0.65mm" H 5800 3200 50  0001 C CNN
F 3 "" H 5800 3200 50  0001 C CNN
	1    5800 3200
	1    0    0    -1  
$EndComp
NoConn ~ 6150 3200
NoConn ~ 6150 3300
NoConn ~ 6150 3400
Text Label 5050 3000 0    50   ~ 0
on_bt
Wire Wire Line
	1800 2650 3100 2650
Connection ~ 3100 2650
Wire Wire Line
	3100 2650 3100 2850
Wire Wire Line
	3100 1600 3100 2650
Wire Wire Line
	2500 1400 2500 2550
$EndSCHEMATC
