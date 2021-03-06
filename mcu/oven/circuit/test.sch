EESchema Schematic File Version 4
LIBS:test-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 2
Title ""
Date ""
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Crystal Y2
U 1 1 5DA07972
P 5050 2200
F 0 "Y2" V 5004 2331 50  0000 L CNN
F 1 "32.768" V 5095 2331 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_5032-2Pin_5.0x3.2mm_HandSoldering" H 5050 2200 50  0001 C CNN
F 3 "~" H 5050 2200 50  0001 C CNN
	1    5050 2200
	0    1    1    0   
$EndComp
$Comp
L Device:Crystal Y1
U 1 1 5DA079DF
P 5050 1300
F 0 "Y1" V 5004 1431 50  0000 L CNN
F 1 "8m" V 5095 1431 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_5032-2Pin_5.0x3.2mm_HandSoldering" H 5050 1300 50  0001 C CNN
F 3 "~" H 5050 1300 50  0001 C CNN
	1    5050 1300
	0    1    1    0   
$EndComp
$Comp
L Device:C C5
U 1 1 5DA07B89
P 4750 1900
F 0 "C5" V 4498 1900 50  0000 C CNN
F 1 "12pf" V 4589 1900 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4788 1750 50  0001 C CNN
F 3 "~" H 4750 1900 50  0001 C CNN
	1    4750 1900
	0    1    1    0   
$EndComp
$Comp
L Device:C C6
U 1 1 5DA07BE6
P 4750 2350
F 0 "C6" V 4498 2350 50  0000 C CNN
F 1 "12pf" V 4589 2350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4788 2200 50  0001 C CNN
F 3 "~" H 4750 2350 50  0001 C CNN
	1    4750 2350
	0    1    1    0   
$EndComp
$Comp
L Device:C C3
U 1 1 5DA07C45
P 4750 1000
F 0 "C3" V 4498 1000 50  0000 C CNN
F 1 "22pf" V 4589 1000 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4788 850 50  0001 C CNN
F 3 "~" H 4750 1000 50  0001 C CNN
	1    4750 1000
	0    1    1    0   
$EndComp
$Comp
L Device:C C4
U 1 1 5DA07CFC
P 4750 1450
F 0 "C4" V 4498 1450 50  0000 C CNN
F 1 "22pf" V 4589 1450 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4788 1300 50  0001 C CNN
F 3 "~" H 4750 1450 50  0001 C CNN
	1    4750 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 1450 5050 1450
Connection ~ 5050 1450
Wire Wire Line
	4900 2350 5050 2350
Connection ~ 5050 2350
$Comp
L power:GND #PWR07
U 1 1 5DA09FFF
P 4200 1500
F 0 "#PWR07" H 4200 1250 50  0001 C CNN
F 1 "GND" H 4205 1327 50  0000 C CNN
F 2 "" H 4200 1500 50  0001 C CNN
F 3 "" H 4200 1500 50  0001 C CNN
	1    4200 1500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5DA0A3F7
P 7800 2050
F 0 "R3" V 7593 2050 50  0000 C CNN
F 1 "10k" V 7684 2050 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 7730 2050 50  0001 C CNN
F 3 "~" H 7800 2050 50  0001 C CNN
	1    7800 2050
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5DA0AA29
P 8150 1500
F 0 "R4" H 8220 1546 50  0000 L CNN
F 1 "10k" H 8220 1455 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 8080 1500 50  0001 C CNN
F 3 "~" H 8150 1500 50  0001 C CNN
	1    8150 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1650 8150 1850
Wire Wire Line
	8150 1850 8300 1850
Wire Wire Line
	8150 1150 8150 1350
NoConn ~ 9600 3250
NoConn ~ 9600 3350
NoConn ~ 9600 4050
NoConn ~ 9600 4150
NoConn ~ 9600 4450
NoConn ~ 8300 2950
NoConn ~ 8300 3050
NoConn ~ 8300 3150
NoConn ~ 8300 3250
NoConn ~ 8300 3350
NoConn ~ 8300 3450
NoConn ~ 8300 3550
NoConn ~ 8300 3650
NoConn ~ 8300 3750
NoConn ~ 8300 4050
Wire Wire Line
	8900 1650 8900 1350
Wire Wire Line
	9000 1650 9000 1350
Wire Wire Line
	9000 1350 8900 1350
