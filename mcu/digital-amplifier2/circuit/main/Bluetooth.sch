EESchema Schematic File Version 4
LIBS:digital-amplifier2-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 7 7
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
NoConn ~ 4450 2375
NoConn ~ 4300 2375
NoConn ~ 4150 2375
NoConn ~ 2850 3125
NoConn ~ 4000 2375
NoConn ~ 3850 2375
NoConn ~ 3700 2375
NoConn ~ 5600 3275
NoConn ~ 5600 3425
NoConn ~ 5600 3575
NoConn ~ 5600 3725
NoConn ~ 5600 3875
NoConn ~ 5600 4175
NoConn ~ 5600 4325
NoConn ~ 5600 4475
NoConn ~ 5600 4625
NoConn ~ 5600 4775
NoConn ~ 5600 4925
NoConn ~ 5600 5225
NoConn ~ 5600 5375
NoConn ~ 4150 6275
NoConn ~ 3700 6275
Text HLabel 2100 3725 0    50   Input ~ 0
CLK
Text HLabel 2100 3575 0    50   Input ~ 0
FS
Text HLabel 2100 3425 0    50   Input ~ 0
SD
Wire Wire Line
	2850 3425 2100 3425
Wire Wire Line
	2850 3575 2100 3575
Wire Wire Line
	2850 3725 2100 3725
NoConn ~ 2850 4175
NoConn ~ 2850 4325
NoConn ~ 2850 4475
NoConn ~ 2850 4625
NoConn ~ 2850 4775
Text HLabel 2100 4925 0    50   Input ~ 0
EN
Wire Wire Line
	2100 4925 2850 4925
$Comp
L Device:C_Small C125
U 1 1 5FD6F3D0
P 1800 5450
F 0 "C125" H 1892 5496 50  0000 L CNN
F 1 "22uf" H 1892 5405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1800 5450 50  0001 C CNN
F 3 "~" H 1800 5450 50  0001 C CNN
	1    1800 5450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5FD6FF1D
P 1800 5925
F 0 "#PWR0101" H 1800 5675 50  0001 C CNN
F 1 "GND" H 1805 5752 50  0000 C CNN
F 2 "" H 1800 5925 50  0001 C CNN
F 3 "" H 1800 5925 50  0001 C CNN
	1    1800 5925
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 5075 1800 5075
Wire Wire Line
	1800 5075 1800 5350
Wire Wire Line
	1800 5550 1800 5650
Wire Wire Line
	2850 5225 2125 5225
Wire Wire Line
	2125 5225 2125 5650
Wire Wire Line
	2125 5650 1800 5650
Connection ~ 1800 5650
Wire Wire Line
	1800 5650 1800 5925
Wire Wire Line
	4300 6275 4300 6900
Wire Wire Line
	4300 6900 4450 6900
Wire Wire Line
	4450 6275 4450 6900
Connection ~ 4450 6900
Wire Wire Line
	4450 6900 5050 6900
$Comp
L power:GND #PWR0102
U 1 1 5FD70E74
P 4000 6725
F 0 "#PWR0102" H 4000 6475 50  0001 C CNN
F 1 "GND" H 4005 6552 50  0000 C CNN
F 2 "" H 4000 6725 50  0001 C CNN
F 3 "" H 4000 6725 50  0001 C CNN
	1    4000 6725
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 6725 4000 6275
$Comp
L power:GND #PWR0105
U 1 1 5FD71A86
P 6250 4325
F 0 "#PWR0105" H 6250 4075 50  0001 C CNN
F 1 "GND" H 6255 4152 50  0000 C CNN
F 2 "" H 6250 4325 50  0001 C CNN
F 3 "" H 6250 4325 50  0001 C CNN
	1    6250 4325
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4325 6250 4025
Wire Wire Line
	6250 4025 5600 4025
$Comp
L power:GND #PWR0103
U 1 1 5FD726D5
P 5875 2425
F 0 "#PWR0103" H 5875 2175 50  0001 C CNN
F 1 "GND" H 5880 2252 50  0000 C CNN
F 2 "" H 5875 2425 50  0001 C CNN
F 3 "" H 5875 2425 50  0001 C CNN
	1    5875 2425
	1    0    0    -1  
$EndComp
Wire Wire Line
	5875 2425 5875 2175
Wire Wire Line
	4600 2375 4600 2175
Wire Wire Line
	5600 5075 6175 5075
Wire Wire Line
	5600 5525 6175 5525
Text HLabel 6175 5075 2    50   Input ~ 0
button
Text HLabel 6175 5525 2    50   Input ~ 0
state
Text HLabel 3100 2025 0    50   Input ~ 0
SPDIF
Wire Wire Line
	3550 2375 3550 2025
Wire Wire Line
	3550 2025 3100 2025
NoConn ~ 2850 4025
NoConn ~ 3550 6275
$Comp
L Device:R_Small R70
U 1 1 5FD79927
P 2275 5925
F 0 "R70" H 2334 5971 50  0000 L CNN
F 1 "1k" H 2334 5880 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2275 5925 50  0001 C CNN
F 3 "~" H 2275 5925 50  0001 C CNN
	1    2275 5925
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R71
U 1 1 5FD7999C
P 2600 5925
F 0 "R71" H 2659 5971 50  0000 L CNN
F 1 "1k" H 2659 5880 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2600 5925 50  0001 C CNN
F 3 "~" H 2600 5925 50  0001 C CNN
	1    2600 5925
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D9
U 1 1 5FD79E0B
P 2275 6300
F 0 "D9" V 2220 6378 50  0000 L CNN
F 1 "LED" V 2311 6378 50  0000 L CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 2275 6300 50  0001 C CNN
F 3 "~" H 2275 6300 50  0001 C CNN
	1    2275 6300
	0    1    1    0   
