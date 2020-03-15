#include "driver/spi_master.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "base.h"
#include "max31865.h"
#include "max7219.h"
#include "peripherals.h"
#include "esp_log.h"

static const char *TAG = "peripherals";

static MAX31865 rtd1;
static MAX31865 rtd2;

max7219 nixietube;

bool peripheral_event(Events *cb, event_id id)
{
    static bool re1down = false;
    static bool re2down = false;
    static bool re1a = false;
    static bool re1b = false;
    static bool re2a = false;
    static bool re2b = false;

    switch(id) {
        case GPIO_NUM_0:
            re1down = !re1down;
            if (re1down) {
                if (cb->sw1.down != NULL) {
                    cb->sw1.down();
                }
            } else {
                if (cb->sw1.up != NULL) {
                    cb->sw1.up();
                }
            }
            return true;
        case GPIO_NUM_2:
            re2down = !re2down;
            if (re2down) {
                if (cb->sw2.down != NULL) {
                    cb->sw2.down();
                }
            } else {
                if (cb->sw2.up != NULL) {
                    cb->sw2.up();
                }
            }
            return true;
        case GPIO_NUM_16:
            re2a = !re2a;
            if (re2a) {
                if (re2b) {
                    cb->sw1.increase();
                } else {
                    cb->sw1.decrease();
                }
            }
            return true;
        case GPIO_NUM_17:
            re2b = !re2b;
            return true;
        case GPIO_NUM_18:
            re1a = !re1a;
            if (re1a) {
                if (re1b) {
                    cb->sw2.increase();
                } else {
                    cb->sw2.decrease();
                }
            }
            return true;
        case GPIO_NUM_19:
            re1b = !re1b;
            return true;
    }

    return false;
}

static void IRAM_ATTR encoder_isr_handler(void* arg)
{
    xQueueSendFromISR(event_queue, arg, NULL);
}

void gpio_init()
{
    esp_err_t r;

    gpio_config_t io_conf;

    //开关1，开关2 输入|下拉|上下延边中断
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_0) | (1ULL<<GPIO_NUM_2);
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    //编码器1，2 输入|下拉|上下延边中断
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_18) |
        (1ULL<<GPIO_NUM_19) |
        (1ULL<<GPIO_NUM_16) |
        (1ULL<<GPIO_NUM_17);
    gpio_config(&io_conf);

    //led 输出|下拉|无中断
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    /* io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_36) | (1ULL<<GPIO_NUM_39); */
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_15) | (1ULL<<GPIO_NUM_5);
    gpio_config(&io_conf);

    //蜂鸣器 输出|下拉|无中断
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_13);
    gpio_config(&io_conf);

    //继电器1,2,3,4 输出|下拉|无中断
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_33) |
        (1ULL<<GPIO_NUM_25) |
        (1ULL<<GPIO_NUM_26) |
        (1ULL<<GPIO_NUM_27);
    gpio_config(&io_conf);

    //注册事件中断
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, encoder_isr_handler, (void*)GPIO_NUM_0);
    gpio_isr_handler_add(GPIO_NUM_2, encoder_isr_handler, (void*)GPIO_NUM_2);
    gpio_isr_handler_add(GPIO_NUM_16, encoder_isr_handler, (void*)GPIO_NUM_16);
    gpio_isr_handler_add(GPIO_NUM_17, encoder_isr_handler, (void*)GPIO_NUM_17);
    gpio_isr_handler_add(GPIO_NUM_18, encoder_isr_handler, (void*)GPIO_NUM_18);
    gpio_isr_handler_add(GPIO_NUM_19, encoder_isr_handler, (void*)GPIO_NUM_19);

    /* r = adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_0db ); */
    //初始化mode读取adc
    r = adc1_config_width(ADC_WIDTH_BIT_12);
    assert( r == ESP_OK );

    r = adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db);
    assert( r == ESP_OK );
}

void spi_init()
{
    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num=GPIO_NUM_22,
        .mosi_io_num=GPIO_NUM_21,
        .sclk_io_num=GPIO_NUM_23,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=0
    };

    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(SPI2_HOST, &buscfg, 2);
    ESP_ERROR_CHECK(ret);

    //上测温
    devcfg.spics_io_num=GPIO_NUM_12,               //CS pin
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &rtd1.spi);
    ESP_ERROR_CHECK(ret);

    //下测温
    devcfg.spics_io_num = GPIO_NUM_14;
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &rtd2.spi);
    ESP_ERROR_CHECK(ret);

    //数码管
    devcfg.spics_io_num = GPIO_NUM_32;
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &nixietube.spi);
    ESP_ERROR_CHECK(ret);
}

void peripherals_init()
{
    gpio_init();
    spi_init();
    max7219_init(&nixietube);
}

//通过adc读取模式
uint8_t mode_read()
{
    int raw = adc1_get_raw(ADC1_CHANNEL_0);

    if ( raw == -1 ) {
        ESP_LOGI(TAG, "mode adc error");
        return 0;
    }

    uint8_t now_mode = 0;

    if (raw > 2700) {
        now_mode = MODE_IDLE;
    } else if (raw > 2400) {
        now_mode = MODE_BAKE;
    } else if (raw > 2300) {
        now_mode = MODE_BAKE|MODE_ROTATE;
    } else if (raw > 2200) {
        now_mode = MODE_FERMENT;
    } else if (raw > 2100) {
        now_mode = MODE_BAKE|MODE_FAN;
    } else if (raw > 1900) {
        now_mode = MODE_BAKE|MODE_FAN|MODE_ROTATE;
    }

    ESP_LOGI(TAG, "row:%d mode:%d", raw, now_mode);

    return now_mode;
}


//读取温度
bool read_temp()
{
    float temp1, temp2;

    temp1 = MAX31865_Read_1shot(&rtd1);
    temp2 = MAX31865_Read_1shot(&rtd2);

    if (temp1 != temp1) {
        ESP_LOGI(TAG, "top max31865 error");
        return false;
    } else {
        now_temp_top = temp1;
    }

    if (temp2 != temp2) {
        ESP_LOGI(TAG, "bottom max31865 error");
        return false;
    } else {
        now_temp_bottom = temp1;
    }

    return true;
}

//控制输出
void relay_ctrl()
{
    float low1, low2, high1, high2;

    if (sw_bake) {
        low1 = (float)temp_top - 0.5;
        low2 = (float)temp_bottom - 0.5;
        high1 = low1 + 1;
        high2 = low2 + 1;

        if (now_temp_top < low1) {
            gpio_set_level(GPIO_NUM_33, 1);
        }
        if (now_temp_top > high1) {
            gpio_set_level(GPIO_NUM_33, 0);
        }
        if (now_temp_bottom < low2) {
            gpio_set_level(GPIO_NUM_25, 1);
        }
        if (now_temp_bottom > high2) {
            gpio_set_level(GPIO_NUM_25, 0);
        }
    } else {
        gpio_set_level(GPIO_NUM_33, 0);
        gpio_set_level(GPIO_NUM_25, 0);
    }

    gpio_set_level(GPIO_NUM_26, sw_fan);
    gpio_set_level(GPIO_NUM_27, sw_rotate);
}
