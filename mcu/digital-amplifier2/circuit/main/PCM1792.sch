EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 10
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
L Audio:PCM1792A U12
U 1 1 6071C44B
P 5750 3900
F 0 "U12" H 5750 4050 50  0000 C CNN
F 1 "PCM1792A" H 5750 3925 50  0000 C CNN
F 2 "Package_SO:SSOP-28_5.3x10.2mm_P0.65mm" H 5750 3900 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/pcm1792a.pdf" H 5750 3900 50  0001 C CNN
	1    5750 3900
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R48
U 1 1 6072FC32
P 925 1000
F 0 "R48" V 675 1025 50  0000 C CNN
F 1 "0Ω" V 775 1025 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 925 1000 50  0001 C CNN
F 3 "~" H 925 1000 50  0001 C CNN
	1    925  1000
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C50
U 1 1 60731439
P 1150 1200
F 0 "C50" H 1250 1250 50  0000 L CNN
F 1 "10uf" H 1250 1200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1150 1200 50  0001 C CNN
F 3 "~" H 1150 1200 50  0001 C CNN
	1    1150 1200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR070
U 1 1 607325D7
P 1150 1500
F 0 "#PWR070" H 1150 1250 50  0001 C CNN
F 1 "GND" H 1200 1300 50  0000 C CNN
F 2 "" H 1150 1500 50  0001 C CNN
F 3 "" H 1150 1500 50  0001 C CNN
	1    1150 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	700  800  700  1000
Wire Wire Line
	825  1000 700  1000
Wire Wire Line
	1025 1000 1150 1000
Wire Wire Line
	1150 1000 1150 1100
Wire Wire Line
	1150 1300 1150 1400
Wire Wire Line
	1150 1500 1150 1400
Wire Wire Line
	1150 1000 1575 1000
Connection ~ 1150 1000
Text Label 1750 1000 0    50   ~ 0
vdd1
$Comp
L power:+5V #PWR071
U 1 1 60741198
P 2475 775
F 0 "#PWR071" H 2475 625 50  0001 C CNN
F 1 "+5V" H 2515 985 50  0000 C CNN
F 2 "" H 2475 775 50  0001 C CNN
F 3 "" H 2475 775 50  0001 C CNN
	1    2475 775 
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R49
U 1 1 6074119E
P 2700 975
F 0 "R49" V 2450 1000 50  0000 C CNN
F 1 "0Ω" V 2550 1000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 2700 975 50  0001 C CNN
F 3 "~" H 2700 975 50  0001 C CNN
	1    2700 975 
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C52
U 1 1 607411A4
P 2925 1175
F 0 "C52" H 3025 1225 50  0000 L CNN
F 1 "10uf" H 3025 1175 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 2925 1175 50  0001 C CNN
F 3 "~" H 2925 1175 50  0001 C CNN
	1    2925 1175
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR072
U 1 1 607411AA
P 2925 1475
F 0 "#PWR072" H 2925 1225 50  0001 C CNN
F 1 "GND" H 2975 1275 50  0000 C CNN
F 2 "" H 2925 1475 50  0001 C CNN
F 3 "" H 2925 1475 50  0001 C CNN
	1    2925 1475
	1    0    0    -1  
$EndComp
Wire Wire Line
	2475 775  2475 975 
Wire Wire Line
	2600 975  2475 975 
Wire Wire Line
	2800 975  2925 975 
Wire Wire Line
	2925 975  2925 1075
Wire Wire Line
	2925 1275 2925 1375
Wire Wire Line
	2925 1475 2925 1375
Wire Wire Line
	2925 975  3350 975 
Connection ~ 2925 975 
Text Label 6525 975  0    50   ~ 0
vdd2
$Comp
L power:+3.3V #PWR069
U 1 1 607421D7
P 700 800
F 0 "#PWR069" H 700 650 50  0001 C CNN
F 1 "+3.3V" H 750 1000 50  0000 C CNN
F 2 "" H 700 800 50  0001 C CNN
F 3 "" H 700 800 50  0001 C CNN
	1    700  800 
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C51
U 1 1 607449DD
P 1575 1200
F 0 "C51" H 1700 1250 50  0000 L CNN
F 1 "100nf" H 1700 1200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 1575 1200 50  0001 C CNN
F 3 "~" H 1575 1200 50  0001 C CNN
	1    1575 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1575 1000 1575 1100
Wire Wire Line
	1575 1300 1575 1400
Connection ~ 1575 1000
Wire Wire Line
	1575 1400 1150 1400
$Comp
L Device:C_Small C53
U 1 1 6074749E
P 3350 1175
F 0 "C53" H 3450 1250 50  0000 L CNN
F 1 "100nf" H 3450 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3350 1175 50  0001 C CNN
F 3 "~" H 3350 1175 50  0001 C CNN
	1    3350 1175
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 1275 3350 1375
Wire Wire Line
	3350 1375 2925 1375
Connection ~ 3350 975 
Wire Wire Line
	3350 975  3350 1075
Wire Wire Line
	1575 1000 1950 1000
Wire Wire Line
	5750 3100 5750 2950
