EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 11
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
L Device:D_Small D7
U 1 1 60C7D426
P 7475 1025
F 0 "D7" H 7450 775 50  0000 C CNN
F 1 "ss34" H 7450 875 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" V 7475 1025 50  0001 C CNN
F 3 "~" V 7475 1025 50  0001 C CNN
	1    7475 1025
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C68
U 1 1 60C7D42C
P 7950 1300
F 0 "C68" H 8050 1350 50  0000 L CNN
F 1 "22uf" H 8050 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7950 1300 50  0001 C CNN
F 3 "~" H 7950 1300 50  0001 C CNN
	1    7950 1300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C70
U 1 1 60C7D432
P 8475 1300
F 0 "C70" H 8600 1350 50  0000 L CNN
F 1 "22uf" H 8600 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8475 1300 50  0001 C CNN
F 3 "~" H 8475 1300 50  0001 C CNN
	1    8475 1300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C72
U 1 1 60C7D438
P 9025 1300
F 0 "C72" H 9150 1350 50  0000 L CNN
F 1 "100nf" H 9150 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9025 1300 50  0001 C CNN
F 3 "~" H 9025 1300 50  0001 C CNN
	1    9025 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 1200 7950 1025
Wire Wire Line
	7950 1025 7575 1025
Wire Wire Line
	8475 1200 8475 1025
Wire Wire Line
	8475 1025 7950 1025
Connection ~ 7950 1025
Wire Wire Line
	9025 1200 9025 1025
Wire Wire Line
	9025 1025 8475 1025
Connection ~ 8475 1025
Wire Wire Line
	7950 1425 7950 1400
Wire Wire Line
	8475 1400 8475 1425
Wire Wire Line
	8475 1425 7950 1425
Wire Wire Line
	9025 1400 9025 1425
Wire Wire Line
	9025 1425 8475 1425
Connection ~ 8475 1425
Wire Wire Line
	10625 1025 10625 850 
Connection ~ 9025 1025
$Comp
L power:PWR_FLAG #FLG010
U 1 1 60C7D454
P 10075 825
F 0 "#FLG010" H 10075 900 50  0001 C CNN
F 1 "PWR_FLAG" H 10100 1025 50  0000 C CNN
F 2 "" H 10075 825 50  0001 C CNN
F 3 "~" H 10075 825 50  0001 C CNN
	1    10075 825 
	1    0    0    -1  
$EndComp
Wire Wire Line
	10075 825  10075 1025
Wire Wire Line
	6800 1025 7375 1025
$Comp
L power:GND #PWR091
U 1 1 60C7D45C
P 9025 1600
F 0 "#PWR091" H 9025 1350 50  0001 C CNN
F 1 "GND" H 9050 1400 50  0000 C CNN
F 2 "" H 9025 1600 50  0001 C CNN
F 3 "" H 9025 1600 50  0001 C CNN
	1    9025 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	9025 1600 9025 1425
Connection ~ 9025 1425
$Comp
L power:GND #PWR085
U 1 1 60C7D46F
P 2875 5250
F 0 "#PWR085" H 2875 5000 50  0001 C CNN
F 1 "GND" H 2900 5025 50  0000 C CNN
F 2 "" H 2875 5250 50  0001 C CNN
F 3 "" H 2875 5250 50  0001 C CNN
	1    2875 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR087
U 1 1 60C7D4AD
P 4525 5250
F 0 "#PWR087" H 4525 5000 50  0001 C CNN
F 1 "GND" H 4575 5025 50  0000 C CNN
F 2 "" H 4525 5250 50  0001 C CNN
F 3 "" H 4525 5250 50  0001 C CNN
	1    4525 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4525 4000 4525 3900
Text Label 5450 4300 0    50   ~ 0
FB1
Wire Wire Line
	5100 4300 5675 4300
$Comp
L Device:R_Small R48
U 1 1 60C7D4F3
P 4750 4650
F 0 "R48" H 4825 4725 50  0000 L CNN
F 1 "1k" H 4825 4625 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 4750 4650 50  0001 C CNN
F 3 "~" H 4750 4650 50  0001 C CNN
	1    4750 4650
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R50
U 1 1 60C7D4F9
P 5000 4300
F 0 "R50" V 4775 4325 50  0000 C CNN
F 1 "13k" V 4875 4325 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 5000 4300 50  0001 C CNN
F 3 "~" H 5000 4300 50  0001 C CNN
	1    5000 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 4300 4750 4300