Connection ~ 8900 1350
Wire Wire Line
	9100 1650 9100 1350
Wire Wire Line
	9100 1350 9000 1350
Connection ~ 9000 1350
Wire Wire Line
	9200 1650 9200 1350
Wire Wire Line
	9200 1350 9100 1350
Connection ~ 9100 1350
$Comp
L power:GND #PWR012
U 1 1 5DA13AAB
P 9100 5200
F 0 "#PWR012" H 9100 4950 50  0001 C CNN
F 1 "GND" H 9105 5027 50  0000 C CNN
F 2 "" H 9100 5200 50  0001 C CNN
F 3 "" H 9100 5200 50  0001 C CNN
	1    9100 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 5200 9100 5050
Wire Wire Line
	9100 5050 9100 4650
Wire Wire Line
	9000 4650 9000 5050
Wire Wire Line
	8900 4650 8900 5050
Wire Wire Line
	8900 5050 9000 5050
Connection ~ 9000 5050
Wire Wire Line
	8800 4650 8800 5050
Wire Wire Line
	8800 5050 8900 5050
Connection ~ 8900 5050
Connection ~ 9100 5050
Wire Wire Line
	9000 5050 9100 5050
NoConn ~ 8300 2550
Wire Wire Line
	9600 2950 9800 2950
Wire Wire Line
	9600 3450 10150 3450
Wire Wire Line
	9600 3650 10150 3650
Text Label 9800 3550 0    50   ~ 0
mosi
Text Label 9800 3450 0    50   ~ 0
sck
Text Label 9800 3650 0    50   ~ 0
miso
Text Label 9800 3750 0    50   ~ 0
cs1
Text Label 9800 3850 0    50   ~ 0
cs2
Text Label 9800 3950 0    50   ~ 0
cs3
Wire Wire Line
	7900 4150 8300 4150
Wire Wire Line
	8300 4250 7900 4250
Wire Wire Line
	7900 4350 8300 4350
Wire Wire Line
	8300 4450 7900 4450
$Comp
L Regulator_Linear:AMS1117-3.3 U1
U 1 1 5DA1850C
P 1800 1750
F 0 "U1" H 1800 1992 50  0000 C CNN
F 1 "AMS1117-3.3" H 1800 1901 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 1800 1950 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 1900 1500 50  0001 C CNN
	1    1800 1750
	1    0    0    -1  
$EndComp
Connection ~ 1800 2050
Wire Wire Line
	2250 1750 2100 1750
Wire Wire Line
	1500 1750 1400 1750
Wire Wire Line
	1200 1650 1200 1750
Wire Wire Line
	1200 1750 1400 1750
Connection ~ 1400 1750
Wire Wire Line
	2400 1700 2400 1750
Wire Wire Line
	2400 1750 2250 1750
Connection ~ 2250 1750
$Comp
L power:GND #PWR02
U 1 1 5DA22CED
P 1800 2250
F 0 "#PWR02" H 1800 2000 50  0001 C CNN
F 1 "GND" H 1805 2077 50  0000 C CNN
F 2 "" H 1800 2250 50  0001 C CNN
F 3 "" H 1800 2250 50  0001 C CNN
	1    1800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 2050 1800 2250
Wire Wire Line
	4900 1000 5050 1000
Wire Wire Line
	5050 1000 5050 1150
Wire Wire Line
	4600 1000 4600 1450
Connection ~ 5050 1000
Wire Wire Line
	4900 1900 5050 1900
Wire Wire Line
	4600 1900 4600 2350
Connection ~ 4600 1900
Wire Wire Line
	5050 2050 5050 1900
Connection ~ 5050 1900
Wire Wire Line
	7950 2050 8300 2050
$Sheet
S 1600 3750 1150 750 
U 5DA16D7F
F0 "Sheet5DA16D7E" 50
F1 "max38165_1.sch" 50
F2 "sck" I R 2750 3900 50 
F3 "mosi" I R 2750 4000 50 
F4 "miso" I R 2750 4100 50 
F5 "cs1" I R 2750 4200 50 
F6 "force2" I L 1600 4200 50 
F7 "rtdin2+" I L 1600 4300 50 
F8 "rtdin2-" I L 1600 4400 50 
F9 "cs2" I R 2750 4300 50 
F10 "force1" I L 1600 3800 50 
F11 "rtdin1+" I L 1600 3900 50 
F12 "rtdin1-" I L 1600 4000 50 
$EndSheet
Wire Wire Line
	1100 4200 1600 4200
