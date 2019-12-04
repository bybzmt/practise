EESchema Schematic File Version 4
LIBS:oven2-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
NoConn ~ 6500 3950
NoConn ~ 6500 3650
NoConn ~ 8900 3250
$Comp
L Device:R R8
U 1 1 5DAEED74
P 9800 3750
F 0 "R8" V 9593 3750 50  0000 C CNN
F 1 "9.53k" V 9684 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 9730 3750 50  0001 C CNN
F 3 "~" H 9800 3750 50  0001 C CNN
	1    9800 3750
	0    1    1    0   
$EndComp
$Comp
L power:VCC #PWR014
U 1 1 5DAFC4B3
P 9550 3000
F 0 "#PWR014" H 9550 2850 50  0001 C CNN
F 1 "VCC" H 9567 3173 50  0000 C CNN
F 2 "" H 9550 3000 50  0001 C CNN
F 3 "" H 9550 3000 50  0001 C CNN
	1    9550 3000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5DAFC982
P 5650 3800
F 0 "#PWR013" H 5650 3550 50  0001 C CNN
F 1 "GND" H 5655 3627 50  0000 C CNN
F 2 "" H 5650 3800 50  0001 C CNN
F 3 "" H 5650 3800 50  0001 C CNN
	1    5650 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	5950 3550 5950 3800
$Comp
L oven2:CC56-12EWA U3
U 1 1 5DE65E8C
P 2550 1700
F 0 "U3" H 2550 2367 50  0000 C CNN
F 1 "CC56-12EWA" H 2550 2276 50  0000 C CNN
F 2 "oven2:2831as" H 2550 1100 50  0001 C CNN
F 3 "http://www.kingbrightusa.com/images/catalog/SPEC/CA56-12EWA.pdf" H 2320 1730 50  0001 C CNN
	1    2550 1700
	1    0    0    -1  
$EndComp
$Comp
L oven2:CC56-12EWA U4
U 1 1 5DE65F6A
P 2550 3000
F 0 "U4" H 2550 3667 50  0000 C CNN
F 1 "CC56-12EWA" H 2550 3576 50  0000 C CNN
F 2 "oven2:2831as" H 2550 2400 50  0001 C CNN
F 3 "http://www.kingbrightusa.com/images/catalog/SPEC/CA56-12EWA.pdf" H 2320 3030 50  0001 C CNN
	1    2550 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 1400 950  1400
Text Label 1000 1400 0    50   ~ 0
dA
Wire Wire Line
	950  1500 1450 1500
Wire Wire Line
	1450 1600 950  1600
Wire Wire Line
	950  1700 1450 1700
Wire Wire Line
	950  1800 1450 1800
Wire Wire Line
	950  1900 1450 1900
Wire Wire Line
	950  2000 1450 2000
Wire Wire Line
	950  2100 1450 2100
Wire Wire Line
	3650 1800 4150 1800
Wire Wire Line
	3650 1900 4150 1900
Wire Wire Line
	3650 2000 4150 2000
Wire Wire Line
	1000 2700 1450 2700
Wire Wire Line
	1000 2800 1450 2800
Wire Wire Line
	1000 2900 1450 2900
Wire Wire Line
	1000 3000 1450 3000
Wire Wire Line
	1000 3100 1450 3100
Wire Wire Line
	1000 3200 1450 3200
Wire Wire Line
	1000 3300 1450 3300
Wire Wire Line
	1000 3400 1450 3400
Wire Wire Line
	4100 3100 3650 3100
Wire Wire Line
	4100 3200 3650 3200
Wire Wire Line
	4100 3300 3650 3300
Text Label 1000 1500 0    50   ~ 0
dB
Text Label 1000 1600 0    50   ~ 0
dC
Text Label 1000 1700 0    50   ~ 0
dD
Text Label 1000 1800 0    50   ~ 0
dE
Text Label 1000 1900 0    50   ~ 0
dF
Text Label 1000 2000 0    50   ~ 0
dG
Text Label 1000 2100 0    50   ~ 0
DP
Text Label 3900 1800 0    50   ~ 0
c1
Text Label 3900 1900 0    50   ~ 0
c2
Text Label 3900 2000 0    50   ~ 0
c3
Text Label 1100 2700 0    50   ~ 0
dA
Text Label 1100 2800 0    50   ~ 0
dB
Text Label 1100 2900 0    50   ~ 0
dC
Text Label 1100 3000 0    50   ~ 0
dD
Text Label 1100 3100 0    50   ~ 0
dE
Text Label 1100 3200 0    50   ~ 0
dF
Text Label 1100 3300 0    50   ~ 0
dG
Text Label 1100 3400 0    50   ~ 0
DP
Text Label 3850 3100 0    50   ~ 0
c4
Text Label 3850 3200 0    50   ~ 0
c5
Text Label 3850 3300 0    50   ~ 0
c6
Text Label 6250 3350 0    50   ~ 0
c1
Text Label 6250 3750 0    50   ~ 0
c3
Text Label 6250 4250 0    50   ~ 0
c2
Text Label 6250 3450 0    50   ~ 0
c5
Text Label 6250 3850 0    50   ~ 0
c4
Text Label 6250 4150 0    50   ~ 0
c6
Wire Wire Line
	6200 3350 6500 3350