Wire Wire Line
	4750 4550 4750 4300
Connection ~ 4750 4300
Wire Wire Line
	4750 4300 4525 4300
Wire Wire Line
	4750 4750 4750 4950
Wire Wire Line
	4750 4950 4525 4950
Connection ~ 4525 4950
Wire Wire Line
	4525 4950 4525 5250
Text Label 9025 925  1    50   ~ 0
FB1
Wire Wire Line
	925  3675 925  3900
$Comp
L Device:L_Small L10
U 1 1 60C7D510
P 9325 1025
F 0 "L10" V 9525 1025 50  0000 C CNN
F 1 "2.2uh / 0805" V 9425 1000 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 9325 1025 50  0001 C CNN
F 3 "~" H 9325 1025 50  0001 C CNN
	1    9325 1025
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9025 1025 9225 1025
Wire Wire Line
	9425 1025 9625 1025
$Comp
L Device:C_Small C74
U 1 1 60C7D518
P 9625 1300
F 0 "C74" H 9750 1350 50  0000 L CNN
F 1 "22uf" H 9750 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9625 1300 50  0001 C CNN
F 3 "~" H 9625 1300 50  0001 C CNN
	1    9625 1300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C76
U 1 1 60C7D51E
P 10075 1300
F 0 "C76" H 10200 1350 50  0000 L CNN
F 1 "22uf" H 10200 1300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10075 1300 50  0001 C CNN
F 3 "~" H 10075 1300 50  0001 C CNN
	1    10075 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 1200 9625 1025
Connection ~ 9625 1025
Wire Wire Line
	9625 1400 9625 1425
Wire Wire Line
	10075 1400 10075 1425
Wire Wire Line
	10075 1425 9625 1425
Wire Wire Line
	9625 1025 10075 1025
Wire Wire Line
	10075 1200 10075 1025
Connection ~ 10075 1025
Wire Wire Line
	10075 1025 10625 1025
Wire Wire Line
	9025 1025 9025 725 
Wire Wire Line
	9625 1425 9025 1425
Connection ~ 9625 1425
$Comp
L Device:C_Small C67
U 1 1 60CC4997
P 7950 2850
F 0 "C67" H 8050 2900 50  0000 L CNN
F 1 "22uf" H 8050 2850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7950 2850 50  0001 C CNN
F 3 "~" H 7950 2850 50  0001 C CNN
	1    7950 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C69
U 1 1 60CC499D
P 8475 2850
F 0 "C69" H 8600 2900 50  0000 L CNN
F 1 "22uf" H 8600 2850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8475 2850 50  0001 C CNN
F 3 "~" H 8475 2850 50  0001 C CNN
	1    8475 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C71
U 1 1 60CC49A3
P 9025 2850
F 0 "C71" H 9150 2900 50  0000 L CNN
F 1 "100nf" H 9150 2850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9025 2850 50  0001 C CNN
F 3 "~" H 9025 2850 50  0001 C CNN
	1    9025 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 2750 7950 2575
Wire Wire Line
	8475 2750 8475 2575
Wire Wire Line
	8475 2575 7950 2575
Wire Wire Line
	9025 2750 9025 2575
Wire Wire Line
	9025 2575 8475 2575
Connection ~ 8475 2575
Wire Wire Line
	7950 2975 7950 2950
Wire Wire Line
	8475 2950 8475 2975
Wire Wire Line
	8475 2975 7950 2975
Wire Wire Line
	9025 2950 9025 2975
Wire Wire Line
	9025 2975 8475 2975
Connection ~ 8475 2975
Wire Wire Line
	10625 2575 10625 2400
