EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 12
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 800  975  1250 3225
U 5F047EA6
F0 "stm32" 50
F1 "stm32.sch" 50
F2 "I2S1_FS" O R 2050 1500 50 
F3 "I2C_SCL" I R 2050 1075 50 
F4 "I2C_SDA" I R 2050 1175 50 
F5 "I2S1_SCK" O R 2050 1400 50 
F6 "I2S1_SD" O R 2050 1600 50 
F7 "headphone_in" I R 2050 1850 50 
F8 "BT_state" I R 2050 2550 50 
F9 "BT_en" I R 2050 2650 50 
F10 "BT_bn" I R 2050 2450 50 
F11 "I2S1_MCK" O R 2050 1300 50 
F12 "SAI1_SCK_A" I R 2050 2075 50 
F13 "SAI1_SD_A" I R 2050 2325 50 
F14 "SAI1_FS_A" I R 2050 2200 50 
F15 "headphone_en" O R 2050 1725 50 
F16 "BT_state2" I R 2050 2755 50 
$EndSheet
$Sheet
S 690  6485 1250 1000
U 5F16021E
F0 "power" 50
F1 "power.sch" 50
$EndSheet
Wire Wire Line
	2050 1075 2825 1075
Wire Wire Line
	2050 1175 2825 1175
Wire Wire Line
	2050 2075 2825 2075
Wire Wire Line
	2050 2200 2825 2200
Wire Wire Line
	2050 2325 2825 2325
Wire Wire Line
	2050 1400 2825 1400
Wire Wire Line
	2050 1500 2825 1500
Wire Wire Line
	2050 1600 2825 1600
Text Label 2325 1075 0    50   ~ 0
SCL
Text Label 2325 1175 0    50   ~ 0
SDA
Text Label 2325 2200 0    50   ~ 0
SAI1_A_FS
Text Label 2325 2325 0    50   ~ 0
SAI1_A_SD
Wire Wire Line
	2050 1850 2825 1850
Wire Wire Line
	2050 1725 2825 1725
Text Label 2200 1850 0    50   ~ 0
headphone_in
$Sheet
S 9475 2950 950  1325
U 5FD6457F
F0 "Bluetooth" 50
F1 "Bluetooth.sch" 50
F2 "CLK" I L 9475 3275 50 
F3 "FS" I L 9475 3375 50 
F4 "SD" I L 9475 3475 50 
F5 "EN" I L 9475 3600 50 
F6 "button" I L 9475 3700 50 
F7 "state" O L 9475 3800 50 
F8 "state2" O L 9475 3895 50 
$EndSheet
Text Label 2325 1400 0    50   ~ 0
I2S1_SCK
Text Label 2325 1500 0    50   ~ 0
I2S1_FS
Text Label 2325 1600 0    50   ~ 0
I2S1_SD
Text Label 7060 1250 0    50   ~ 0
I2S1_SCK
Text Label 7060 1350 0    50   ~ 0
I2S1_FS
Text Label 7060 1450 0    50   ~ 0
I2S1_SD
Text Label 8850 3600 0    50   ~ 0
BT_en
Text Label 8850 3700 0    50   ~ 0
BT_bn
Text Label 8845 3895 0    50   ~ 0
BT_state2
Wire Wire Line
	2050 2450 2825 2450
Wire Wire Line
	2050 2550 2825 2550
Wire Wire Line
	2050 2650 2825 2650
Text Label 2200 2650 0    50   ~ 0
BT_en
Text Label 2200 2450 0    50   ~ 0
BT_bn
Text Label 2200 2550 0    50   ~ 0
BT_state
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 6042C575
P 9315 6210
F 0 "H1" H 9445 6275 50  0000 L CNN
F 1 "m3" H 9445 6175 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad" H 9315 6210 50  0001 C CNN
F 3 "~" H 9315 6210 50  0001 C CNN
	1    9315 6210
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 6042C5CD
P 9705 6210
F 0 "H2" H 9845 6275 50  0000 L CNN
F 1 "m3" H 9845 6175 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad" H 9705 6210 50  0001 C CNN
F 3 "~" H 9705 6210 50  0001 C CNN
	1    9705 6210
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 6042C62A
P 10100 6205
F 0 "H3" H 10225 6285 50  0000 L CNN
F 1 "m3" H 10225 6185 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad" H 10100 6205 50  0001 C CNN
F 3 "~" H 10100 6205 50  0001 C CNN
	1    10100 6205
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 6042C676
P 10485 6210
F 0 "H4" H 10620 6275 50  0000 L CNN
F 1 "m3" H 10620 6175 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad" H 10485 6210 50  0001 C CNN
F 3 "~" H 10485 6210 50  0001 C CNN
	1    10485 6210
	1    0    0    -1  
