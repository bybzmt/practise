EESchema Schematic File Version 4
LIBS:oven2-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
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
	9900 1750 10250 1750
Wire Wire Line
	9900 1850 10250 1850
Wire Wire Line
	8550 3600 8550 3950
Wire Wire Line
	8050 1850 7700 1850
Text Label 10050 1750 0    50   ~ 0
GND
Text Label 8550 3850 1    50   ~ 0
GND
Text Label 7800 1850 0    50   ~ 0
GND
Wire Wire Line
	9900 3150 10250 3150
Wire Wire Line
	9900 2150 10250 2150
Wire Wire Line
	9900 2250 10250 2250
$Sheet
S 1400 1800 950  600 
U 5DDE5893
F0 "power" 50
F1 "power.sch" 50
$EndSheet
$Sheet
S 1400 3900 950  500 
U 5DDFA9C8
F0 "max7219" 50
F1 "max7219.sch" 50
F2 "sck" I R 2350 3950 50 
F3 "mosi" I R 2350 4050 50 
F4 "cs3" I R 2350 4150 50 
$EndSheet
$Sheet
S 1400 2850 950  550 
U 5DDE4FEC
F0 "max38176" 50
F1 "max38176.sch" 50
F2 "sck" I R 2350 2900 50 
F3 "miso" I R 2350 3000 50 
F4 "mosi" I R 2350 3100 50 
F5 "cs1" I R 2350 3200 50 
F6 "cs2" I R 2350 3300 50 
$EndSheet
$Comp
L power:GND #PWR07
U 1 1 5DE10D8A
P 10400 1750
F 0 "#PWR07" H 10400 1500 50  0001 C CNN
F 1 "GND" H 10405 1577 50  0000 C CNN
F 2 "" H 10400 1750 50  0001 C CNN
F 3 "" H 10400 1750 50  0001 C CNN
	1    10400 1750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10250 1850 10250 1750
Wire Wire Line
	10250 1750 10400 1750
Connection ~ 10250 1750
$Comp
L power:VDD #PWR06
U 1 1 5DE115AB
P 7450 1200
F 0 "#PWR06" H 7450 1050 50  0001 C CNN
F 1 "VDD" H 7467 1373 50  0000 C CNN
F 2 "" H 7450 1200 50  0001 C CNN
F 3 "" H 7450 1200 50  0001 C CNN
	1    7450 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 1950 8050 1950
$Comp
L Device:C C3
U 1 1 5DE11A84
P 7950 1350
F 0 "C3" V 8202 1350 50  0000 C CNN
F 1 "100nf" V 8111 1350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7988 1200 50  0001 C CNN
F 3 "~" H 7950 1350 50  0001 C CNN
	1    7950 1350
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C2
U 1 1 5DE11C3A
P 7950 800
F 0 "C2" V 7698 800 50  0000 C CNN
F 1 "10uf" V 7789 800 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7988 650 50  0001 C CNN
F 3 "~" H 7950 800 50  0001 C CNN
	1    7950 800 
	0    1    1    0   
$EndComp
Wire Wire Line
	7700 1850 7700 1600
Wire Wire Line
	7700 1600 8100 1600
Wire Wire Line
	8100 1600 8100 1350
Wire Wire Line
	8100 800  8100 1350
Connection ~ 8100 1350
Wire Wire Line
	7800 800  7800 1350
Wire Wire Line
	7800 1350 7450 1350
Wire Wire Line
	7450 1350 7450 1950
Connection ~ 7800 1350
Wire Wire Line
	7450 1350 7450 1200
Connection ~ 7450 1350
$Comp
L Device:R R6
U 1 1 5DE1340A
P 7100 1700
F 0 "R6" H 7170 1746 50  0000 L CNN
F 1 "9.53k" H 7170 1655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 7030 1700 50  0001 C CNN
F 3 "~" H 7100 1700 50  0001 C CNN
	1    7100 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5DE1346A
P 6650 2050
F 0 "C1" V 6902 2050 50  0000 C CNN
F 1 "100nf" V 6811 2050 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6688 1900 50  0001 C CNN
F 3 "~" H 6650 2050 50  0001 C CNN
	1    6650 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5DE13548
