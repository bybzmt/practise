#ifndef __24L01_H
#define __24L01_H

#include "stm32f1xx_hal.h"
#include "spi.h"
#include <stdbool.h>

//24L01发送接收数据宽度定义
#define NRF24L01_ADR_WIDTH    5
//5字节的地址宽度
#define NRF24L01_PLOAD_WIDTH  32
//32字节的用户数据宽度

typedef void (*NRF24L01_OnRxMsg)(uint8_t *buf) ;
typedef void (*NRF24L01_OnTxOk)() ;
typedef void (*NRF24L01_OnTxFail)() ;

typedef struct {
    SPIx spi;

    GPIO_TypeDef *ce_port;
    uint16_t ce_pin;

    GPIO_TypeDef *irq_port;
    uint16_t irq_pin;

    NRF24L01_OnRxMsg onRxMsg;
    NRF24L01_OnTxOk onTxOk;
    NRF24L01_OnTxFail onTxFail;

    uint8_t _rx_buf[NRF24L01_PLOAD_WIDTH+1];
    uint8_t _tx_buf[NRF24L01_PLOAD_WIDTH+1];

    uint8_t *rxbuf;
    uint8_t *txbuf;

} NRF24L01;

//初始化
void NRF24L01_Init(NRF24L01 *nrf);

void NRF24L01_Irq(); //中断回调
bool NRF24L01_Check(NRF24L01 *nrf);//检查24L01是否存在
void NRF24L01_RxPacket(NRF24L01 *nrf);//配置为接收模式
void NRF24L01_TxPacket(NRF24L01 *nrf);//发送一个包的数据
#endif