Connection ~ 9025 2575
$Comp
L power:PWR_FLAG #FLG09
U 1 1 60CC49BF
P 10075 2375
F 0 "#FLG09" H 10075 2450 50  0001 C CNN
F 1 "PWR_FLAG" H 10100 2575 50  0000 C CNN
F 2 "" H 10075 2375 50  0001 C CNN
F 3 "~" H 10075 2375 50  0001 C CNN
	1    10075 2375
	1    0    0    -1  
$EndComp
Wire Wire Line
	10075 2375 10075 2575
$Comp
L power:GND #PWR090
U 1 1 60CC49C7
P 9025 3150
F 0 "#PWR090" H 9025 2900 50  0001 C CNN
F 1 "GND" H 9050 2950 50  0000 C CNN
F 2 "" H 9025 3150 50  0001 C CNN
F 3 "" H 9025 3150 50  0001 C CNN
	1    9025 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9025 3150 9025 2975
Connection ~ 9025 2975
$Comp
L Device:L_Small L9
U 1 1 60CC4A72
P 9325 2575
F 0 "L9" V 9525 2575 50  0000 C CNN
F 1 "2.2uh / 0805" V 9425 2550 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 9325 2575 50  0001 C CNN
F 3 "~" H 9325 2575 50  0001 C CNN
	1    9325 2575
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9025 2575 9225 2575
Wire Wire Line
	9425 2575 9625 2575
$Comp
L Device:C_Small C73
U 1 1 60CC4A7A
P 9625 2850
F 0 "C73" H 9750 2900 50  0000 L CNN
F 1 "22uf" H 9750 2850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9625 2850 50  0001 C CNN
F 3 "~" H 9625 2850 50  0001 C CNN
	1    9625 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C75
U 1 1 60CC4A80
P 10075 2850
F 0 "C75" H 10200 2900 50  0000 L CNN
F 1 "22uf" H 10200 2850 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10075 2850 50  0001 C CNN
F 3 "~" H 10075 2850 50  0001 C CNN
	1    10075 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 2750 9625 2575
Connection ~ 9625 2575
Wire Wire Line
	9625 2950 9625 2975
Wire Wire Line
	10075 2950 10075 2975
Wire Wire Line
	10075 2975 9625 2975
Wire Wire Line
	9625 2575 10075 2575
Wire Wire Line
	10075 2750 10075 2575
Connection ~ 10075 2575
Wire Wire Line
	10075 2575 10625 2575
Wire Wire Line
	9625 2975 9025 2975
Connection ~ 9625 2975
$Comp
L power:+15V #PWR093
U 1 1 60CC9BDF
P 10625 850
F 0 "#PWR093" H 10625 700 50  0001 C CNN
F 1 "+15V" H 10675 1050 50  0000 C CNN
F 2 "" H 10625 850 50  0001 C CNN
F 3 "" H 10625 850 50  0001 C CNN
	1    10625 850 
	1    0    0    -1  
$EndComp
$Comp
L power:-15V #PWR092
U 1 1 60CCAD7B
P 10625 2400
F 0 "#PWR092" H 10625 2500 50  0001 C CNN
F 1 "-15V" H 10675 2625 50  0000 C CNN
F 2 "" H 10625 2400 50  0001 C CNN
F 3 "" H 10625 2400 50  0001 C CNN
	1    10625 2400
	1    0    0    -1  
$EndComp
Text HLabel 1400 950  0    50   Input ~ 0
EN
$Comp
L Device:R_Small R42
U 1 1 60C6A5EC
P 1750 950
F 0 "R42" V 1525 950 50  0000 C CNN
F 1 "9.53k" V 1625 950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1750 950 50  0001 C CNN
F 3 "~" H 1750 950 50  0001 C CNN
	1    1750 950 
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R41
U 1 1 60C6BBD4
P 1525 1150
F 0 "R41" H 1625 1200 50  0000 L CNN
F 1 "9.53k" H 1625 1150 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" H 1525 1150 50  0001 C CNN
F 3 "~" H 1525 1150 50  0001 C CNN
	1    1525 1150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR073
U 1 1 60C7B219
P 1525 1350
F 0 "#PWR073" H 1525 1100 50  0001 C CNN
F 1 "GND" H 1575 1150 50  0000 C CNN
F 2 "" H 1525 1350 50  0001 C CNN
F 3 "" H 1525 1350 50  0001 C CNN
	1    1525 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1525 1050 1525 950 