P 6100 2050
F 0 "#PWR05" H 6100 1800 50  0001 C CNN
F 1 "GND" V 6105 1922 50  0000 R CNN
F 2 "" H 6100 2050 50  0001 C CNN
F 3 "" H 6100 2050 50  0001 C CNN
	1    6100 2050
	0    1    1    0   
$EndComp
Wire Wire Line
	7100 2050 8050 2050
Wire Wire Line
	7100 1850 7100 2050
Wire Wire Line
	6800 2050 7100 2050
Connection ~ 7100 2050
Wire Wire Line
	7100 1550 7100 1350
Wire Wire Line
	7100 1350 7450 1350
Wire Wire Line
	6100 2050 6500 2050
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 5DE153D0
P 4600 5300
F 0 "J3" H 4680 5342 50  0000 L CNN
F 1 "tx" H 4680 5251 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x01_P2.54mm_Vertical" H 4600 5300 50  0001 C CNN
F 3 "~" H 4600 5300 50  0001 C CNN
	1    4600 5300
	1    0    0    -1  
$EndComp
Text Label 10050 3150 0    50   ~ 0
IO0
Text Label 10050 2150 0    50   ~ 0
tx
Text Label 10050 2250 0    50   ~ 0
rx
Wire Wire Line
	4400 5500 4050 5500
Text Label 4200 5100 0    50   ~ 0
rx
Text Label 4200 5300 0    50   ~ 0
tx
Wire Wire Line
	4400 5300 4050 5300
Wire Wire Line
	4400 5100 4050 5100
Text Label 4200 5500 0    50   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 5DE38102
P 4600 5100
F 0 "J2" H 4680 5142 50  0000 L CNN
F 1 "rx" H 4680 5051 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x01_P2.54mm_Vertical" H 4600 5100 50  0001 C CNN
F 3 "~" H 4600 5100 50  0001 C CNN
	1    4600 5100
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 5DE3A192
P 4600 5500
F 0 "J5" H 4680 5542 50  0000 L CNN
F 1 "io0" H 4680 5451 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x01_P2.54mm_Vertical" H 4600 5500 50  0001 C CNN
F 3 "~" H 4600 5500 50  0001 C CNN
	1    4600 5500
	1    0    0    -1  
$EndComp
Text Label 4200 5700 0    50   ~ 0
IO0
Wire Wire Line
	4400 5700 4050 5700
NoConn ~ 9900 2750
Text Label 10050 2550 0    50   ~ 0
e1_a
Wire Wire Line
	7350 2550 8050 2550
Wire Wire Line
	7350 2650 8050 2650
Text Label 10050 2650 0    50   ~ 0
e1_b
Wire Wire Line
	2900 2900 2350 2900
Wire Wire Line
	2900 3000 2350 3000
Wire Wire Line
	2900 3100 2350 3100
Wire Wire Line
	2900 3200 2350 3200
Wire Wire Line
	2900 3300 2350 3300
Wire Wire Line
	2900 3950 2350 3950
Wire Wire Line
	2900 4050 2350 4050
Wire Wire Line
	2900 4150 2350 4150
Text Label 2650 2900 0    50   ~ 0
sck
Text Label 2650 3000 0    50   ~ 0
miso
Text Label 2650 3100 0    50   ~ 0
mosi
Text Label 2650 3200 0    50   ~ 0
cs1
Text Label 2650 3300 0    50   ~ 0
cs2
Text Label 2650 3950 0    50   ~ 0
sck
Text Label 2650 4050 0    50   ~ 0
mosi
Text Label 2650 4150 0    50   ~ 0
cs3
Wire Wire Line
	10250 1950 9900 1950
Wire Wire Line
	10250 2050 9900 2050
Wire Wire Line
	10250 2350 9900 2350
Wire Wire Line
	8050 2350 7350 2350
Wire Wire Line
	7350 2450 8050 2450
Wire Wire Line
	8050 2750 7350 2750
Text Label 7450 2350 0    50   ~ 0
cs1
Text Label 7450 2450 0    50   ~ 0
cs2
Text Label 7450 2550 0    50   ~ 0
cs3
Text Label 10050 1950 0    50   ~ 0
sck
Text Label 10050 2050 0    50   ~ 0
miso
Text Label 10050 2350 0    50   ~ 0
mosi
Wire Wire Line
	10250 2550 9900 2550
Wire Wire Line
	10250 2650 9900 2650
Wire Wire Line
	9900 2850 10250 2850