Wire Wire Line
	1100 4300 1600 4300
Wire Wire Line
	1100 4400 1600 4400
Wire Wire Line
	2750 3900 3150 3900
Wire Wire Line
	3150 4000 2750 4000
Wire Wire Line
	2750 4100 3150 4100
Wire Wire Line
	3150 4200 2750 4200
Text Label 2900 3900 0    50   ~ 0
sck
Text Label 2900 4000 0    50   ~ 0
mosi
Text Label 2900 4100 0    50   ~ 0
miso
Text Label 2900 4200 0    50   ~ 0
cs1
Wire Wire Line
	9600 3750 10150 3750
Wire Wire Line
	9600 3550 10150 3550
Wire Wire Line
	1250 1150 1000 1150
Wire Wire Line
	1000 1150 1000 1750
Wire Wire Line
	1000 1750 1200 1750
Connection ~ 1200 1750
Wire Wire Line
	1350 1150 1350 1350
Wire Wire Line
	1350 1350 2250 1350
Wire Wire Line
	2250 1350 2250 1750
Wire Wire Line
	2800 1150 2800 2050
Wire Wire Line
	2750 4300 3150 4300
Text Label 2900 4300 0    50   ~ 0
cs2
Wire Wire Line
	1100 4000 1600 4000
Wire Wire Line
	9600 3850 10150 3850
Wire Wire Line
	9600 3950 10150 3950
$Comp
L power:VDD #PWR04
U 1 1 5DA4FAD0
P 2400 1700
F 0 "#PWR04" H 2400 1550 50  0001 C CNN
F 1 "VDD" H 2417 1873 50  0000 C CNN
F 2 "" H 2400 1700 50  0001 C CNN
F 3 "" H 2400 1700 50  0001 C CNN
	1    2400 1700
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5DA4FD8A
P 1200 1650
F 0 "#PWR01" H 1200 1500 50  0001 C CNN
F 1 "VCC" H 1217 1823 50  0000 C CNN
F 2 "" H 1200 1650 50  0001 C CNN
F 3 "" H 1200 1650 50  0001 C CNN
	1    1200 1650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 1350 8800 1350
Connection ~ 8150 1350
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5DA64B69
P 800 1750
F 0 "#FLG01" H 800 1825 50  0001 C CNN
F 1 "PWR_FLAG" H 800 1924 50  0000 C CNN
F 2 "" H 800 1750 50  0001 C CNN
F 3 "~" H 800 1750 50  0001 C CNN
	1    800  1750
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  1750 1000 1750
Connection ~ 1000 1750
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5DA67CE5
P 800 2050
F 0 "#FLG02" H 800 2125 50  0001 C CNN
F 1 "PWR_FLAG" H 800 2224 50  0000 C CNN
F 2 "" H 800 2050 50  0001 C CNN
F 3 "~" H 800 2050 50  0001 C CNN
	1    800  2050
	1    0    0    -1  
$EndComp
$Comp
L test-rescue:MAX7219CWG+-test U2
U 1 1 5DA752A7
P 2400 5650
F 0 "U2" H 3600 6037 60  0000 C CNN
F 1 "MAX7219CWG+" H 3600 5931 60  0000 C CNN
F 2 "test:MAX7219CWG+" H 3600 5890 60  0001 C CNN
F 3 "" H 2400 5650 60  0000 C CNN
	1    2400 5650
	1    0    0    -1  
$EndComp
NoConn ~ 2400 6350
NoConn ~ 2400 6050
NoConn ~ 4800 5650
Wire Wire Line
	4800 5750 5850 5750
Wire Wire Line
	4800 5850 5850 5850
Wire Wire Line
	5850 5950 4800 5950
Wire Wire Line
	4800 6050 5850 6050
Wire Wire Line
	4800 6350 5400 6350
Wire Wire Line
	5400 6350 5400 6150
Wire Wire Line
	5400 6150 5850 6150
Wire Wire Line
	4800 6450 5450 6450
Wire Wire Line
	5450 6450 5450 6250
Wire Wire Line
	5450 6250 5850 6250
Wire Wire Line
	4800 6550 5500 6550
Wire Wire Line
	5500 6550 5500 6350
Wire Wire Line
	5500 6350 5850 6350
Wire Wire Line
	4800 6650 5550 6650
