#include <string.h>
#include "max7219.h"
#include "esp_log.h"

static const char *TAG = "max7219";

//译码表
//0-9,-,EHLP,blank
static uint8_t display_bits[16] = {
    //0
    0B11111010U,
    //1
    0B00011000U,
    //2
    0B01101110U,
    //3
    0B01111100U,
    //4
    0B10011100U,
    //5
    0B11110100U,
    //6
    0B11110110U,
    //7
    0B01011000U,
    //8
    0B11111110U,
    //9
    0B11111100U,
    //-
    0B00000100U,
    //E
    0B11100110U,
    //H
    0B10011110U,
    //L
    0B10100010U,
    //P
    0B11001110U,
    //blank
    0u
};

void max7219_set(max7219 *h, uint8_t addr, uint8_t data) {
    esp_err_t ret;
    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.length = 2;
    trans.flags=SPI_TRANS_USE_TXDATA;
    trans.tx_data[0] = addr;
    trans.tx_data[1] = data;

    ret = spi_device_transmit(h->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "spi通信失败");
    }
}

void max7219_init(max7219 *h) {
    max7219_reset(h);

    //重置所有显示
    max7219_set(h, 1, 0);
    max7219_set(h, 2, 0);
    max7219_set(h, 3, 0);
    max7219_set(h, 4, 0);
    max7219_set(h, 5, 0);
    max7219_set(h, 6, 0);
    max7219_set(h, 7, 0);
    max7219_set(h, 8, 0);
}

void max7219_reset(max7219 *h) {
    memset(&h->_display, 0xff, sizeof(h->_display));

    //关闭测试模式
    max7219_set(h, 0xF, 0);

    //译码模(&disp, 2, tmp)式
    /* max7219_set(h, 0x9, 0xff); */
    //非译码模式
    max7219_set(h, 0x9, 0);

    //字位
    if (h->limit > 0x7) {
        h->limit = 0;
    }
    max7219_set(h, 0xB, h->limit);

    //亮度
    if (h->intensity > 0xF) {
        h->intensity = 0;
    }
    max7219_set(h, 0xA, h->intensity);

    //打开电源
    max7219_set(h, 0xC, 1);
}

//测试模式
void max7219_test(max7219 *h, bool ok) {
    max7219_set(h, 0xF, ok);
}

//显示数字
//0-9,10>-,11>E,12>H,13>L,14>P,15>灭
//dp于128
void max7219_show(max7219 *h, max7219_digit offset, uint8_t data) {
    if (h->_display[offset] != data) {
        h->_display[offset] = data;

        //译码并传输
        max7219_set(h, offset, display_bits[data & 0xf] | (data>>7));
    }
}

//刷新显示
void max7219_refresh(max7219 *h) {
    for (uint8_t i=0; i<8; i++) {
        max7219_set(h, i, display_bits[h->_display[i] & 0xf] | (h->_display[i] >> 7));
    }
}