Wire Wire Line
	10250 2950 9900 2950
Wire Wire Line
	10250 3050 9900 3050
Wire Wire Line
	7350 2850 8050 2850
Wire Wire Line
	7350 2950 8050 2950
Text Label 7450 2650 0    50   ~ 0
out1
Text Label 7450 2750 0    50   ~ 0
out2
Text Label 7450 2850 0    50   ~ 0
out3
Text Label 7450 2950 0    50   ~ 0
out4
Text Label 10050 3050 0    50   ~ 0
mode
Text Label 10050 2850 0    50   ~ 0
e2_a
Text Label 10050 2950 0    50   ~ 0
e2_b
$Comp
L oven2:esp32-wroom-32u U1
U 1 1 5DDD2413
P 8950 2600
F 0 "U1" H 8975 3815 50  0000 C CNN
F 1 "esp32-wroom-32u" H 8975 3724 50  0000 C CNN
F 2 "oven2:esp32-wroom-32u" H 8750 3050 50  0001 C CNN
F 3 "" H 8750 3050 50  0001 C CNN
	1    8950 2600
	1    0    0    -1  
$EndComp
Text Label 7450 2050 0    50   ~ 0
EN
$Comp
L Device:Rotary_Encoder_Switch SW1
U 1 1 5DF20726
P 4450 3450
F 0 "SW1" H 4450 3817 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 4450 3726 50  0000 C CNN
F 2 "oven2:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 4300 3610 50  0001 C CNN
F 3 "~" H 4450 3710 50  0001 C CNN
	1    4450 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW2
U 1 1 5DF20857
P 4450 4100
F 0 "SW2" H 4450 4467 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 4450 4376 50  0000 C CNN
F 2 "oven2:RotaryEncoder_Alps_EC11E-Switch_Vertical_H20mm" H 4300 4260 50  0001 C CNN
F 3 "~" H 4450 4360 50  0001 C CNN
	1    4450 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 3450 4150 3450
Wire Wire Line
	3950 4100 3950 3750
Wire Wire Line
	3600 3350 4150 3350
Wire Wire Line
	3600 3550 4150 3550
Wire Wire Line
	3600 4000 4150 4000
Wire Wire Line
	3600 4200 4150 4200
Text Label 3700 3550 0    50   ~ 0
e1_a
Text Label 3700 3350 0    50   ~ 0
e1_b
Text Label 3700 4200 0    50   ~ 0
e2_a
Text Label 3700 4000 0    50   ~ 0
e2_b
Wire Wire Line
	4150 4100 3950 4100
Wire Wire Line
	4900 4000 4750 4000
Wire Wire Line
	4750 3550 4900 3550
Wire Wire Line
	4900 3550 4900 3750
Wire Wire Line
	5300 4200 4750 4200
Wire Wire Line
	5300 3350 4750 3350
Text Label 5100 3350 0    50   ~ 0
IO0
Text Label 5100 4200 0    50   ~ 0
IO2
NoConn ~ 8750 3600
NoConn ~ 8850 3600
NoConn ~ 8950 3600
NoConn ~ 9050 3600
NoConn ~ 9150 3600
NoConn ~ 9250 3600
Text Label 4200 5800 0    50   ~ 0
EN
Wire Wire Line
	4400 5800 4050 5800
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5DFEBD14
P 4700 6900
F 0 "J1" H 4700 6700 50  0000 C CNN
F 1 "mode" H 4700 7150 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 4700 6900 50  0001 C CNN
F 3 "~" H 4700 6900 50  0001 C CNN
	1    4700 6900
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR02
U 1 1 5DFEE6E7
P 4200 6700
F 0 "#PWR02" H 4200 6550 50  0001 C CNN
F 1 "VDD" H 4217 6873 50  0000 C CNN
F 2 "" H 4200 6700 50  0001 C CNN
F 3 "" H 4200 6700 50  0001 C CNN
	1    4200 6700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5DFEE7DB
P 4200 7150
F 0 "#PWR03" H 4200 6900 50  0001 C CNN
F 1 "GND" H 4205 6977 50  0000 C CNN
F 2 "" H 4200 7150 50  0001 C CNN
F 3 "" H 4200 7150 50  0001 C CNN
	1    4200 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 7150 4200 7000
Wire Wire Line
	4200 7000 4500 7000