$EndComp
$Comp
L Device:LED D10
U 1 1 5FD79E88
P 2600 6325
F 0 "D10" V 2545 6403 50  0000 L CNN
F 1 "LED" V 2636 6403 50  0000 L CNN
F 2 "LED_SMD:LED_0805_2012Metric" H 2600 6325 50  0001 C CNN
F 3 "~" H 2600 6325 50  0001 C CNN
	1    2600 6325
	0    1    1    0   
$EndComp
Wire Wire Line
	2600 5825 2600 5525
Wire Wire Line
	2600 5525 2850 5525
Wire Wire Line
	2850 5375 2275 5375
Wire Wire Line
	2275 5375 2275 5825
Wire Wire Line
	2275 6025 2275 6150
Wire Wire Line
	2600 6025 2600 6175
Wire Wire Line
	2600 6475 2600 6650
Wire Wire Line
	2600 6650 2450 6650
Wire Wire Line
	2275 6650 2275 6450
Wire Wire Line
	3025 6825 2450 6825
Wire Wire Line
	2450 6825 2450 6650
Connection ~ 2450 6650
Wire Wire Line
	2450 6650 2275 6650
NoConn ~ 4600 6275
NoConn ~ 4750 6275
NoConn ~ 4900 6275
NoConn ~ 3850 6275
NoConn ~ 2850 3275
$Comp
L power:GND #PWR0104
U 1 1 5FDC12A0
P 6250 3300
F 0 "#PWR0104" H 6250 3050 50  0001 C CNN
F 1 "GND" H 6255 3127 50  0000 C CNN
F 2 "" H 6250 3300 50  0001 C CNN
F 3 "" H 6250 3300 50  0001 C CNN
	1    6250 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3300 6250 3125
Wire Wire Line
	5600 3125 6250 3125
$Comp
L misc:SJR-BTM331 U13
U 1 1 5FD6BC07
P 2950 2475
F 0 "U13" H 4225 2753 50  0000 C CNN
F 1 "SJR-BTM331" H 4225 2662 50  0000 C CNN
F 2 "misc:SJR-BTM331" H 3350 975 50  0001 C CNN
F 3 "" H 3350 975 50  0001 C CNN
	1    2950 2475
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0106
U 1 1 5FE85C0D
P 8875 1075
F 0 "#PWR0106" H 8875 925 50  0001 C CNN
F 1 "+3.3V" H 8890 1248 50  0000 C CNN
F 2 "" H 8875 1075 50  0001 C CNN
F 3 "" H 8875 1075 50  0001 C CNN
	1    8875 1075
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R73
U 1 1 5FE85C5E
P 9150 1425
F 0 "R73" V 8954 1425 50  0000 C CNN
F 1 "0Ω" V 9045 1425 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 9150 1425 50  0001 C CNN
F 3 "~" H 9150 1425 50  0001 C CNN
	1    9150 1425
	0    1    1    0   
$EndComp
Wire Wire Line
	8875 1075 8875 1425
Wire Wire Line
	8875 1425 9050 1425
Wire Wire Line
	9250 1425 9425 1425
Text Label 9775 1425 0    50   ~ 0
vdd_x
$Comp
L Device:C_Small C126
U 1 1 5FE872FA
P 9425 1625
F 0 "C126" H 9517 1671 50  0000 L CNN
F 1 "10uf" H 9517 1580 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9425 1625 50  0001 C CNN
F 3 "~" H 9425 1625 50  0001 C CNN
	1    9425 1625
	1    0    0    -1  
$EndComp
Wire Wire Line
	9425 1525 9425 1425
Connection ~ 9425 1425
Wire Wire Line
	9425 1425 10050 1425
Wire Wire Line
	9425 1725 9425 1900
$Comp
L power:GND #PWR0107
U 1 1 5FE88CAB
P 9425 1900
F 0 "#PWR0107" H 9425 1650 50  0001 C CNN
F 1 "GND" H 9430 1727 50  0000 C CNN
F 2 "" H 9425 1900 50  0001 C CNN
F 3 "" H 9425 1900 50  0001 C CNN
	1    9425 1900
	1    0    0    -1  
$EndComp
Text Label 2150 5075 0    50   ~ 0
vdd_x
Text Label 4700 6900 0    50   ~ 0
vdd_x
$Comp
L power:PWR_FLAG #FLG018
U 1 1 5FE8E41D
P 9425 1100
F 0 "#FLG018" H 9425 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 9425 1274 50  0000 C CNN
F 2 "" H 9425 1100 50  0001 C CNN
F 3 "~" H 9425 1100 50  0001 C CNN
	1    9425 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9425 1100 9425 1425
Text Label 2650 6825 0    50   ~ 0
vdd_x
Wire Wire Line
	4600 2175 4900 2175
Wire Wire Line
	4900 2175 5875 2175
Connection ~ 4900 2175
Wire Wire Line
	4900 1800 4900 2175
Wire Wire Line
	4900 2175 4900 2375
NoConn ~ 2850 3875
$Comp
L Connector:Conn_Coaxial J7
U 1 1 6012EC07
P 4950 1400
F 0 "J7" H 5050 1400 50  0000 L CNN
F 1 "Conn_Coaxial" H 5050 1300 50  0000 L CNN
F 2 "Connector_Coaxial:U.FL_Hirose_U.FL-R-SMT-1_Vertical" H 4950 1400 50  0001 C CNN
F 3 " ~" H 4950 1400 50  0001 C CNN
	1    4950 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 1400 4750 2375
Wire Wire Line
	4950 1600 4950 1800
Wire Wire Line
	4950 1800 4900 1800
$EndSCHEMATC