Wire Wire Line
	1525 1350 1525 1250
Text Label 1925 950  0    50   ~ 0
enx
Wire Wire Line
	2875 4000 2350 4000
Text Label 2400 4000 0    50   ~ 0
enx
Wire Wire Line
	1850 950  2100 950 
Wire Wire Line
	1400 950  1525 950 
Connection ~ 1525 950 
Wire Wire Line
	1525 950  1650 950 
$Comp
L Device:C_Small C63
U 1 1 60CF7E28
P 2425 4675
F 0 "C63" H 2525 4725 50  0000 L CNN
F 1 "NC" H 2525 4675 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2425 4675 50  0001 C CNN
F 3 "~" H 2425 4675 50  0001 C CNN
	1    2425 4675
	1    0    0    -1  
$EndComp
Wire Wire Line
	2425 4775 2425 5200
Wire Wire Line
	2425 5200 2025 5200
Wire Wire Line
	2425 4575 2425 4350
Connection ~ 2425 4350
Wire Wire Line
	2425 4350 2875 4350
$Comp
L power:GND #PWR075
U 1 1 60C7D4BE
P 925 5250
F 0 "#PWR075" H 925 5000 50  0001 C CNN
F 1 "GND" H 975 5025 50  0000 C CNN
F 2 "" H 925 5250 50  0001 C CNN
F 3 "" H 925 5250 50  0001 C CNN
	1    925  5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	925  4575 925  4375
Wire Wire Line
	925  4775 925  4950
$Comp
L Device:C_Small C57
U 1 1 60C7D4B6
P 925 4675
F 0 "C57" H 1025 4725 50  0000 L CNN
F 1 "10uf" H 1025 4675 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 925 4675 50  0001 C CNN
F 3 "~" H 925 4675 50  0001 C CNN
	1    925  4675
	1    0    0    -1  
$EndComp
Wire Wire Line
	1625 4775 1625 5250
Wire Wire Line
	1275 4775 1275 5250
$Comp
L power:GND #PWR082
U 1 1 60C7D4A1
P 2025 5250
F 0 "#PWR082" H 2025 5000 50  0001 C CNN
F 1 "GND" H 2050 5025 50  0000 C CNN
F 2 "" H 2025 5250 50  0001 C CNN
F 3 "" H 2025 5250 50  0001 C CNN
	1    2025 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR080
U 1 1 60C7D49B
P 1625 5250
F 0 "#PWR080" H 1625 5000 50  0001 C CNN
F 1 "GND" H 1650 5050 50  0000 C CNN
F 2 "" H 1625 5250 50  0001 C CNN
F 3 "" H 1625 5250 50  0001 C CNN
	1    1625 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR078
U 1 1 60C7D495
P 1275 5250
F 0 "#PWR078" H 1275 5000 50  0001 C CNN
F 1 "GND" H 1325 5025 50  0000 C CNN
F 2 "" H 1275 5250 50  0001 C CNN
F 3 "" H 1275 5250 50  0001 C CNN
	1    1275 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2025 5150 2025 5200
Wire Wire Line
	2025 4775 2025 4950
Wire Wire Line
	1275 4150 1275 4575
Wire Wire Line
	1625 4250 1625 4575
Wire Wire Line
	2025 4575 2025 4350
$Comp
L Device:R_Small R45
U 1 1 60C7D489
P 2025 4675
F 0 "R45" H 2125 4750 50  0000 L CNN
F 1 "2.37k" H 2125 4650 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2025 4675 50  0001 C CNN
F 3 "~" H 2025 4675 50  0001 C CNN
	1    2025 4675
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C61
U 1 1 60C7D483
P 2025 5050
F 0 "C61" H 2150 5100 50  0000 L CNN
F 1 "100nf" H 2150 5050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2025 5050 50  0001 C CNN
F 3 "~" H 2025 5050 50  0001 C CNN
	1    2025 5050
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C59
U 1 1 60C7D47D
P 1625 4675
F 0 "C59" H 1750 4725 50  0000 L CNN
F 1 "47nf" H 1750 4675 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1625 4675 50  0001 C CNN
F 3 "~" H 1625 4675 50  0001 C CNN
	1    1625 4675
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R43
U 1 1 60C7D477
P 1275 4675
F 0 "R43" H 1375 4725 50  0000 L CNN
F 1 "43k" H 1375 4675 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 1275 4675 50  0001 C CNN
F 3 "~" H 1275 4675 50  0001 C CNN
	1    1275 4675
	1    0    0    -1  
