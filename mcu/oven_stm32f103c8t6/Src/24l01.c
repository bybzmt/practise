#include <string.h>
#include "24l01.h"
#include "spi.h"

//NRF24L01�Ĵ�����������
#define CMD_READ        0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define CMD_WRITE       0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP             0xFF  //�ղ���,����������״̬�Ĵ���

//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                              //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA           0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR       0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW        0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR      0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP        0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS          0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                              //bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX  	0x10  //�ﵽ����ʹ����ж�
#define TX_OK   	0x20  //TX��������ж�
#define RX_OK   	0x40  //���յ������ж�

#define OBSERVE_TX      0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD              0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0      0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1      0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2      0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3      0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4      0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5      0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR         0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0        0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define FIFO_STATUS     0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                              //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;
//////////////////////////////////////////////////////////////////////////////////////////////////////////

const uint8_t TX_ADDRESS[ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

static NRF24L01 *gnrf = NULL;

static uint8_t NRF24L01_Read_Reg(NRF24L01 *nrf, uint8_t reg); //���Ĵ���
static uint8_t NRF24L01_Write_Reg(NRF24L01 *nrf, uint8_t reg, uint8_t value); //д�Ĵ���
static inline void NRF24L01_Xfer(NRF24L01 *nrf, uint8_t reg, uint8_t len); //��������
static void NRF24L01_Write_Buf(NRF24L01 *nrf, uint8_t reg, uint8_t *buf, uint8_t len); //д������

//��ʼ��24L01��IO��
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

    //ȷ��CE�͵�ƽ
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    return;

    //�����ж�
    HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    //�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
    NRF24L01_Write_Reg(nrf, CMD_WRITE+SETUP_RETR,0x1a);
    //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RF_CH,40);
    //����TX�������,0db����,2Mbps,���������濪��
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RF_SETUP,0x0f);
    //���õ�ַ����
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+SETUP_AW, 3);
    //ѡ��ͨ��0����Ч���ݿ��
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+RX_PW_P0, PLOAD_WIDTH);
    //�ر��Զ�Ӧ��
    NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x00);
    //ʹ��ͨ��0�Ľ��յ�ַ
    NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_RXADDR,0x01);
    //дTX�ڵ��ַ
  	NRF24L01_Write_Buf(nrf, CMD_WRITE+TX_ADDR,(uint8_t*)TX_ADDRESS, ADR_WIDTH);
    //����RX�ڵ��ַ,��ҪΪ��ʹ��ACK
    NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS, ADR_WIDTH);
}

//���24L01�Ƿ����
bool NRF24L01_Check(NRF24L01 *nrf)
{
    memset(nrf->rxbuf, 0xa5, 5);

    //д��5���ֽڵĵ�ַ.
	NRF24L01_Xfer(nrf, CMD_WRITE+TX_ADDR, 5);
    //����д��ĵ�ַ
	NRF24L01_Xfer(nrf, TX_ADDR, 5);

    return memcmp(nrf->rxbuf, nrf->txbuf, 5) == 0 ? true : false;
}

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t NRF24L01_Write_Reg(NRF24L01 *nrf, uint8_t reg, uint8_t value)
{
    uint8_t tx[2]={reg, value}, rx[2];

    SPI_TransmitReceive(&nrf->spi, tx, rx, sizeof(tx));

    return rx[0];
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t NRF24L01_Read_Reg(NRF24L01 *nrf, uint8_t reg)
{
    uint8_t tx[2]={reg, 0xff}, rx[2] = {0, 0};

    SPI_TransmitReceive(&nrf->spi, tx, rx, sizeof(tx));

    return rx[1];
}

//����ָ���ȵ�����
//reg:�Ĵ���(λ��)
//len:���ݳ���
inline void NRF24L01_Xfer(NRF24L01 *nrf, uint8_t reg, uint8_t len)
{
    nrf->_tx_buf[0] = reg;
    SPI_TransmitReceive(&nrf->spi, nrf->_tx_buf, nrf->_rx_buf, len+1);
}

//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
void NRF24L01_Write_Buf(NRF24L01 *nrf, uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    memcpy(nrf->txbuf, pBuf, len);
    NRF24L01_Xfer(nrf, reg, len);
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
void NRF24L01_TxPacket(NRF24L01 *nrf)
{
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //дTX�ڵ��ַ
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+TX_ADDR,(uint8_t*)TX_ADDRESS, ADR_WIDTH);
    //�����Զ�Ӧ��
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x00);
    //����RX�ڵ��ַ,��ҪΪ��ʹ��ACK
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS, ADR_WIDTH);
    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+CONFIG,0x0e);
    //д���ݵ�TX BUF
  	NRF24L01_Xfer(nrf, WR_TX_PLOAD, PLOAD_WIDTH);

    //��������
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 1);
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������
void NRF24L01_RxPacket(NRF24L01 *nrf)
{
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //ʹ��ͨ��0���Զ�Ӧ��
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+EN_AA,0x01);
    //����RX�ڵ��ַ,��ҪΪ��ʹ��ACK
  	//NRF24L01_Write_Buf(nrf, CMD_WRITE+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,ADR_WIDTH);
    //���RX FIFO�Ĵ���
    NRF24L01_Write_Reg(nrf, FLUSH_RX, 0xff);
    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ
  	NRF24L01_Write_Reg(nrf, CMD_WRITE+CONFIG, 0x0f);

    //CEΪ��,�������ģʽ
    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 1);
}

void NRF24L01_Irq() {

    if (gnrf == NULL) return;

    NRF24L01 *nrf = gnrf;

    HAL_GPIO_WritePin(nrf->ce_port, nrf->ce_pin, 0);

    //��������жϣ�����ȡ״̬�Ĵ�����ֵ
	uint8_t sta = NRF24L01_Write_Reg(nrf, CMD_WRITE|STATUS, MAX_TX|RX_OK|TX_OK);

	if ( sta & MAX_TX) { //�ﵽ����ط�����
        if (nrf->onTxFail == NULL) return;

        nrf->onTxFail();
    } else if (sta & TX_OK) { //�������
        if (nrf->onTxOk == NULL) return;

        nrf->onTxOk();
    } else if (sta & RX_OK) { //���յ�����
        if (nrf->onRxMsg == NULL) return;

        //��ȡ����
        NRF24L01_Xfer(nrf, RD_RX_PLOAD, PLOAD_WIDTH);

        nrf->onRxMsg(nrf->rxbuf);
    }
}