Wire Wire Line
	4500 6800 4200 6800
Wire Wire Line
	4200 6800 4200 6700
Wire Wire Line
	4500 6900 3900 6900
Text Label 4150 6900 2    50   ~ 0
mode
$Comp
L Connector_Generic:Conn_01x04 J8
U 1 1 5E01079E
P 9850 5400
F 0 "J8" H 9930 5392 50  0000 L CNN
F 1 "output" H 9930 5301 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 9850 5400 50  0001 C CNN
F 3 "~" H 9850 5400 50  0001 C CNN
	1    9850 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 5300 9050 5300
Wire Wire Line
	9050 5400 9650 5400
Wire Wire Line
	9050 5500 9650 5500
Wire Wire Line
	9050 5600 9650 5600
Text Label 9200 5300 0    50   ~ 0
out1
Text Label 9200 5400 0    50   ~ 0
out2
Text Label 9200 5500 0    50   ~ 0
out3
Text Label 9200 5600 0    50   ~ 0
out4
$Comp
L Device:R R?
U 1 1 5DE06DD5
P 1600 5700
AR Path="/5DDE5893/5DE06DD5" Ref="R?"  Part="1" 
AR Path="/5DE06DD5" Ref="R11"  Part="1" 
F 0 "R11" H 1670 5746 50  0000 L CNN
F 1 "68r" H 1670 5655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 1530 5700 50  0001 C CNN
F 3 "~" H 1600 5700 50  0001 C CNN
	1    1600 5700
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 5DE06DDC
P 1600 6150
AR Path="/5DDE5893/5DE06DDC" Ref="D?"  Part="1" 
AR Path="/5DE06DDC" Ref="D2"  Part="1" 
F 0 "D2" V 1545 6228 50  0000 L CNN
F 1 "LED" V 1636 6228 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm" H 1600 6150 50  0001 C CNN
F 3 "~" H 1600 6150 50  0001 C CNN
	1    1600 6150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 5850 1600 6000
Wire Wire Line
	1600 6300 1600 6600
Wire Wire Line
	1600 5100 1600 5550
Text Label 1600 5250 0    50   ~ 0
led1
$Comp
L Device:R R?
U 1 1 5DE27EF8
P 2350 5700
AR Path="/5DDE5893/5DE27EF8" Ref="R?"  Part="1" 
AR Path="/5DE27EF8" Ref="R12"  Part="1" 
F 0 "R12" H 2420 5746 50  0000 L CNN
F 1 "68r" H 2420 5655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 2280 5700 50  0001 C CNN
F 3 "~" H 2350 5700 50  0001 C CNN
	1    2350 5700
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D?
U 1 1 5DE27EFF
P 2350 6150
AR Path="/5DDE5893/5DE27EFF" Ref="D?"  Part="1" 
AR Path="/5DE27EFF" Ref="D3"  Part="1" 
F 0 "D3" V 2295 6228 50  0000 L CNN
F 1 "LED" V 2386 6228 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm" H 2350 6150 50  0001 C CNN
F 3 "~" H 2350 6150 50  0001 C CNN
	1    2350 6150
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5DE27F06
P 2350 6600
AR Path="/5DDE5893/5DE27F06" Ref="#PWR?"  Part="1" 
AR Path="/5DE27F06" Ref="#PWR0101"  Part="1" 
F 0 "#PWR0101" H 2350 6350 50  0001 C CNN
F 1 "GND" H 2355 6427 50  0000 C CNN
F 2 "" H 2350 6600 50  0001 C CNN
F 3 "" H 2350 6600 50  0001 C CNN
	1    2350 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 5850 2350 6000
Wire Wire Line
	2350 6300 2350 6600
Wire Wire Line
	2350 5100 2350 5550
Text Label 2350 5250 0    50   ~ 0
led2
$Comp
L power:GND #PWR?
U 1 1 5DE06DE3
P 1600 6600
AR Path="/5DDE5893/5DE06DE3" Ref="#PWR?"  Part="1" 
AR Path="/5DE06DE3" Ref="#PWR0102"  Part="1" 
F 0 "#PWR0102" H 1600 6350 50  0001 C CNN
F 1 "GND" H 1605 6427 50  0000 C CNN
F 2 "" H 1600 6600 50  0001 C CNN
F 3 "" H 1600 6600 50  0001 C CNN
	1    1600 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 2150 8050 2150
