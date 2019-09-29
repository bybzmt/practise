#include <string.h>
#include "24l01.h"
#include "spi.h"

//NRF24L01寄存器操作命令
#define CMD_READ        0x00  //读配置寄存器,低5位为寄存器地址
#define CMD_WRITE       0x20  //写配置寄存器,低5位为寄存器地址
#define RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NOP             0xFF  //空操作,可以用来读状态寄存器

//SPI(NRF24L01)寄存器地址
#define CONFIG          0x00  //配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
                              //bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define EN_AA           0x01  //使能自动应答功能  bit0~5,对应通道0~5
#define EN_RXADDR       0x02  //接收地址允许,bit0~5,对应通道0~5
#define SETUP_AW        0x03  //设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define SETUP_RETR      0x04  //建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define RF_CH           0x05  //RF通道,bit6:0,工作通道频率;
#define RF_SETUP        0x06  //RF寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define STATUS          0x07  //状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                              //bit5:数据发送完成中断;bit6:接收数据中断;
#define MAX_TX  	0x10  //达到最大发送次数中断
#define TX_OK   	0x20  //TX发送完成中断
#define RX_OK   	0x40  //接收到数据中断

#define OBSERVE_TX      0x08  //发送检测寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define CD              0x09  //载波检测寄存器,bit0,载波检测;
#define RX_ADDR_P0      0x0A  //数据通道0接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P1      0x0B  //数据通道1接收地址,最大长度5个字节,低字节在前
#define RX_ADDR_P2      0x0C  //数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P3      0x0D  //数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P4      0x0E  //数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define RX_ADDR_P5      0x0F  //数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define TX_ADDR         0x10  //发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define RX_PW_P0        0x11  //接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P1        0x12  //接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P2        0x13  //接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P3        0x14  //接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P4        0x15  //接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define RX_PW_P5        0x16  //接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define FIFO_STATUS     0x17  //FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
                              //bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
//////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t TX_ADDRESS[ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址
const uint8_t RX_ADDRESS[ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //发送地址

static NRF24L01 *gnrf = NULL;

static uint8_t NRF24L01_Read_Reg(NRF24L01 *nrf, uint8_t reg); //读寄存器
static uint8_t NRF24L01_Write_Reg(NRF24L01 *nrf, uint8_t reg, uint8_t value); //写寄存器
static inline void NRF24L01_Xfer(NRF24L01 *nrf, uint8_t reg, uint8_t len); //传输数据
static void NRF24L01_Write_Buf(NRF24L01 *nrf, uint8_t reg, uint8_t *buf, uint8_t len); //写数据区

//初始化24L01的IO口
void NRF24L01_Init(NRF24L01 *nrf)
{
    nrf->ce_port = GPIOB;
    nrf->ce_pin = GPIO_PIN_1;
    nrf->irq_port = GPIOB;
    nrf->irq_pin = GPIO_PIN_0;

    nrf->rxbuf = nrf->_rx_buf + 1;
    nrf->txbuf = nrf->_tx_buf + 1;
    gnrf = nrf;

    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //确保CE低电平
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    return;

    //启用中断
    HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    //设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
    NRF24L01_Write_Reg(nrf, CMD_WRITE+SETUP_RETR,0x1a);
    //设置RF通道为40
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RF_CH,40);
    //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RF_SETUP,0x0f);
    //设置地址长度
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+SETUP_AW, 3);
    //选择通道0的有效数据宽度
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RX_PW_P0, PLOAD_WIDTH);
    //关闭自动应答
    NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x00);
    //使能通道0的接收地址
    NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_RXADDR,0x01);
    //写TX节点地址
  	NRF24L01_Write_Buf(nrf, CMD_WRITE+TX_ADDR,(uint8_t*)TX_ADDRESS, ADR_WIDTH);
    //设置RX节点地址,主要为了使能ACK
    NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS, ADR_WIDTH);
}

//检测24L01是否存在
bool NRF24L01_Check(NRF24L01 *nrf)
{
    memset(nrf->rxbuf, 0xa5, 5);

    //写入5个字节的地址.
	NRF24L01_Xfer(nrf, CMD_WRITE+TX_ADDR, 5);
    //读出写入的地址
	NRF24L01_Xfer(nrf, TX_ADDR, 5);

    return memcmp(nrf->rxbuf, nrf->txbuf, 5) == 0 ? true : false;
}

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
uint8_t NRF24L01_Write_Reg(NRF24L01 *nrf, uint8_t reg, uint8_t value)
{
    uint8_t tx[2]={reg, value}, rx[2];

    SPI_TransmitReceive(&nrf->spi, tx, rx, sizeof(tx));

    return rx[0];
}
//读取SPI寄存器值
//reg:要读的寄存器
uint8_t NRF24L01_Read_Reg(NRF24L01 *nrf, uint8_t reg)
{
    uint8_t tx[2]={reg, 0xff}, rx[2] = {0, 0};

    SPI_TransmitReceive(&nrf->spi, tx, rx, sizeof(tx));

    return rx[1];
}

//传输指长度的数据
//reg:寄存器(位置)
//len:数据长度
inline void NRF24L01_Xfer(NRF24L01 *nrf, uint8_t reg, uint8_t len)
{
    nrf->_tx_buf[0] = reg;
    SPI_TransmitReceive(&nrf->spi, nrf->_tx_buf, nrf->_rx_buf, len+1);
}

//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
void NRF24L01_Write_Buf(NRF24L01 *nrf, uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    memcpy(nrf->txbuf, pBuf, len);
    NRF24L01_Xfer(nrf, reg, len);
}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
void NRF24L01_TxPacket(NRF24L01 *nrf)
{
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //写TX节点地址
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+TX_ADDR,(uint8_t*)TX_ADDRESS, ADR_WIDTH);
    //禁用自动应答
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x00);
    //设置RX节点地址,主要为了使能ACK
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS, ADR_WIDTH);
    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+CONFIG,0x0e);
    //写数据到TX BUF
  	NRF24L01_Xfer(nrf, WR_TX_PLOAD, PLOAD_WIDTH);

    //启动发送
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 1);
}

//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了
void NRF24L01_RxPacket(NRF24L01 *nrf)
{
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //使能通道0的自动应答
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x01);
    //设置RX节点地址,主要为了使能ACK
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,ADR_WIDTH);
    //清除RX FIFO寄存器
    NRF24L01_Write_Reg(nrf, FLUSH_RX, 0xff);
    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+CONFIG, 0x0f);

    //CE为高,进入接收模式
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 1);
}

void NRF24L01_Irq() {

    if (gnrf == NULL) return;

    NRF24L01 *nrf = gnrf;

    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //清除所有中断，并读取状态寄存器的值
	uint8_t sta = NRF24L01_Write_Reg(nrf, CMD_WRITE|STATUS, MAX_TX|RX_OK|TX_OK);

	if ( sta & MAX_TX) { //达到最大重发次数
        if (nrf->onTxFail == NULL) return;

        nrf->onTxFail();
    } else if (sta & TX_OK) { //发送完成
        if (nrf->onTxOk == NULL) return;

        nrf->onTxOk();
    } else if (sta & RX_OK) { //接收到数据
        if (nrf->onRxMsg == NULL) return;

        //读取数据
        NRF24L01_Xfer(nrf, RD_RX_PLOAD, PLOAD_WIDTH);

        nrf->onRxMsg(nrf->rxbuf);
    }
}

