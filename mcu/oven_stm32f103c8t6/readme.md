#烤箱程序

这一个烤箱程序

硬件有：

1. 加热管2个(上下各一个)
2. 模式切换开关1个
3. 温控调节(可调变阻器)2个
4. 温控探头(pt100)2个
5. pt100专用ADC模块2个
6. 总开关（定时器)1个
7. 继电器4个
8. 电风扇1个
9. 烤架转动电机1个

与控制模连接的有:

1. 模式选择开关1个
2. 温度感应器2个
3. 温度调器2个
4. 继电器2个

其它的由电路直接控制

发酵模式:
* 温度30度
* 烘烤模式

烘烤模式温度由调节器控制, 最高温度为250度

引脚功能定义:
```
pa0 ADC1
pa1 ADC2
pa2 ADC3

pa3 top_sensor-CS
pa4 bottom_sensor-CS
pa5 SPI CLK
pa6 SPI MISO
pa7 SPI MOSI
pa8 wireless-CS

pa9 display-G / UART1_TX
pa10 display-F / UART1_RX
pa11 display-E / USB_DP
pa12 display-D / USB_DM

pa13 swd JTMS/SWDIO
pa14 swd JTCK/SWCLK
pa15 display-DP / swd JTDI

pb0 wireless-irq
pb1 wireless-ce

pb3 display-C / swd JTDO/TRACESWO
pb4 display-B /  swd JNTRST

pb5 display-A
pb6 display-char6
pb7 display-char5
pb8 display-char4
pb9 display-char3
pb10 display-char2
pb11 display-char1
pb12 output1
pb13 output2
pb14 output3
pb15 output4

pc13 板灯
pc14 晶振y1
pc15 晶振y1
```

程序功能:

1. 每秒1次
2. 查看当模模式
3. 查看当前设定温度
4. 根据设定温度控制继电器开关