$EndComp
Connection ~ 2025 5200
Wire Wire Line
	2025 5200 2025 5250
Wire Wire Line
	2025 4350 2425 4350
Wire Wire Line
	1625 4250 2875 4250
Wire Wire Line
	1275 4150 2875 4150
Connection ~ 925  3900
Wire Wire Line
	925  3900 2875 3900
Connection ~ 7950 2575
Wire Wire Line
	7950 2575 7575 2575
$Comp
L Device:D_Small D?
U 1 1 60CC4991
P 7475 2575
AR Path="/5F16021E/60CC4991" Ref="D?"  Part="1" 
AR Path="/6074FC63/60CC4991" Ref="D6"  Part="1" 
F 0 "D6" H 7450 2325 50  0000 C CNN
F 1 "ss34" H 7450 2425 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" V 7475 2575 50  0001 C CNN
F 3 "~" V 7475 2575 50  0001 C CNN
	1    7475 2575
	1    0    0    -1  
$EndComp
Wire Wire Line
	5325 3200 5750 3200
Wire Wire Line
	5325 2225 5400 2225
Wire Wire Line
	5400 2225 5400 2125
Wire Wire Line
	5400 2025 5325 2025
Wire Wire Line
	5400 2125 5750 2125
Connection ~ 5400 2125
Wire Wire Line
	5400 2125 5400 2025
Wire Wire Line
	5325 1025 5750 1025
$Comp
L power:GND #PWR0126
U 1 1 60DAF4DF
P 5750 2125
F 0 "#PWR0126" H 5750 1875 50  0001 C CNN
F 1 "GND" V 5775 1975 50  0000 R CNN
F 2 "" H 5750 2125 50  0001 C CNN
F 3 "" H 5750 2125 50  0001 C CNN
	1    5750 2125
	0    -1   -1   0   
$EndComp
Text Label 5500 1025 0    50   ~ 0
out-
Text Label 7000 2575 0    50   ~ 0
out-
Wire Wire Line
	7375 2575 6875 2575
Text Label 5500 3200 0    50   ~ 0
out+
Text Label 7000 1025 0    50   ~ 0
out+
$Comp
L Device:C_Small C58
U 1 1 610F7DFC
P 550 4675
F 0 "C58" H 650 4725 50  0000 L CNN
F 1 "10uf" H 650 4675 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 550 4675 50  0001 C CNN
F 3 "~" H 550 4675 50  0001 C CNN
	1    550  4675
	1    0    0    -1  
$EndComp
Wire Wire Line
	550  4575 550  4375
Wire Wire Line
	550  4375 925  4375
Connection ~ 925  4375
Wire Wire Line
	925  4375 925  3900
Wire Wire Line
	550  4775 550  4950
Wire Wire Line
	550  4950 925  4950
Connection ~ 925  4950
Wire Wire Line
	925  4950 925  5250
$Comp
L power:VCC #PWR045
U 1 1 60E5B30C
P 925 3675
F 0 "#PWR045" H 925 3525 50  0001 C CNN
F 1 "VCC" H 950 3875 50  0000 C CNN
F 2 "" H 925 3675 50  0001 C CNN
F 3 "" H 925 3675 50  0001 C CNN
	1    925  3675
	1    0    0    -1  
$EndComp
$Comp
L misc:Transformer_diy_02 T1
U 1 1 60E7BCB2
P 5125 1525
F 0 "T1" H 5150 2325 50  0000 C CNN
F 1 "Transformer_diy_02" H 5125 2175 50  0000 C CNN
F 2 "misc:Transformer_10p_22x14" H 5125 1525 50  0001 C CNN
F 3 "" H 4825 1525 50  0001 C CNN
	1    5125 1525
	1    0    0    -1  