$EndComp
NoConn ~ 9315 6310
NoConn ~ 9705 6310
NoConn ~ 10100 6305
NoConn ~ 10485 6310
$Sheet
S 9600 700  800  1075
U 6079F77B
F0 "TPA6120A2" 50
F1 "TPA6120A2.sch" 50
F2 "JackIN" I R 10400 1650 50 
F3 "LIN+" I L 9600 925 50 
F4 "LIN-" I L 9600 1050 50 
F5 "RIN+" I L 9600 1175 50 
F6 "RIN-" I L 9600 1300 50 
$EndSheet
$Sheet
S 4350 6525 1300 975 
U 6074FC63
F0 "headphone_power" 50
F1 "headphone_power.sch" 50
$EndSheet
$Sheet
S 7675 700  850  1100
U 6071C2C4
F0 "PCM1792" 50
F1 "PCM1792.sch" 50
F2 "SCL" I L 7675 875 50 
F3 "SDA" I L 7675 975 50 
F4 "SCK" I L 7675 1150 50 
F5 "BCK" I L 7675 1250 50 
F6 "LRCK" I L 7675 1350 50 
F7 "DATA" I L 7675 1450 50 
F8 "OUTLP" O R 8525 1175 50 
F9 "OUTLN" O R 8525 1300 50 
F10 "OUTRP" O R 8525 925 50 
F11 "OUTRN" O R 8525 1050 50 
$EndSheet
$Sheet
S 8650 700  800  1100
U 607205FC
F0 "OPA4134" 50
F1 "OPA4134.sch" 50
F2 "INA" I L 8650 925 50 
F3 "OUTA" O R 9450 925 50 
F4 "INC" I L 8650 1175 50 
F5 "OUTC" O R 9450 1175 50 
F6 "INB" I L 8650 1050 50 
F7 "OUTB" O R 9450 1050 50 
F8 "IND" I L 8650 1300 50 
F9 "OUTD" O R 9450 1300 50 
$EndSheet
Wire Wire Line
	8525 925  8650 925 
Wire Wire Line
	8525 1050 8650 1050
Wire Wire Line
	8525 1175 8650 1175
Wire Wire Line
	8525 1300 8650 1300
Wire Wire Line
	9450 925  9600 925 
Wire Wire Line
	9450 1050 9600 1050
Wire Wire Line
	9450 1175 9600 1175
Wire Wire Line
	9450 1300 9600 1300
Wire Wire Line
	10400 1650 10875 1650
Text Label 7060 975  0    50   ~ 0
SDA
Text Label 7060 875  0    50   ~ 0
SCL
Wire Wire Line
	6910 875  7675 875 
Wire Wire Line
	6910 975  7675 975 
Text Label 8860 3275 0    50   ~ 0
SAI1_A_SCK
Text Label 8860 3375 0    50   ~ 0
SAI1_A_FS
Wire Wire Line
	8710 3375 9475 3375
Wire Wire Line
	8710 3275 9475 3275
Wire Wire Line
	6910 1150 7675 1150
Text Label 8860 3475 0    50   ~ 0
SAI1_A_SD
Wire Wire Line
	8710 3475 9475 3475
Wire Wire Line
	2050 1300 2825 1300
Text Label 2325 1300 0    50   ~ 0
I2S1_MCK
Wire Wire Line
	8710 3600 9475 3600
Wire Wire Line
	8710 3700 9475 3700
Wire Wire Line
	8710 3800 9475 3800
Wire Wire Line
	6900 1250 7675 1250
Wire Wire Line
	6900 1350 7675 1350
Wire Wire Line
	6900 1450 7675 1450
Text Label 7050 1150 0    50   ~ 0
I2S1_MCK
Text Label 2325 2075 0    50   ~ 0
SAI1_A_SCK
Text Label 2200 1725 0    50   ~ 0
dac_power_en
Text Label 10425 1650 0    50   ~ 0
headphone_in
Wire Wire Line
	9475 3895 8710 3895
Text Label 8845 3800 0    50   ~ 0
BT_state
Wire Wire Line
	2050 2755 2825 2755
Text Label 2195 2755 0    50   ~ 0
BT_state2
$Sheet
S 7600 5050 1000 950 
U 60973CA0
F0 "power7v" 50
F1 "power7v.sch" 50
F2 "EN" I L 7600 5325 50 
$EndSheet
$Sheet
S 2725 6500 1125 950 
U 6099BEE5
F0 "power±15v" 50
F1 "power±15v.sch" 50
$EndSheet
Wire Wire Line
	6835 5325 7600 5325
Text Label 6950 5325 0    50   ~ 0
dac_power_en
$EndSCHEMATC