Wire Wire Line
	5550 6650 5550 6450
Wire Wire Line
	5550 6450 5850 6450
$Comp
L Device:R R2
U 1 1 5DAEED74
P 5100 6250
F 0 "R2" V 4893 6250 50  0000 C CNN
F 1 "9.53k" V 4984 6250 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5030 6250 50  0001 C CNN
F 3 "~" H 5100 6250 50  0001 C CNN
	1    5100 6250
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 6250 4950 6250
Wire Wire Line
	4800 6150 4900 6150
Wire Wire Line
	5250 6150 5250 6250
Wire Wire Line
	4900 6150 4900 5250
Connection ~ 4900 6150
Wire Wire Line
	4900 6150 5250 6150
$Comp
L power:VCC #PWR08
U 1 1 5DAFC4B3
P 4900 5250
F 0 "#PWR08" H 4900 5100 50  0001 C CNN
F 1 "VCC" H 4917 5423 50  0000 C CNN
F 2 "" H 4900 5250 50  0001 C CNN
F 3 "" H 4900 5250 50  0001 C CNN
	1    4900 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5DAFC982
P 2100 7100
F 0 "#PWR03" H 2100 6850 50  0001 C CNN
F 1 "GND" H 2105 6927 50  0000 C CNN
F 2 "" H 2100 7100 50  0001 C CNN
F 3 "" H 2100 7100 50  0001 C CNN
	1    2100 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 6450 2100 6450
Wire Wire Line
	2100 6450 2100 7100
Wire Wire Line
	2400 5950 2100 5950
Wire Wire Line
	2100 5950 2100 6450
Connection ~ 2100 6450
Wire Wire Line
	4800 6750 5250 6750
Text Label 1850 6750 0    50   ~ 0
cs3
Text Label 4850 6750 0    50   ~ 0
sck
Wire Wire Line
	1350 5750 2400 5750
Wire Wire Line
	2400 5850 1350 5850
Wire Wire Line
	1350 5950 2000 5950
Wire Wire Line
	2000 5950 2000 6150
Wire Wire Line
	2000 6150 2400 6150
Wire Wire Line
	2400 6250 1900 6250
Wire Wire Line
	1900 6250 1900 6050
Wire Wire Line
	1900 6050 1350 6050
Wire Wire Line
	1350 6150 1800 6150
Wire Wire Line
	1800 6150 1800 6550
Wire Wire Line
	1800 6550 2400 6550
Wire Wire Line
	2400 6650 1700 6650
Wire Wire Line
	1700 6650 1700 6250
Wire Wire Line
	1700 6250 1350 6250
Text Label 2150 5650 0    50   ~ 0
mosi
Wire Wire Line
	2100 5650 2400 5650
Wire Wire Line
	1750 6750 2400 6750
$Comp
L Connector_Generic:Conn_01x03 J2
U 1 1 5DA78F45
P 900 4300
F 0 "J2" H 1050 4500 50  0000 C CNN
F 1 "temperature_2" H 700 4500 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 900 4300 50  0001 C CNN
F 3 "~" H 900 4300 50  0001 C CNN
	1    900  4300
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5DA79019
P 900 3900
F 0 "J1" H 1000 3700 50  0000 C CNN
F 1 "temperature_1" H 650 3700 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 900 3900 50  0001 C CNN
F 3 "~" H 900 3900 50  0001 C CNN
	1    900  3900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5DA8007D
P 1150 6050
F 0 "J3" H 1070 5525 50  0000 C CNN
F 1 "DIG_0_6" H 1070 5616 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 1150 6050 50  0001 C CNN
F 3 "~" H 1150 6050 50  0001 C CNN
	1    1150 6050
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J5
U 1 1 5DA80516
P 6050 6050
F 0 "J5" H 6130 6042 50  0000 L CNN
F 1 "SEG" H 6130 5951 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 6050 6050 50  0001 C CNN
F 3 "~" H 6050 6050 50  0001 C CNN
	1    6050 6050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J7
U 1 1 5DA8099D
P 7700 4350
F 0 "J7" H 7620 3925 50  0000 C CNN
F 1 "output" H 7620 4016 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 7700 4350 50  0001 C CNN
F 3 "~" H 7700 4350 50  0001 C CNN
	1    7700 4350
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J9
U 1 1 5DA80B04
P 10700 3050
F 0 "J9" H 10780 3092 50  0000 L CNN
F 1 "adc" H 10780 3001 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10700 3050 50  0001 C CNN
F 3 "~" H 10700 3050 50  0001 C CNN
	1    10700 3050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J4