$EndComp
Text Label 2400 3900 0    50   ~ 0
win
Text Label 4725 3900 0    50   ~ 0
sw
Text Label 4575 1325 0    50   ~ 0
win
Wire Wire Line
	4925 1325 4350 1325
Wire Wire Line
	4925 1725 4350 1725
Text Label 4575 1725 0    50   ~ 0
sw
Connection ~ 4525 3900
Wire Wire Line
	2875 4450 2875 4650
Connection ~ 2875 4650
Wire Wire Line
	2875 4750 2875 5250
Wire Wire Line
	2875 4650 2875 4750
Connection ~ 2875 4750
Wire Wire Line
	4525 4550 4525 4650
Connection ~ 4525 4650
Wire Wire Line
	4525 4750 4525 4950
Wire Wire Line
	4525 4650 4525 4750
Connection ~ 4525 4750
$Comp
L ti:TPS55340 U10
U 1 1 60C7D464
P 2875 3900
F 0 "U10" H 3700 4450 60  0000 C CNN
F 1 "TPS55340" H 3700 4300 60  0000 C CNN
F 2 "Package_SO:HTSSOP-14-1EP_4.4x5mm_P0.65mm_EP3.4x5mm_Mask3x3.1mm_ThermalVias" H 3725 3050 60  0001 C CNN
F 3 "" H 3700 4200 60  0000 C CNN
	1    2875 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4525 3900 4925 3900
Wire Wire Line
	5325 1425 5325 1525
Wire Wire Line
	5325 2625 5325 2725
Wire Wire Line
	5325 1525 5750 1525
Connection ~ 5325 1525
Wire Wire Line
	5325 1525 5325 1625
Wire Wire Line
	5325 2725 5750 2725
Connection ~ 5325 2725
Wire Wire Line
	5325 2725 5325 2800
Text Label 5525 1525 0    50   ~ 0
out2-
Text Label 5525 2725 0    50   ~ 0
out2+
$Comp
L Device:D_Small D8
U 1 1 60EE7AA6
P 7475 4050
F 0 "D8" H 7450 3800 50  0000 C CNN
F 1 "ss34" H 7450 3900 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" V 7475 4050 50  0001 C CNN
F 3 "~" V 7475 4050 50  0001 C CNN
	1    7475 4050
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C78
U 1 1 60EE7AAC
P 7950 4325
F 0 "C78" H 8050 4375 50  0000 L CNN
F 1 "22uf" H 8050 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7950 4325 50  0001 C CNN
F 3 "~" H 7950 4325 50  0001 C CNN
	1    7950 4325
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C80
U 1 1 60EE7AB2
P 8475 4325
F 0 "C80" H 8600 4375 50  0000 L CNN
F 1 "22uf" H 8600 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8475 4325 50  0001 C CNN
F 3 "~" H 8475 4325 50  0001 C CNN
	1    8475 4325
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C82
U 1 1 60EE7AB8
P 9025 4325
F 0 "C82" H 9150 4375 50  0000 L CNN
F 1 "100nf" H 9150 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9025 4325 50  0001 C CNN
F 3 "~" H 9025 4325 50  0001 C CNN
	1    9025 4325
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 4225 7950 4050
Wire Wire Line
	7950 4050 7575 4050
Wire Wire Line
	8475 4225 8475 4050
Wire Wire Line
	8475 4050 7950 4050
Connection ~ 7950 4050
Wire Wire Line
	9025 4225 9025 4050
Wire Wire Line
	9025 4050 8475 4050
Connection ~ 8475 4050
Wire Wire Line
	7950 4450 7950 4425
Wire Wire Line
	8475 4425 8475 4450
Wire Wire Line
	8475 4450 7950 4450
Wire Wire Line
	9025 4425 9025 4450
Wire Wire Line
	9025 4450 8475 4450
Connection ~ 8475 4450
Wire Wire Line
	10625 4050 10625 3875