Wire Wire Line
	6200 3450 6500 3450
Wire Wire Line
	5950 3550 6500 3550
Wire Wire Line
	5950 4050 6500 4050
Wire Wire Line
	6200 3750 6500 3750
Wire Wire Line
	6200 3850 6500 3850
Wire Wire Line
	6200 4150 6500 4150
Wire Wire Line
	6200 4250 6500 4250
Wire Wire Line
	9300 3350 8900 3350
Wire Wire Line
	9300 3450 8900 3450
Wire Wire Line
	9300 3550 8900 3550
Wire Wire Line
	9300 3650 8900 3650
Wire Wire Line
	9300 3950 8900 3950
Wire Wire Line
	9300 4050 8900 4050
Wire Wire Line
	9300 4150 8900 4150
Wire Wire Line
	9300 4250 8900 4250
Wire Wire Line
	9550 3750 9550 3200
Wire Wire Line
	8900 3750 9550 3750
Wire Wire Line
	9950 3750 9950 3850
Wire Wire Line
	8900 3850 9950 3850
Connection ~ 9550 3750
Wire Wire Line
	9550 3750 9650 3750
Text Label 9050 3350 0    50   ~ 0
dD
Text Label 9050 3450 0    50   ~ 0
DP
Text Label 9050 3550 0    50   ~ 0
dE
Text Label 9050 3650 0    50   ~ 0
dC
Text Label 9050 3950 0    50   ~ 0
dG
Text Label 9050 4050 0    50   ~ 0
dB
Text Label 9050 4150 0    50   ~ 0
dF
Text Label 9050 4250 0    50   ~ 0
dA
Text HLabel 9550 4350 2    50   Input ~ 0
sck
Wire Wire Line
	8900 4350 9550 4350
Text HLabel 5700 3250 0    50   Input ~ 0
mosi
Text HLabel 5700 4350 0    50   Input ~ 0
cs3
Wire Wire Line
	5700 3250 6500 3250
Wire Wire Line
	5700 4350 6500 4350
Wire Wire Line
	5650 3800 5950 3800
Connection ~ 5950 3800
Wire Wire Line
	5950 3800 5950 4050
$Comp
L Device:C C8
U 1 1 5DFB1E36
P 10300 3350
F 0 "C8" H 10415 3396 50  0000 L CNN
F 1 "10uf" H 10415 3305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10338 3200 50  0001 C CNN
F 3 "~" H 10300 3350 50  0001 C CNN
	1    10300 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5DFB1E93
P 10700 3350
F 0 "C9" H 10815 3396 50  0000 L CNN
F 1 "100nf" H 10815 3305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10738 3200 50  0001 C CNN
F 3 "~" H 10700 3350 50  0001 C CNN
	1    10700 3350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5DFB1EE5
P 10500 3900
F 0 "#PWR015" H 10500 3650 50  0001 C CNN
F 1 "GND" H 10505 3727 50  0000 C CNN
F 2 "" H 10500 3900 50  0001 C CNN
F 3 "" H 10500 3900 50  0001 C CNN
	1    10500 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9550 3200 10300 3200
Connection ~ 9550 3200
Wire Wire Line
	9550 3200 9550 3000
Wire Wire Line
	10300 3200 10700 3200
Connection ~ 10300 3200
Wire Wire Line
	10300 3500 10500 3500
Wire Wire Line
	10500 3900 10500 3500
Connection ~ 10500 3500
Wire Wire Line
	10500 3500 10700 3500
$Comp
L oven2:MAX7219CWG+ U5
U 1 1 5DDED2DF
P 6500 3250
F 0 "U5" H 7700 3637 60  0000 C CNN
F 1 "MAX7219CWG+" H 7700 3531 60  0000 C CNN
F 2 "oven2:MAX7219CWG+" H 7700 3490 60  0001 C CNN
F 3 "" H 6500 3250 60  0000 C CNN
	1    6500 3250
	1    0    0    -1  
$EndComp
$EndSCHEMATC