U 1 1 5DA80F71
P 1350 950
F 0 "J4" V 1316 662 50  0000 R CNN
F 1 "power" V 1225 662 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 1350 950 50  0001 C CNN
F 3 "~" H 1350 950 50  0001 C CNN
	1    1350 950 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 1150 2800 1150
Wire Wire Line
	1450 1150 1550 1150
Connection ~ 1550 1150
$Comp
L Connector_Generic:Conn_01x03 J8
U 1 1 5DA80866
P 10150 4350
F 0 "J8" H 10230 4392 50  0000 L CNN
F 1 "swd" H 10230 4301 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10150 4350 50  0001 C CNN
F 3 "~" H 10150 4350 50  0001 C CNN
	1    10150 4350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J6
U 1 1 5DA80902
P 7100 3950
F 0 "J6" H 7020 3625 50  0000 C CNN
F 1 "usart" H 7020 3716 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 7100 3950 50  0001 C CNN
F 3 "~" H 7100 3950 50  0001 C CNN
	1    7100 3950
	-1   0    0    1   
$EndComp
Wire Wire Line
	7300 3850 8300 3850
Wire Wire Line
	8300 3950 7300 3950
$Comp
L power:GND #PWR09
U 1 1 5DA8E5A5
P 7300 4200
F 0 "#PWR09" H 7300 3950 50  0001 C CNN
F 1 "GND" H 7305 4027 50  0000 C CNN
F 2 "" H 7300 4200 50  0001 C CNN
F 3 "" H 7300 4200 50  0001 C CNN
	1    7300 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 4200 7300 4050
Wire Wire Line
	9600 4250 9950 4250
Wire Wire Line
	9950 4450 9950 4700
$Comp
L power:GND #PWR014
U 1 1 5DAB19AA
P 9950 4700
F 0 "#PWR014" H 9950 4450 50  0001 C CNN
F 1 "GND" H 9955 4527 50  0000 C CNN
F 2 "" H 9950 4700 50  0001 C CNN
F 3 "" H 9950 4700 50  0001 C CNN
	1    9950 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 1450 5350 1450
Wire Wire Line
	4600 1450 4600 1900
Connection ~ 4600 1450
Wire Wire Line
	4600 1450 4200 1450
Wire Wire Line
	4200 1450 4200 1500
$Comp
L MCU_ST_STM32F1:STM32F103C8Tx U3
U 1 1 5DA0258A
P 9000 3150
F 0 "U3" H 8950 3500 50  0000 C CNN
F 1 "STM32F103C8Tx" H 9000 3700 50  0000 C CNN
F 2 "test:STM32F103C8T6" H 8400 1750 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00161566.pdf" H 9000 3150 50  0001 C CNN
	1    9000 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8800 1650 8800 1350
Connection ~ 8800 1350
Wire Wire Line
	8800 1350 8900 1350
Wire Wire Line
	1100 3900 1600 3900
Wire Wire Line
	1100 3800 1600 3800
Text Notes 1100 4400 0    50   ~ 0
rtdin-
Text Notes 1100 4300 0    50   ~ 0
rtdin+
Text Notes 1100 4200 0    50   ~ 0
force
Text Notes 1100 4000 0    50   ~ 0
rtdin-
Text Notes 1100 3900 0    50   ~ 0
rtdin+
Text Notes 1100 3800 0    50   ~ 0
force
$Comp
L power:VDD #PWR011
U 1 1 5DABC1D2
P 8150 1150
F 0 "#PWR011" H 8150 1000 50  0001 C CNN
F 1 "VDD" H 8167 1323 50  0000 C CNN
F 2 "" H 8150 1150 50  0001 C CNN
F 3 "" H 8150 1150 50  0001 C CNN
	1    8150 1150
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C7
U 1 1 5DAC31E8
P 8800 1150
F 0 "C7" H 8750 1350 50  0000 L CNN
F 1 "100nf" V 8700 1000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 8800 1150 50  0001 C CNN
F 3 "~" H 8800 1150 50  0001 C CNN
	1    8800 1150
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C8
U 1 1 5DAC32B8
P 9000 1150
F 0 "C8" H 8950 1350 50  0000 L CNN
F 1 "100nf" V 8900 1000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9000 1150 50  0001 C CNN
F 3 "~" H 9000 1150 50  0001 C CNN
	1    9000 1150
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C9
U 1 1 5DAC3345
P 9200 1150
F 0 "C9" H 9150 1350 50  0000 L CNN
F 1 "100nf" V 9100 1000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9200 1150 50  0001 C CNN
F 3 "~" H 9200 1150 50  0001 C CNN
	1    9200 1150
	1    0    0    -1  