Connection ~ 9025 4050
$Comp
L power:PWR_FLAG #FLG03
U 1 1 60EE7ACE
P 10075 3850
F 0 "#FLG03" H 10075 3925 50  0001 C CNN
F 1 "PWR_FLAG" H 10100 4050 50  0000 C CNN
F 2 "" H 10075 3850 50  0001 C CNN
F 3 "~" H 10075 3850 50  0001 C CNN
	1    10075 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	10075 3850 10075 4050
Wire Wire Line
	6800 4050 7375 4050
$Comp
L power:GND #PWR047
U 1 1 60EE7AD6
P 9025 4625
F 0 "#PWR047" H 9025 4375 50  0001 C CNN
F 1 "GND" H 9050 4425 50  0000 C CNN
F 2 "" H 9025 4625 50  0001 C CNN
F 3 "" H 9025 4625 50  0001 C CNN
	1    9025 4625
	1    0    0    -1  
$EndComp
Wire Wire Line
	9025 4625 9025 4450
Connection ~ 9025 4450
$Comp
L Device:L_Small L5
U 1 1 60EE7ADF
P 9325 4050
F 0 "L5" V 9525 4050 50  0000 C CNN
F 1 "2.2uh / 0805" V 9425 4025 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 9325 4050 50  0001 C CNN
F 3 "~" H 9325 4050 50  0001 C CNN
	1    9325 4050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9025 4050 9225 4050
Wire Wire Line
	9425 4050 9625 4050
$Comp
L Device:C_Small C84
U 1 1 60EE7AE7
P 9625 4325
F 0 "C84" H 9750 4375 50  0000 L CNN
F 1 "22uf" H 9750 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9625 4325 50  0001 C CNN
F 3 "~" H 9625 4325 50  0001 C CNN
	1    9625 4325
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C86
U 1 1 60EE7AED
P 10075 4325
F 0 "C86" H 10200 4375 50  0000 L CNN
F 1 "22uf" H 10200 4325 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10075 4325 50  0001 C CNN
F 3 "~" H 10075 4325 50  0001 C CNN
	1    10075 4325
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 4225 9625 4050
Connection ~ 9625 4050
Wire Wire Line
	9625 4425 9625 4450
Wire Wire Line
	10075 4425 10075 4450
Wire Wire Line
	10075 4450 9625 4450
Wire Wire Line
	9625 4050 10075 4050
Wire Wire Line
	10075 4225 10075 4050
Connection ~ 10075 4050
Wire Wire Line
	10075 4050 10625 4050
Wire Wire Line
	9625 4450 9025 4450
Connection ~ 9625 4450
Text Label 7000 4050 0    50   ~ 0
out2+
$Comp
L Device:C_Small C79
U 1 1 60EF516B
P 7950 5925
F 0 "C79" H 8050 5975 50  0000 L CNN
F 1 "22uf" H 8050 5925 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7950 5925 50  0001 C CNN
F 3 "~" H 7950 5925 50  0001 C CNN
	1    7950 5925
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C81
U 1 1 60EF5171
P 8475 5925
F 0 "C81" H 8600 5975 50  0000 L CNN
F 1 "22uf" H 8600 5925 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8475 5925 50  0001 C CNN
F 3 "~" H 8475 5925 50  0001 C CNN
	1    8475 5925
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C83
U 1 1 60EF5177
P 9025 5925
F 0 "C83" H 9150 5975 50  0000 L CNN
F 1 "100nf" H 9150 5925 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9025 5925 50  0001 C CNN
F 3 "~" H 9025 5925 50  0001 C CNN
	1    9025 5925
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 5825 7950 5650
Wire Wire Line
	8475 5825 8475 5650
Wire Wire Line
	8475 5650 7950 5650
Wire Wire Line
	9025 5825 9025 5650
Wire Wire Line
	9025 5650 8475 5650
Connection ~ 8475 5650
Wire Wire Line
	7950 6050 7950 6025
Wire Wire Line
	8475 6025 8475 6050
Wire Wire Line
	8475 6050 7950 6050
Wire Wire Line
	9025 6025 9025 6050
Wire Wire Line
	9025 6050 8475 6050
Connection ~ 8475 6050
Wire Wire Line
	10625 5650 10625 5475