Wire Wire Line
	7350 2250 8050 2250
Text Label 7450 2150 0    50   ~ 0
led1
Text Label 7450 2250 0    50   ~ 0
led2
Wire Wire Line
	9450 3600 9450 3950
Text Label 9450 3850 1    50   ~ 0
IO2
Wire Wire Line
	3950 3750 4900 3750
Connection ~ 3950 3750
Wire Wire Line
	3950 3750 3950 3450
Connection ~ 4900 3750
NoConn ~ 8050 3050
NoConn ~ 8050 3150
NoConn ~ 9350 3600
Wire Wire Line
	4900 3750 4900 4000
Wire Wire Line
	4900 3750 5300 3750
Text Label 5050 3750 0    50   ~ 0
GND
$Comp
L Transistor_BJT:S8050 Q1
U 1 1 5DE3DFA7
P 6200 6050
F 0 "Q1" H 6391 6096 50  0000 L CNN
F 1 "S8050" H 6391 6005 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6400 5975 50  0001 L CIN
F 3 "http://www.unisonic.com.tw/datasheet/S8050.pdf" H 6200 6050 50  0001 L CNN
	1    6200 6050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5DE3E0BA
P 6300 6600
F 0 "#PWR04" H 6300 6350 50  0001 C CNN
F 1 "GND" H 6305 6427 50  0000 C CNN
F 2 "" H 6300 6600 50  0001 C CNN
F 3 "" H 6300 6600 50  0001 C CNN
	1    6300 6600
	1    0    0    -1  
$EndComp
$Comp
L Device:Buzzer BZ1
U 1 1 5DE3E23E
P 6400 5500
F 0 "BZ1" H 6553 5529 50  0000 L CNN
F 1 "Buzzer" H 6553 5438 50  0000 L CNN
F 2 "Buzzer_Beeper:Buzzer_12x9.5RM7.6" V 6375 5600 50  0001 C CNN
F 3 "~" V 6375 5600 50  0001 C CNN
	1    6400 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 3600 8650 3950
Text Label 8650 3900 1    50   ~ 0
buzzer
Wire Wire Line
	5500 6050 6000 6050
Text Label 5600 6050 0    50   ~ 0
buzzer
Wire Wire Line
	6300 6600 6300 6250
Wire Wire Line
	6300 5050 6300 5400
Wire Wire Line
	6300 5600 6300 5850
$Comp
L power:VDD #PWR01
U 1 1 5DE4E7D7
P 6300 5050
F 0 "#PWR01" H 6300 4900 50  0001 C CNN
F 1 "VDD" H 6317 5223 50  0000 C CNN
F 2 "" H 6300 5050 50  0001 C CNN
F 3 "" H 6300 5050 50  0001 C CNN
	1    6300 5050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J7
U 1 1 5DE769DE
P 8000 5800
F 0 "J7" H 8080 5792 50  0000 L CNN
F 1 "Conn_01x02" H 8080 5701 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Vertical" H 8000 5800 50  0001 C CNN
F 3 "~" H 8000 5800 50  0001 C CNN
	1    8000 5800
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0103
U 1 1 5DE85213
P 7700 5650
F 0 "#PWR0103" H 7700 5500 50  0001 C CNN
F 1 "VCC" H 7717 5823 50  0000 C CNN
F 2 "" H 7700 5650 50  0001 C CNN
F 3 "" H 7700 5650 50  0001 C CNN
	1    7700 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5DE852E4
P 7700 6050
F 0 "#PWR0104" H 7700 5800 50  0001 C CNN
F 1 "GND" H 7705 5877 50  0000 C CNN
F 2 "" H 7700 6050 50  0001 C CNN
F 3 "" H 7700 6050 50  0001 C CNN
	1    7700 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 6050 7700 5900
Wire Wire Line
	7700 5900 7800 5900
Wire Wire Line
	7700 5650 7700 5800
Wire Wire Line
	7700 5800 7800 5800
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 5DE9D047
P 4600 5700
F 0 "J4" H 4680 5692 50  0000 L CNN
F 1 "Conn_01x02" H 4680 5601 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4600 5700 50  0001 C CNN
F 3 "~" H 4600 5700 50  0001 C CNN
	1    4600 5700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