$EndComp
Wire Wire Line
	8800 1050 9000 1050
Wire Wire Line
	9000 1050 9200 1050
Connection ~ 9000 1050
Wire Wire Line
	8800 1250 8800 1350
Wire Wire Line
	9000 1250 9000 1350
Wire Wire Line
	9200 1250 9200 1350
Connection ~ 9200 1350
$Comp
L power:GND #PWR013
U 1 1 5DB357BD
P 9750 1400
F 0 "#PWR013" H 9750 1150 50  0001 C CNN
F 1 "GND" H 9755 1227 50  0000 C CNN
F 2 "" H 9750 1400 50  0001 C CNN
F 3 "" H 9750 1400 50  0001 C CNN
	1    9750 1400
	1    0    0    -1  
$EndComp
Connection ~ 9200 1050
$Comp
L Device:C_Small C10
U 1 1 5DB3D33D
P 9400 1150
F 0 "C10" H 9350 1350 50  0000 L CNN
F 1 "100nf" V 9300 1000 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9400 1150 50  0001 C CNN
F 3 "~" H 9400 1150 50  0001 C CNN
	1    9400 1150
	1    0    0    -1  
$EndComp
Connection ~ 9400 1050
Wire Wire Line
	9400 1250 9400 1350
Wire Wire Line
	9200 1050 9400 1050
Wire Wire Line
	9200 1350 9400 1350
Wire Wire Line
	9750 1400 9750 1050
Wire Wire Line
	9400 1050 9750 1050
$Comp
L power:VDD #PWR05
U 1 1 5DB539E8
P 3250 1000
F 0 "#PWR05" H 3250 850 50  0001 C CNN
F 1 "VDD" H 3267 1173 50  0000 C CNN
F 2 "" H 3250 1000 50  0001 C CNN
F 3 "" H 3250 1000 50  0001 C CNN
	1    3250 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5DB53A70
P 3250 1350
F 0 "R1" H 3320 1396 50  0000 L CNN
F 1 "1k" H 3320 1305 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3180 1350 50  0001 C CNN
F 3 "~" H 3250 1350 50  0001 C CNN
	1    3250 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D1
U 1 1 5DB53C5A
P 3250 1800
F 0 "D1" V 3195 1878 50  0000 L CNN
F 1 "LED" V 3286 1878 50  0000 L CNN
F 2 "LED_SMD:LED_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3250 1800 50  0001 C CNN
F 3 "~" H 3250 1800 50  0001 C CNN
	1    3250 1800
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5DB53EC3
P 3250 2250
F 0 "#PWR06" H 3250 2000 50  0001 C CNN
F 1 "GND" H 3255 2077 50  0000 C CNN
F 2 "" H 3250 2250 50  0001 C CNN
F 3 "" H 3250 2250 50  0001 C CNN
	1    3250 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 1000 3250 1200
Wire Wire Line
	3250 1500 3250 1650
Wire Wire Line
	3250 1950 3250 2250
Wire Wire Line
	1800 2050 2250 2050
$Comp
L Device:C C2
U 1 1 5DB728B7
P 2250 1900
F 0 "C2" H 2365 1946 50  0000 L CNN
F 1 "10uf" H 2365 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 2288 1750 50  0001 C CNN
F 3 "~" H 2250 1900 50  0001 C CNN
	1    2250 1900
	1    0    0    -1  
$EndComp
Connection ~ 2250 2050
Wire Wire Line
	2250 2050 2800 2050
Wire Wire Line
	800  2050 1400 2050
$Comp
L Device:C C1
U 1 1 5DB729AB
P 1400 1900
F 0 "C1" H 1515 1946 50  0000 L CNN
F 1 "10uf" H 1515 1855 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 1438 1750 50  0001 C CNN
F 3 "~" H 1400 1900 50  0001 C CNN
	1    1400 1900
	1    0    0    -1  