Text Label 5550 2775 1    50   ~ 0
vdd1
Text Label 5750 2775 1    50   ~ 0
vdd2
Wire Wire Line
	6050 4700 6050 4925
Wire Wire Line
	6050 4925 5950 4925
Wire Wire Line
	5550 4925 5550 4700
Wire Wire Line
	5750 4700 5750 4925
Connection ~ 5750 4925
Wire Wire Line
	5750 4925 5550 4925
Wire Wire Line
	5850 4700 5850 4925
Connection ~ 5850 4925
Wire Wire Line
	5850 4925 5750 4925
Wire Wire Line
	5950 4700 5950 4925
Connection ~ 5950 4925
Wire Wire Line
	5950 4925 5850 4925
$Comp
L power:GND #PWR073
U 1 1 6072A908
P 5950 5075
F 0 "#PWR073" H 5950 4825 50  0001 C CNN
F 1 "GND" H 5975 4850 50  0000 C CNN
F 2 "" H 5950 5075 50  0001 C CNN
F 3 "" H 5950 5075 50  0001 C CNN
	1    5950 5075
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 5075 5950 4925
Wire Wire Line
	5850 3100 5850 2950
Wire Wire Line
	5850 2950 5750 2950
Connection ~ 5750 2950
Wire Wire Line
	5750 2950 5750 2575
Wire Wire Line
	5950 3100 5950 2950
Wire Wire Line
	5950 2950 5850 2950
Connection ~ 5850 2950
$Comp
L Device:R_Small R50
U 1 1 6072CF91
P 6625 4775
F 0 "R50" H 6700 4850 50  0000 L CNN
F 1 "10k" H 6700 4750 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 6625 4775 50  0001 C CNN
F 3 "~" H 6625 4775 50  0001 C CNN
	1    6625 4775
	1    0    0    -1  
$EndComp
Wire Wire Line
	6625 4675 6625 4500
Wire Wire Line
	6625 4500 6450 4500
Wire Wire Line
	6625 4875 6625 4925
Wire Wire Line
	6625 4925 6050 4925
Connection ~ 6050 4925
Wire Wire Line
	4925 4925 5550 4925
Connection ~ 5550 4925
NoConn ~ 6450 4200
NoConn ~ 6450 3800
Wire Wire Line
	5050 4100 4925 4100
Wire Wire Line
	5050 3900 4925 3900
Wire Wire Line
	4925 3900 4925 4100
Connection ~ 4925 4100
Wire Wire Line
	5050 3800 4325 3800
Wire Wire Line
	5050 4000 4325 4000
Text HLabel 4325 3800 0    50   Input ~ 0
SCL
Text HLabel 4325 4000 0    50   Input ~ 0
SDA
Wire Wire Line
	5050 3300 4350 3300
Wire Wire Line
	5050 3400 4350 3400
Wire Wire Line
	5050 3500 4350 3500
Wire Wire Line
	5050 3600 4350 3600
Text HLabel 4350 3300 0    50   Input ~ 0
SCK
Text HLabel 4350 3400 0    50   Input ~ 0
BCK
Text HLabel 4350 3500 0    50   Input ~ 0
LRCK
Text HLabel 4350 3600 0    50   Input ~ 0
DATA
Wire Wire Line
	4325 4500 5050 4500
Text HLabel 4325 4500 0    50   Input ~ 0
RST
Wire Wire Line
	3350 975  3850 975 
$Comp
L Device:C_Small C54
U 1 1 6074B522
P 3850 1175
F 0 "C54" H 3950 1225 50  0000 L CNN
F 1 "10uf" H 3950 1175 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3850 1175 50  0001 C CNN
F 3 "~" H 3850 1175 50  0001 C CNN
	1    3850 1175
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C55
U 1 1 6074B528
P 4275 1175
F 0 "C55" H 4375 1250 50  0000 L CNN
F 1 "100nf" H 4375 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4275 1175 50  0001 C CNN
F 3 "~" H 4275 1175 50  0001 C CNN
	1    4275 1175
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C56
U 1 1 6074D431
P 4725 1175
F 0 "C56" H 4825 1225 50  0000 L CNN
F 1 "10uf" H 4825 1175 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4725 1175 50  0001 C CNN
F 3 "~" H 4725 1175 50  0001 C CNN
	1    4725 1175
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C57
U 1 1 6074D437
P 5150 1175
F 0 "C57" H 5250 1250 50  0000 L CNN
F 1 "100nf" H 5250 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5150 1175 50  0001 C CNN
F 3 "~" H 5150 1175 50  0001 C CNN
	1    5150 1175
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 1075 3850 975 
Connection ~ 3850 975 
Wire Wire Line
	3850 975  4275 975 
Wire Wire Line
	4275 1075 4275 975 
Connection ~ 4275 975 
Wire Wire Line
	4275 975  4725 975 
Wire Wire Line
	4725 1075 4725 975 
Connection ~ 4725 975 
Wire Wire Line
	4725 975  5150 975 
Wire Wire Line
	5150 1075 5150 975 
Connection ~ 5150 975 
Wire Wire Line
	5150 975  5575 975 