Connection ~ 9025 5650
$Comp
L power:PWR_FLAG #FLG011
U 1 1 60EF518B
P 10075 5450
F 0 "#FLG011" H 10075 5525 50  0001 C CNN
F 1 "PWR_FLAG" H 10100 5650 50  0000 C CNN
F 2 "" H 10075 5450 50  0001 C CNN
F 3 "~" H 10075 5450 50  0001 C CNN
	1    10075 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	10075 5450 10075 5650
$Comp
L power:GND #PWR074
U 1 1 60EF5192
P 9025 6225
F 0 "#PWR074" H 9025 5975 50  0001 C CNN
F 1 "GND" H 9050 6025 50  0000 C CNN
F 2 "" H 9025 6225 50  0001 C CNN
F 3 "" H 9025 6225 50  0001 C CNN
	1    9025 6225
	1    0    0    -1  
$EndComp
Wire Wire Line
	9025 6225 9025 6050
Connection ~ 9025 6050
$Comp
L Device:L_Small L6
U 1 1 60EF519A
P 9325 5650
F 0 "L6" V 9525 5650 50  0000 C CNN
F 1 "2.2uh / 0805" V 9425 5625 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 9325 5650 50  0001 C CNN
F 3 "~" H 9325 5650 50  0001 C CNN
	1    9325 5650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9025 5650 9225 5650
Wire Wire Line
	9425 5650 9625 5650
$Comp
L Device:C_Small C85
U 1 1 60EF51A2
P 9625 5925
F 0 "C85" H 9750 5975 50  0000 L CNN
F 1 "22uf" H 9750 5925 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9625 5925 50  0001 C CNN
F 3 "~" H 9625 5925 50  0001 C CNN
	1    9625 5925
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C87
U 1 1 60EF51A8
P 10075 5925
F 0 "C87" H 10200 5975 50  0000 L CNN
F 1 "22uf" H 10200 5925 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10075 5925 50  0001 C CNN
F 3 "~" H 10075 5925 50  0001 C CNN
	1    10075 5925
	1    0    0    -1  
$EndComp
Wire Wire Line
	9625 5825 9625 5650
Connection ~ 9625 5650
Wire Wire Line
	9625 6025 9625 6050
Wire Wire Line
	10075 6025 10075 6050
Wire Wire Line
	10075 6050 9625 6050
Wire Wire Line
	9625 5650 10075 5650
Wire Wire Line
	10075 5825 10075 5650
Connection ~ 10075 5650
Wire Wire Line
	10075 5650 10625 5650
Wire Wire Line
	9625 6050 9025 6050
Connection ~ 9625 6050
Connection ~ 7950 5650
Wire Wire Line
	7950 5650 7575 5650
$Comp
L Device:D_Small D?
U 1 1 60EF51C1
P 7475 5650
AR Path="/5F16021E/60EF51C1" Ref="D?"  Part="1" 
AR Path="/6074FC63/60EF51C1" Ref="D9"  Part="1" 
F 0 "D9" H 7450 5400 50  0000 C CNN
F 1 "ss34" H 7450 5500 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" V 7475 5650 50  0001 C CNN
F 3 "~" V 7475 5650 50  0001 C CNN
	1    7475 5650
	1    0    0    -1  
$EndComp
Text Label 7000 5650 0    50   ~ 0
out2-
Wire Wire Line
	7375 5650 6875 5650
$Comp
L power:-8V #PWR077
U 1 1 60F00A3C
P 10625 5475
F 0 "#PWR077" H 10625 5575 50  0001 C CNN
F 1 "-8V" H 10650 5700 50  0000 C CNN
F 2 "" H 10625 5475 50  0001 C CNN
F 3 "" H 10625 5475 50  0001 C CNN
	1    10625 5475
	1    0    0    -1  
$EndComp
$Comp
L power:+8V #PWR076
U 1 1 60F013C0
P 10625 3875
F 0 "#PWR076" H 10625 3725 50  0001 C CNN
F 1 "+8V" H 10675 4075 50  0000 C CNN
F 2 "" H 10625 3875 50  0001 C CNN
F 3 "" H 10625 3875 50  0001 C CNN
	1    10625 3875
	1    0    0    -1  
$EndComp
$EndSCHEMATC