$EndComp
Connection ~ 1400 2050
Wire Wire Line
	1400 2050 1800 2050
Wire Wire Line
	7700 2750 8300 2750
Wire Wire Line
	8300 2650 7700 2650
Wire Wire Line
	7700 2350 8300 2350
Wire Wire Line
	7700 2250 8300 2250
Text Label 7750 2250 0    50   ~ 0
XTAL_IN
Text Label 7750 2350 0    50   ~ 0
XTAL_OUT
Text Label 7750 2650 0    50   ~ 0
OSC32_IN
Text Label 7750 2750 0    50   ~ 0
OSC32_OUT
Wire Wire Line
	5050 1000 5350 1000
Wire Wire Line
	5050 1900 5900 1900
Wire Wire Line
	5050 2350 5900 2350
Text Label 5450 1000 0    50   ~ 0
XTAL_IN
Text Label 5450 1450 0    50   ~ 0
XTAL_OUT
Text Label 5450 1900 0    50   ~ 0
OSC32_IN
Text Label 5450 2350 0    50   ~ 0
OSC32_OUT
$Comp
L power:GND #PWR010
U 1 1 5DADCEA1
P 7400 2150
F 0 "#PWR010" H 7400 1900 50  0001 C CNN
F 1 "GND" H 7405 1977 50  0000 C CNN
F 2 "" H 7400 2150 50  0001 C CNN
F 3 "" H 7400 2150 50  0001 C CNN
	1    7400 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 2150 7400 2050
Wire Wire Line
	7400 2050 7650 2050
$Comp
L Device:R R7
U 1 1 5DAC429C
P 5350 1250
F 0 "R7" H 5420 1296 50  0000 L CNN
F 1 "1m" H 5420 1205 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5280 1250 50  0001 C CNN
F 3 "~" H 5350 1250 50  0001 C CNN
	1    5350 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 1450 5350 1400
Connection ~ 5350 1450
Wire Wire Line
	5350 1450 5900 1450
Wire Wire Line
	5350 1100 5350 1000
Connection ~ 5350 1000
Wire Wire Line
	5350 1000 5900 1000
Wire Wire Line
	9600 4350 9950 4350
$Comp
L Device:C_Small C17
U 1 1 5DB381FB
P 9800 2700
F 0 "C17" V 9750 2750 50  0000 L CNN
F 1 "10nf" V 9900 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 9800 2700 50  0001 C CNN
F 3 "~" H 9800 2700 50  0001 C CNN
	1    9800 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C18
U 1 1 5DB38313
P 10050 2700
F 0 "C18" V 10000 2750 50  0000 L CNN
F 1 "10nf" V 10150 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10050 2700 50  0001 C CNN
F 3 "~" H 10050 2700 50  0001 C CNN
	1    10050 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C19
U 1 1 5DB383C3
P 10300 2700
F 0 "C19" V 10250 2750 50  0000 L CNN
F 1 "10nf" V 10400 2600 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 10300 2700 50  0001 C CNN
F 3 "~" H 10300 2700 50  0001 C CNN
	1    10300 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 2800 9800 2950
Connection ~ 9800 2950
Wire Wire Line
	10050 2800 10050 3050
Connection ~ 10050 3050
Wire Wire Line
	10050 3050 9600 3050
Wire Wire Line
	9800 2950 10500 2950
Wire Wire Line
	10050 3050 10500 3050
Wire Wire Line
	9600 3150 10300 3150
Wire Wire Line
	10300 2800 10300 3150
Connection ~ 10300 3150
Wire Wire Line
	10300 3150 10500 3150
$Comp
L power:GND #PWR029
U 1 1 5DB6ED1A
P 10650 2550
F 0 "#PWR029" H 10650 2300 50  0001 C CNN
F 1 "GND" H 10655 2377 50  0000 C CNN
F 2 "" H 10650 2550 50  0001 C CNN
F 3 "" H 10650 2550 50  0001 C CNN
	1    10650 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 2600 9800 2450
Wire Wire Line
	9800 2450 10050 2450
Wire Wire Line
	10050 2600 10050 2450
Connection ~ 10050 2450
Wire Wire Line
	10050 2450 10300 2450
Wire Wire Line
	10300 2600 10300 2450
Connection ~ 10300 2450
Wire Wire Line
	10300 2450 10650 2450
Wire Wire Line
	10650 2550 10650 2450
$EndSCHEMATC