Wire Wire Line
	5150 1275 5150 1375
Connection ~ 3350 1375
Wire Wire Line
	3850 1275 3850 1375
Connection ~ 3850 1375
Wire Wire Line
	3850 1375 3350 1375
Wire Wire Line
	4275 1275 4275 1375
Connection ~ 4275 1375
Wire Wire Line
	4275 1375 3850 1375
Wire Wire Line
	4725 1275 4725 1375
Wire Wire Line
	4275 1375 4725 1375
Connection ~ 4725 1375
Wire Wire Line
	4725 1375 5150 1375
$Comp
L Device:C_Small C58
U 1 1 60764347
P 5575 1175
F 0 "C58" H 5675 1225 50  0000 L CNN
F 1 "10uf" H 5675 1175 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5575 1175 50  0001 C CNN
F 3 "~" H 5575 1175 50  0001 C CNN
	1    5575 1175
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C59
U 1 1 6076434D
P 6000 1175
F 0 "C59" H 6100 1250 50  0000 L CNN
F 1 "100nf" H 6100 1150 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6000 1175 50  0001 C CNN
F 3 "~" H 6000 1175 50  0001 C CNN
	1    6000 1175
	1    0    0    -1  
$EndComp
Wire Wire Line
	5575 1075 5575 975 
Wire Wire Line
	6000 1075 6000 975 
Wire Wire Line
	6000 1275 6000 1375
Wire Wire Line
	5575 1275 5575 1375
Wire Wire Line
	5575 975  6000 975 
Connection ~ 5575 975 
Wire Wire Line
	6000 975  6900 975 
Connection ~ 6000 975 
Wire Wire Line
	6000 1375 5575 1375
Connection ~ 5150 1375
Connection ~ 5575 1375
Wire Wire Line
	5575 1375 5150 1375
Wire Wire Line
	4925 4100 4925 4925
Wire Wire Line
	5050 4200 4725 4200
Wire Wire Line
	4725 4200 4725 2950
Wire Wire Line
	5550 2575 5550 2950
Connection ~ 5550 2950
Wire Wire Line
	5550 2950 5550 3100
Wire Wire Line
	6450 3600 7350 3600
Wire Wire Line
	6450 3700 7350 3700
Wire Wire Line
	6450 4000 7350 4000
Wire Wire Line
	6450 4100 7350 4100
Text HLabel 7350 3600 2    50   Output ~ 0
OUTLP
Text HLabel 7350 3700 2    50   Output ~ 0
OUTLN
Text HLabel 7350 4000 2    50   Output ~ 0
OUTRP
Text HLabel 7350 4100 2    50   Output ~ 0
OUTRN
$Comp
L Device:C_Small C61
U 1 1 60731100
P 7200 3100
F 0 "C61" H 7100 3050 50  0000 R CNN
F 1 "47uf" H 7100 3100 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7200 3100 50  0001 C CNN
F 3 "~" H 7200 3100 50  0001 C CNN
	1    7200 3100
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C60
U 1 1 60730200
P 6750 3100
F 0 "C60" H 6650 3025 50  0000 R CNN
F 1 "47uf" H 6650 3125 50  0000 R CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6750 3100 50  0001 C CNN
F 3 "~" H 6750 3100 50  0001 C CNN
	1    6750 3100
	-1   0    0    1   
$EndComp
Wire Wire Line
	6750 3200 6750 3300
Wire Wire Line
	6750 3300 6450 3300
Wire Wire Line
	6450 3400 7200 3400
Wire Wire Line
	7200 3400 7200 3200
Wire Wire Line
	7200 3000 7200 2950
Wire Wire Line
	7200 2950 6750 2950
Connection ~ 5950 2950
Wire Wire Line
	6750 3000 6750 2950
Connection ~ 6750 2950
Wire Wire Line
	6750 2950 5950 2950
Wire Wire Line
	4725 2950 5550 2950
$Comp
L power:PWR_FLAG #FLG?
U 1 1 60C30536
P 1575 825
AR Path="/5F16021E/60C30536" Ref="#FLG?"  Part="1" 
AR Path="/6071C2C4/60C30536" Ref="#FLG010"  Part="1" 
F 0 "#FLG010" H 1575 900 50  0001 C CNN
F 1 "PWR_FLAG" H 1575 999 50  0000 C CNN
F 2 "" H 1575 825 50  0001 C CNN
F 3 "~" H 1575 825 50  0001 C CNN
	1    1575 825 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG?
U 1 1 60C36C69
P 3350 775
AR Path="/5F16021E/60C36C69" Ref="#FLG?"  Part="1" 
AR Path="/6071C2C4/60C36C69" Ref="#FLG011"  Part="1" 
F 0 "#FLG011" H 3350 850 50  0001 C CNN
F 1 "PWR_FLAG" H 3350 949 50  0000 C CNN
F 2 "" H 3350 775 50  0001 C CNN
F 3 "~" H 3350 775 50  0001 C CNN
	1    3350 775 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 775  3350 975 
Wire Wire Line
	1575 825  1575 1000
Connection ~ 2925 1375
Connection ~ 1150 1400
$EndSCHEMATC