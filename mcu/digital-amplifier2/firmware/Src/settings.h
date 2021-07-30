#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
 extern "C" {
#endif

#define VOLUME_0dB 0xCF

/* usb供电时最大音量 */
#define VOLUME_ON_USB_POWER (0xCF-50)

 /*
  * Volume: 0.5dB/step
  * 0xFF: +24 dB
  * 0xCF: 0 dB
  * 0x01: –103 dB
  * 0x00: MUTE
*/
typedef uint8_t volume_t;

/* 用户设置 */
typedef struct {
    uint32_t _fnv1a;

    /* 输入源 */
    uint8_t input_mode;
    /* 音量 */
    volume_t vol;
    /* 静音 */
    bool mute;
    /* 耳机是否打开  */
    bool headphone_on;
    /* 音箱是否打开  */
    bool speakers_on;
    /* 耳机音箱自动切换 */
    bool auto_switch;
    /* 无数据时是否自动关闭 */
    bool auto_off;
} settings_t;

extern settings_t settings;

void audio_setVolume(volume_t vol);
void audio_setMute(bool flag);

void settings_fnv_set(void);
void settings_fnv_check(void);
void settings_init(void);

#ifdef __cplusplus
}
#endif

#endif
