EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 12
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
L Connector:Conn_01x04_Female J5
U 1 1 609FAD53
P 5200 3375
F 0 "J5" H 5075 2925 50  0000 C CNN
F 1 "swd" H 5075 3025 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 5200 3375 50  0001 C CNN
F 3 "~" H 5200 3375 50  0001 C CNN
	1    5200 3375
	-1   0    0    1   
$EndComp
Wire Wire Line
	5525 3475 5400 3475
Wire Wire Line
	5525 3575 5525 3475
$Comp
L Connector:TestPoint TP4
U 1 1 609F5D6E
P 6950 3525
F 0 "TP4" V 7175 3550 50  0000 C CNN
F 1 "TestPoint" V 7075 3550 50  0000 C CNN
F 2 "TestPoint:TestPoint_Pad_1.0x1.0mm" H 7150 3525 50  0001 C CNN
F 3 "~" H 7150 3525 50  0001 C CNN
	1    6950 3525
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint TP3
U 1 1 609F5742
P 6950 3075
F 0 "TP3" V 7175 3125 50  0000 C CNN
F 1 "TestPoint" V 7075 3125 50  0000 C CNN
F 2 "TestPoint:TestPoint_Pad_1.0x1.0mm" H 7150 3075 50  0001 C CNN
F 3 "~" H 7150 3075 50  0001 C CNN
	1    6950 3075
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0121
U 1 1 609F501D
P 5525 3575
F 0 "#PWR0121" H 5525 3325 50  0001 C CNN
F 1 "GND" H 5575 3350 50  0000 C CNN
F 2 "" H 5525 3575 50  0001 C CNN
F 3 "" H 5525 3575 50  0001 C CNN
	1    5525 3575
	1    0    0    -1  
$EndComp
Text HLabel 7325 3075 2    50   Input ~ 0
JTDI
Wire Wire Line
	7325 3075 6950 3075
Wire Wire Line
	5775 3175 5400 3175
Wire Wire Line
	5775 3275 5400 3275
Wire Wire Line
	5775 3375 5400 3375
Wire Wire Line
	7325 3525 6950 3525
Text HLabel 5775 3175 2    50   Input ~ 0
JTDO
Text HLabel 5775 3275 2    50   Input ~ 0
SWDIO
Text HLabel 5775 3375 2    50   Input ~ 0
SWCLK
Text HLabel 7325 3525 2    50   Input ~ 0
JTRST
$EndSCHEMATC
