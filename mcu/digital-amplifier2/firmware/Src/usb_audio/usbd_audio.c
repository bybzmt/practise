#include "usbd_audio.h"
#include "usbd_ctlreq.h"

#include "base.h"

// clang-format off
#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/*
 * Max packet size: (freq / 1000 + extra_samples) * channels * bytes_per_sample
 * e.g. (96000 / 1000 + 1) * 2(stereo) * 2(16bit) = 388
 */
#define AUDIO_PACKET_SZE_16B(frq) (uint8_t)(((frq / 1000U + 1) * 2U * 2U) & 0xFFU), \
    (uint8_t)((((frq / 1000U + 1) * 2U * 2U) >> 8) & 0xFFU)

#define AUDIO_PACKET_SZE_24B(frq) (uint8_t)(((frq / 1000U + 1) * 2U * 3U) & 0xFFU), \
    (uint8_t)((((frq / 1000U + 1) * 2U * 3U) >> 8) & 0xFFU)

/* Feature Unit Config */
#define AUDIO_CONTROL_FEATURES AUDIO_CONTROL_MUTE | AUDIO_CONTROL_VOL


static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef* pdev, uint8_t cfgidx);

static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef* pdev, uint8_t cfgidx);

static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);

static uint8_t* USBD_AUDIO_GetCfgDesc(uint16_t* length);

static uint8_t* USBD_AUDIO_GetDeviceQualifierDesc(uint16_t* length);

static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef* pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef* pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef* pdev);

static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef* pdev);

static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef* pdev);

static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum);

static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum);

static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);
static void AUDIO_REQ_GetMax(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);
static void AUDIO_REQ_GetMin(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);
static void AUDIO_REQ_GetRes(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);

static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req);

static void AUDIO_OUT_StopAndReset(USBD_HandleTypeDef* pdev);

static void AUDIO_OUT_Restart(USBD_HandleTypeDef* pdev);

static uint8_t VOL_PERCENT(int16_t vol);

/**
 * @}
 */

/** @defgroup USBD_AUDIO_Private_Variables
 * @{
 */

USBD_ClassTypeDef USBD_AUDIO = {
    USBD_AUDIO_Init,
    USBD_AUDIO_DeInit,
    USBD_AUDIO_Setup,
    USBD_AUDIO_EP0_TxReady,
    USBD_AUDIO_EP0_RxReady,
    USBD_AUDIO_DataIn,
    USBD_AUDIO_DataOut,
    USBD_AUDIO_SOF,
    USBD_AUDIO_IsoINIncomplete,
    USBD_AUDIO_IsoOutIncomplete,
    USBD_AUDIO_GetCfgDesc,
    USBD_AUDIO_GetCfgDesc,
    USBD_AUDIO_GetCfgDesc,
    USBD_AUDIO_GetDeviceQualifierDesc,
};

/* USB AUDIO device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_CfgDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END = {
    /* Configuration 1 */
    0x09,                              /* bLength */
    USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
    LOBYTE(USB_AUDIO_CONFIG_DESC_SIZ), /* wTotalLength 121 bytes*/
    HIBYTE(USB_AUDIO_CONFIG_DESC_SIZ),
    0x02, /* bNumInterfaces */
    0x01, /* bConfigurationValue */
    0x00, /* iConfiguration */
    0xC0, /* bmAttributes  BUS Powred*/
    0x32, /* bMaxPower = 100 mA*/
    /* 09 byte*/

    /* USB Speaker Standard interface descriptor */
    AUDIO_INTERFACE_DESC_SIZE,   /* bLength */
    USB_DESC_TYPE_INTERFACE,     /* bDescriptorType */
    0x00,                        /* bInterfaceNumber */
    0x00,                        /* bAlternateSetting */
    0x00,                        /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,      /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOCONTROL, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,    /* bInterfaceProtocol */
    0x00,                        /* iInterface */
    /* 09 byte*/

    /* USB Speaker Class-specific AC Interface Descriptor */
    AUDIO_INTERFACE_DESC_SIZE,       /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype */
    0x00, /* 1.00 */                 /* bcdADC */
    0x01,
    0x27, /* wTotalLength = 39*/
    0x00,
    0x01, /* bInCollection */
    0x01, /* baInterfaceNr */
    /* 09 byte*/

    /* USB Speaker Input Terminal Descriptor */
    AUDIO_INPUT_TERMINAL_DESC_SIZE,  /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype */
    0x01,                            /* bTerminalID */
    0x01,                            /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
    0x01,
    0x00, /* bAssocTerminal */
    0x02, /* bNrChannels */
    0x03, /* wChannelConfig 0x0003  FL FR */
    0x00,
    0x00, /* iChannelNames */
    0x00, /* iTerminal */
    /* 12 byte*/

    /* USB Speaker Audio Feature Unit Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype */
    AUDIO_OUT_STREAMING_CTRL,        /* bUnitID */
    0x01,                            /* bSourceID */
    0x01,                            /* bControlSize */
    AUDIO_CONTROL_FEATURES,          /* bmaControls(0) */
    0,                               /* bmaControls(1) */
    0x00,                            /* iTerminal */
    /* 09 byte*/

    /*USB Speaker Output Terminal Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype */
    0x03,                            /* bTerminalID */
    0x01,                            /* wTerminalType  0x0301*/
    0x03,
    0x00, /* bAssocTerminal */
    0x02, /* bSourceID */
    0x00, /* iTerminal */
    /* 09 byte*/

    /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
    /* Interface 1, Alternate Setting 0                                             */
    AUDIO_INTERFACE_DESC_SIZE,     /* bLength */
    USB_DESC_TYPE_INTERFACE,       /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 1                                           */
    AUDIO_INTERFACE_DESC_SIZE,     /* bLength */
    USB_DESC_TYPE_INTERFACE,       /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x01,                          /* bAlternateSetting */
    0x02,                          /* bNumEndpoints - 1 output & 1 feedback */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Speaker Audio Streaming Interface Descriptor */
    AUDIO_STREAMING_INTERFACE_DESC_SIZE, /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,             /* bDescriptorSubtype */
    0x01,                                /* bTerminalLink */
    0x01,                                /* bDelay */
    0x01,                                /* wFormatTag AUDIO_FORMAT_PCM  0x0001*/
    0x00,
    /* 07 byte*/

    /* USB Speaker Audio Type I Format Interface Descriptor */
    0x11,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,     /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,             /* bFormatType */
    0x02,                            /* bNrChannels */
    0x02,                            /* bSubFrameSize :  2 Bytes per frame (16bits) */
    0x10,                            /* bBitResolution (16-bits per sample) */
    0x03,                            /* bSamFreqType 3 frequencies supported */
    AUDIO_SAMPLE_FREQ(44100),        /* Audio sampling frequency coded on 3 bytes */
    AUDIO_SAMPLE_FREQ(48000),        /* Audio sampling frequency coded on 3 bytes */
    AUDIO_SAMPLE_FREQ(96000),        /* Audio sampling frequency coded on 3 bytes */
    /* 17 byte*/

    /* Endpoint 1 - Standard Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,         /* bLength */
    USB_DESC_TYPE_ENDPOINT,                    /* bDescriptorType */
    AUDIO_OUT_EP,                              /* bEndpointAddress 1 out endpoint*/
    USBD_EP_TYPE_ISOC_ASYNC,                   /* bmAttributes */
    AUDIO_PACKET_SZE_16B(USBD_AUDIO_FREQ_MAX), /* wMaxPacketSize in Bytes (freq / 1000 + extra_samples) * channels * bytes_per_sample */
    0x01,                                      /* bInterval */
    0x00,                                      /* bRefresh */
    AUDIO_IN_EP,                               /* bSynchAddress */
    /* 09 byte*/

    /* Endpoint - Audio Streaming Descriptor */
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE, /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,             /* bDescriptor */
    0x01,                               /* bmAttributes - Sampling Frequency control is supported. See UAC Spec 1.0 p.62 */
    0x00,                               /* bLockDelayUnits */
    0x00,                               /* wLockDelay */
    0x00,
    /* 07 byte*/

    /* Endpoint 2 - Standard Descriptor - See UAC Spec 1.0 p.63 4.6.2.1 Standard AS Isochronous Synch Endpoint Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE, /* bLength */
    USB_DESC_TYPE_ENDPOINT,            /* bDescriptorType */
    AUDIO_IN_EP,                       /* bEndpointAddress */
    0x11,                              /* bmAttributes */
    0x03, 0x00,                        /* wMaxPacketSize in Bytes */
    0x01,                              /* bInterval 1ms */
    SOF_RATE,                          /* bRefresh 4ms = 2^2 */
    0x00,                              /* bSynchAddress */
    /* 09 byte*/

    /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 2                                           */
    AUDIO_INTERFACE_DESC_SIZE,     /* bLength */
    USB_DESC_TYPE_INTERFACE,       /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x02,                          /* bAlternateSetting */
    0x02,                          /* bNumEndpoints - 1 output & 1 feedback */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Speaker Audio Streaming Interface Descriptor */
    AUDIO_STREAMING_INTERFACE_DESC_SIZE, /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,             /* bDescriptorSubtype */
    0x01,                                /* bTerminalLink */
    0x01,                                /* bDelay */
    0x01,                                /* wFormatTag AUDIO_FORMAT_PCM  0x0001*/
    0x00,
    /* 07 byte*/

    /* USB Speaker Audio Type I Format Interface Descriptor */
    0x11,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,     /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,             /* bFormatType */
    0x02,                            /* bNrChannels */
    0x03,                            /* bSubFrameSize :  3 Bytes per frame (24bits) */
    0x18,                            /* bBitResolution (24-bits per sample) */
    0x03,                            /* bSamFreqType 3 frequencies supported */
    AUDIO_SAMPLE_FREQ(44100),        /* Audio sampling frequency coded on 3 bytes */
    AUDIO_SAMPLE_FREQ(48000),        /* Audio sampling frequency coded on 3 bytes */
    AUDIO_SAMPLE_FREQ(96000),        /* Audio sampling frequency coded on 3 bytes */
    /* 17 byte*/

    /* Endpoint 1 - Standard Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE,         /* bLength */
    USB_DESC_TYPE_ENDPOINT,                    /* bDescriptorType */
    AUDIO_OUT_EP,                              /* bEndpointAddress 1 out endpoint*/
    USBD_EP_TYPE_ISOC_ASYNC,                   /* bmAttributes */
    AUDIO_PACKET_SZE_24B(USBD_AUDIO_FREQ_MAX), /* wMaxPacketSize in Bytes (freq / 1000 + extra_samples) * channels * bytes_per_sample */
    0x01,                                      /* bInterval */
    0x00,                                      /* bRefresh */
    AUDIO_IN_EP,                               /* bSynchAddress */
    /* 09 byte*/

    /* Endpoint - Audio Streaming Descriptor */
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE, /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,             /* bDescriptor */
    0x01,                               /* bmAttributes - Sampling Frequency control is supported. See UAC Spec 1.0 p.62 */
    0x00,                               /* bLockDelayUnits */
    0x00,                               /* wLockDelay */
    0x00,
    /* 07 byte*/

    /* Endpoint 2 - Standard Descriptor - See UAC Spec 1.0 p.63 4.6.2.1 Standard AS Isochronous Synch Endpoint Descriptor */
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE, /* bLength */
    USB_DESC_TYPE_ENDPOINT,            /* bDescriptorType */
    AUDIO_IN_EP,                       /* bEndpointAddress */
    0x11,                              /* bmAttributes */
    0x03, 0x00,                        /* wMaxPacketSize in Bytes */
    0x01,                              /* bInterval 1ms */
    SOF_RATE,                          /* bRefresh 4ms = 2^2 */
    0x00,                              /* bSynchAddress */
    /* 09 byte*/
};

/** 
 * USB Standard Device Descriptor
 * @see https://www.keil.com/pack/doc/mw/USB/html/_u_s_b__device__qualifier__descriptor.html
 */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END = {
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

volatile uint32_t tx_flag = 1;
volatile uint32_t all_ready = 0;

/* FNSOF is critical for frequency changing to work */
volatile uint32_t fnsof = 0;

/**
 * @}
 */
static USBD_AUDIO_HandleTypeDef classdata = {0};


/** @defgroup USBD_AUDIO_Private_Functions
 * @{
 */

/**
 * @brief  USBD_AUDIO_Init
 *         Initialize the AUDIO interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_AUDIO_Init(USBD_HandleTypeDef* pdev, uint8_t cfgidx)
{
    USBD_AUDIO_HandleTypeDef* haudio;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, AUDIO_OUT_EP, USBD_EP_TYPE_ISOC, AUDIO_OUT_PACKET_24B);
    pdev->ep_out[AUDIO_OUT_EP & 0xFU].is_used = 1U;

    /* Open EP IN */
    USBD_LL_OpenEP(pdev, AUDIO_IN_EP, USBD_EP_TYPE_ISOC, AUDIO_IN_PACKET);
    pdev->ep_in[AUDIO_IN_EP & 0xFU].is_used = 1U;

    /* Flush feedback endpoint */
    USBD_LL_FlushEP(pdev, AUDIO_IN_EP);

    /** 
     * Set tx_flag 1 to block feedback transmission in SOF handler since 
     * device is not ready.
     */
    tx_flag = 1U;

    /* Allocate Audio structure */
    pdev->pClassData = &classdata;

    if (pdev->pClassData == NULL) {
        return USBD_FAIL;
    } else {
        haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;
        haudio->alt_setting = 0U;
        haudio->freq = USBD_AUDIO_FREQ_DEFAULT;
        haudio->bit_depth = 16U;
        haudio->vol = audio.volume;

        audio_init(haudio->freq, haudio->bit_depth);

        USBD_LL_PrepareReceive(pdev, AUDIO_OUT_EP, haudio->buffer, AUDIO_OUT_PACKET_24B);
    }
    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_Init
 *         DeInitialize the AUDIO layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_AUDIO_DeInit(USBD_HandleTypeDef* pdev,
        uint8_t cfgidx)
{
    /* Flush all endpoints */
    USBD_LL_FlushEP(pdev, AUDIO_OUT_EP);
    USBD_LL_FlushEP(pdev, AUDIO_IN_EP);

    /* Close EP OUT */
    USBD_LL_CloseEP(pdev, AUDIO_OUT_EP);
    pdev->ep_out[AUDIO_OUT_EP & 0xFU].is_used = 0U;

    /* Close EP IN */
    USBD_LL_CloseEP(pdev, AUDIO_IN_EP);
    pdev->ep_in[AUDIO_IN_EP & 0xFU].is_used = 0U;

    /* Clear feedback transmission flag */
    tx_flag = 0U;

    /* DeInit physical Interface components */
    if (pdev->pClassData != NULL) {

        audio_stop();

        pdev->pClassData = NULL;
    }

    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_Setup
 *         Handle the AUDIO specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */
static uint8_t USBD_AUDIO_Setup(USBD_HandleTypeDef* pdev,
        USBD_SetupReqTypedef* req)
{
    USBD_AUDIO_HandleTypeDef* haudio;
    uint16_t len;
    uint8_t* pbuf;
    uint16_t status_info = 0U;
    uint8_t ret = USBD_OK;

    haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        /* AUDIO Class Requests */
        case USB_REQ_TYPE_CLASS:
            switch (req->bRequest) {
                case AUDIO_REQ_GET_CUR:
                    AUDIO_REQ_GetCurrent(pdev, req);
                    break;

                case AUDIO_REQ_GET_MAX:
                    AUDIO_REQ_GetMax(pdev, req);
                    break;

                case AUDIO_REQ_GET_MIN:
                    AUDIO_REQ_GetMin(pdev, req);
                    break;

                case AUDIO_REQ_GET_RES:
                    AUDIO_REQ_GetRes(pdev, req);
                    break;

                case AUDIO_REQ_SET_CUR:
                    AUDIO_REQ_SetCurrent(pdev, req);
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
            }
            break;

            /* Standard Requests */
        case USB_REQ_TYPE_STANDARD:
            switch (req->bRequest) {
                case USB_REQ_GET_STATUS:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t*)(void*)&status_info, 2U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_GET_DESCRIPTOR:
                    if ((req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE) {
                        pbuf = USBD_AUDIO_CfgDesc + 18;
                        len = MIN(USB_AUDIO_DESC_SIZ, req->wLength);

                        USBD_CtlSendData(pdev, pbuf, len);
                    }
                    break;

                case USB_REQ_GET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t*)(void*)&haudio->alt_setting, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        if ((uint8_t)(req->wValue) <= USBD_MAX_NUM_INTERFACES) {
                            /* Do things only when alt_setting changes */
                            if (haudio->alt_setting != (uint8_t)(req->wValue)) {
                                haudio->alt_setting = (uint8_t)(req->wValue);
                                if (haudio->alt_setting == 0U) {
                                    AUDIO_OUT_StopAndReset(pdev);
                                } else if (haudio->alt_setting == 1U) {
                                    haudio->bit_depth = 16U;
                                    audio.bit_depth= 16;
                                    AUDIO_OUT_Restart(pdev);
                                } else if (haudio->alt_setting == 2U) {
                                    haudio->bit_depth = 24U;
                                    audio.bit_depth= 24;
                                    AUDIO_OUT_Restart(pdev);
                                }
                            }

                            USBD_LL_FlushEP(pdev, AUDIO_IN_EP);
                        } else {
                            /* Call the error management function (command will be nacked */
                            USBD_CtlError(pdev, req);
                            ret = USBD_FAIL;
                        }
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
            }
            break;
        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
    }

    return ret;
}

/**
 * @brief  USBD_AUDIO_GetCfgDesc
 *         return configuration descriptor
 * @param  speed : current device speed
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t* USBD_AUDIO_GetCfgDesc(uint16_t* length)
{
    *length = sizeof(USBD_AUDIO_CfgDesc);
    return USBD_AUDIO_CfgDesc;
}

/**
 * @brief  USBD_AUDIO_DataIn
 *         handle data IN Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_AUDIO_DataIn(USBD_HandleTypeDef* pdev,
        uint8_t epnum)
{
    /* epnum is the lowest 4 bits of bEndpointAddress. See UAC 1.0 spec, p.61 */
    if (epnum == (AUDIO_IN_EP & 0xf)) {
        tx_flag = 0U;
    }
    return USBD_OK;
}

static uint32_t audio_delta_fb(USBD_HandleTypeDef* pdev)
{
    USBD_AUDIO_HandleTypeDef* haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    uint32_t fb_value, fb_raw;

    /* Calculate feedback value based on the change of writable buffer size */
    int16_t delta;
    delta = audio_clock_samples_delta();

    float x = delta;
    x = x * (x/(float)100);
    x = x * x + x*0.35;
    delta = x;

    /* int16_t aa = delta > 0 ? delta : -delta; */
    /* if (aa < 100) { */
        /* delta = delta / 2; */
    /* } else if (aa > 200) { */
        /* delta = delta * 2; */
    /* } */

    fb_value = haudio->freq - delta;

    /*
     * Linux usb audio驱动中freq反馈值的范围为:
     * (freq - freq/8) <= fb <= (freq + freq /4)
     */
    if (fb_value < (haudio->freq - haudio->freq/8)) {
        fb_value = haudio->freq - haudio->freq/8;
    } else if (fb_value > (haudio->freq + haudio->freq/4)) {
        fb_value = haudio->freq + haudio->freq/4;
    }

    fb_raw = ((fb_value / 1000)<<14) | ((fb_value %1000)<<4);
    return fb_raw;
}

/**
 * @brief  USBD_AUDIO_SOF
 *         handle SOF event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_AUDIO_SOF(USBD_HandleTypeDef* pdev)
{
    /* Do stuff only when playing */
    if (all_ready == 1U) {
        /* Transmit feedback only when the last one is transmitted */
        if (tx_flag == 0U) {
            /* Get FNSOF. Use volatile for fnsof_new since its address is mapped to a hardware register. */
            USB_OTG_GlobalTypeDef* USBx = USB_OTG_FS;
            uint32_t USBx_BASE = (uint32_t)USBx;
            uint32_t volatile fnsof_new = (USBx_DEVICE->DSTS & USB_OTG_DSTS_FNSOF) >> 8;

            if ((fnsof & 0x1) == (fnsof_new & 0x1)) {

                uint32_t fb_raw=0;
                fb_raw = audio_delta_fb(pdev);

                USBD_LL_Transmit(pdev, AUDIO_IN_EP, (uint8_t*)&fb_raw, 3U);
                /* Block transmission until it's finished. */
                tx_flag = 1U;
            }
        }
    }

    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_Sync
 *         handle Sync event called from usbd_audio_if.c
 * @param  pdev: device instance
 * @retval status
 */
void USBD_AUDIO_Sync(USBD_HandleTypeDef* pdev, AUDIO_OffsetTypeDef offset)
{
}

/**
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * USBD_AUDIO_IsoINIncomplete & USBD_AUDIO_IsoOutIncomplete are not 
 * enabled by default.
 * 
 * Go to Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
 * Fill in USBD_LL_IsoINIncomplete and USBD_LL_IsoOUTIncomplete with 
 * actual handler functions.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

/**
 * @brief  USBD_AUDIO_IsoINIncomplete
 *         handle data ISO IN Incomplete event
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_AUDIO_IsoINIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum)
{
    USB_OTG_GlobalTypeDef* USBx = USB_OTG_FS;
    uint32_t USBx_BASE = (uint32_t)USBx;
    fnsof = (USBx_DEVICE->DSTS & USB_OTG_DSTS_FNSOF) >> 8;

    if (tx_flag == 1U) {
        tx_flag = 0U;
        USBD_LL_FlushEP(pdev, AUDIO_IN_EP);
    }

    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_IsoOutIncomplete
 *         handle data ISO OUT Incomplete event
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_AUDIO_IsoOutIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum)
{
    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_DataOut
 *         handle data OUT Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_AUDIO_DataOut(USBD_HandleTypeDef* pdev,
        uint8_t epnum)
{
    if (all_ready == 1U && epnum == AUDIO_OUT_EP) {
        uint32_t curr_length = USBD_GetRxCount(pdev, epnum);

        /* Ignore strangely large packets */
        if (curr_length > AUDIO_OUT_PACKET_24B) {
            curr_length = 0U;
        }

        USBD_AUDIO_HandleTypeDef* haudio;
        haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

        audio_append(haudio->buffer, curr_length);

        USBD_LL_PrepareReceive(pdev, AUDIO_OUT_EP, haudio->buffer, AUDIO_OUT_PACKET_24B);
    }

    return USBD_OK;
}

/**
 * @brief  AUDIO_Req_GetCurrent
 *         Handles the GET_CUR Audio control request.
 * @param  pdev: instance
 * @param  req: setup class request
 * @retval status
 */
static void AUDIO_REQ_GetCurrent(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req)
{
    USBD_AUDIO_HandleTypeDef* haudio;
    haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    if ((req->bmRequest & 0x1f) == AUDIO_CONTROL_REQ) {
        switch (HIBYTE(req->wValue)) {
            case AUDIO_CONTROL_REQ_FU_MUTE: {
                                                /* Current mute state */
                                                uint8_t mute = 0;
                                                USBD_CtlSendData(pdev, &mute, 1);
                                            };
                                            break;
            case AUDIO_CONTROL_REQ_FU_VOL: {
                                               /* Current volume. See UAC Spec 1.0 p.77 */
                                               USBD_CtlSendData(pdev, (uint8_t*)&haudio->vol, 2);
                                           };
                                           break;
        }
    } else if ((req->bmRequest & 0x1f) == AUDIO_STREAMING_REQ) {
        if (HIBYTE(req->wValue) == AUDIO_STREAMING_REQ_FREQ_CTRL) {
            /* Current frequency */
            uint32_t freq __attribute__((aligned(4))) = haudio->freq;
            USBD_CtlSendData(pdev, (uint8_t*)&freq, 3);
        }
    }
}

/**
 * @brief  AUDIO_Req_GetMax
 *         Handles the GET_MAX Audio control request.
 * @param  pdev: instance
 * @param  req: setup class request
 * @retval status
 */
static void AUDIO_REQ_GetMax(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req)
{
    if ((req->bmRequest & 0x1f) == AUDIO_CONTROL_REQ) {
        switch (HIBYTE(req->wValue)) {
            case AUDIO_CONTROL_REQ_FU_VOL: {
                                               int16_t vol_max = USBD_AUDIO_VOL_MAX;
                                               USBD_CtlSendData(pdev, (uint8_t*)&vol_max, 2);
                                           };
                                           break;
        }
    }
}

/**
 * @brief  AUDIO_Req_GetMin
 *         Handles the GET_MIN Audio control request.
 * @param  pdev: instance
 * @param  req: setup class request
 * @retval status
 */
static void AUDIO_REQ_GetMin(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req)
{
    if ((req->bmRequest & 0x1f) == AUDIO_CONTROL_REQ) {
        switch (HIBYTE(req->wValue)) {
            case AUDIO_CONTROL_REQ_FU_VOL: {
                                               int16_t vol_min = USBD_AUDIO_VOL_MIN;
                                               USBD_CtlSendData(pdev, (uint8_t*)&vol_min, 2);
                                           };
                                           break;
        }
    }
}

/**
 * @brief  AUDIO_Req_GetRes
 *         Handles the GET_RES Audio control request.
 * @param  pdev: instance
 * @param  req: setup class request
 * @retval status
 */
static void AUDIO_REQ_GetRes(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req)
{
    if ((req->bmRequest & 0x1f) == AUDIO_CONTROL_REQ) {
        switch (HIBYTE(req->wValue)) {
            case AUDIO_CONTROL_REQ_FU_VOL: {
                                               int16_t vol_res = USBD_AUDIO_VOL_STEP;
                                               USBD_CtlSendData(pdev, (uint8_t*)&vol_res, 2);
                                           };
                                           break;
        }
    }
}

/**
 * @brief  AUDIO_Req_SetCurrent
 *         Handles the SET_CUR Audio control request.
 * @param  pdev: instance
 * @param  req: setup class request
 * @retval status
 */
static void AUDIO_REQ_SetCurrent(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req)
{
    USBD_AUDIO_HandleTypeDef* haudio;
    haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    if (req->wLength) {
        /* Prepare the reception of the buffer over EP0 */
        USBD_CtlPrepareRx(pdev,
                haudio->control.data,
                req->wLength);

        haudio->control.cmd = AUDIO_REQ_SET_CUR;          /* Set the request value */
        haudio->control.req_type = req->bmRequest & 0x1f; /* Set the request type. See UAC Spec 1.0 - 5.2.1 Request Layout */
        haudio->control.len = (uint8_t)req->wLength;      /* Set the request data length */
        haudio->control.unit = HIBYTE(req->wIndex);       /* Set the request target unit */
        haudio->control.cs = HIBYTE(req->wValue);         /* Set the request control selector (high byte) */
        haudio->control.cn = LOBYTE(req->wValue);         /* Set the request control number (low byte) */
    }
}

/**
 * @brief  USBD_AUDIO_EP0_RxReady
 *         handle EP0 Rx Ready event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_AUDIO_EP0_RxReady(USBD_HandleTypeDef* pdev)
{
    USBD_AUDIO_HandleTypeDef* haudio;
    haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    if (haudio->control.cmd == AUDIO_REQ_SET_CUR) { /* In this driver, to simplify code, only SET_CUR request is managed */

        if (haudio->control.req_type == AUDIO_CONTROL_REQ) {
            switch (haudio->control.cs) {
                /* Mute Control */
                case AUDIO_CONTROL_REQ_FU_MUTE: {
                                                    audio_setMute(haudio->control.data[0]);
                                                };
                                                break;
                                                /* Volume Control */
                case AUDIO_CONTROL_REQ_FU_VOL: {
                                                   int16_t vol = *(int16_t*)&haudio->control.data[0];
                                                   haudio->vol = vol;

                                                   audio_setVolume(VOL_PERCENT(vol));
                                               };
                                               break;
            }

        } else if (haudio->control.req_type == AUDIO_STREAMING_REQ) {
            /* Frequency Control */
            if (haudio->control.cs == AUDIO_STREAMING_REQ_FREQ_CTRL) {
                uint32_t new_freq = *(uint32_t*)&haudio->control.data & 0x00ffffff;

                if (haudio->freq != new_freq) {
                    haudio->freq = new_freq;
                    AUDIO_OUT_Restart(pdev);
                }
            }
        }

        haudio->control.req_type = 0U;
        haudio->control.cs = 0U;
        haudio->control.cn = 0U;
        haudio->control.cmd = 0U;
        haudio->control.len = 0U;
    }

    return USBD_OK;
}

/**
 * @brief  USBD_AUDIO_EP0_TxReady
 *         handle EP0 TRx Ready event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_AUDIO_EP0_TxReady(USBD_HandleTypeDef* pdev)
{
    /* Only OUT control data are processed */
    return USBD_OK;
}

/**
 * @brief  Stop playing and reset buffer pointers
 * @param  pdev: instance
 */
static void AUDIO_OUT_StopAndReset(USBD_HandleTypeDef* pdev)
{
    all_ready = 0U;
    tx_flag = 1U;

    USBD_LL_FlushEP(pdev, AUDIO_IN_EP);
    USBD_LL_FlushEP(pdev, AUDIO_OUT_EP);

    audio_stop();
}

/**
 * @brief  Restart playing with new parameters
 * @param  pdev: instance
 */
static void AUDIO_OUT_Restart(USBD_HandleTypeDef* pdev)
{
    USBD_AUDIO_HandleTypeDef* haudio;
    haudio = (USBD_AUDIO_HandleTypeDef*)pdev->pClassData;

    AUDIO_OUT_StopAndReset(pdev);

    audio_init(haudio->freq, haudio->bit_depth);

    tx_flag = 0U;
    all_ready = 1U;
}

/**
 * @brief  DeviceQualifierDescriptor
 *         return Device Qualifier descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t* USBD_AUDIO_GetDeviceQualifierDesc(uint16_t* length)
{
    *length = sizeof(USBD_AUDIO_DeviceQualifierDesc);
    return USBD_AUDIO_DeviceQualifierDesc;
}

/* Convert USB volume value to uint8_t 0xff +24db - 0x00 -103db */
uint8_t VOL_PERCENT(int16_t vol)
{
/*
    0x7FFF: 127.9961 DB
    0x0100: 1.0000   DB
    0x0000: 0.0000   DB
    0xFE00: -1.0000  DB
    0x8001: -127.9961 DB
    0x8000: -∞
*/

    vol = vol / 128;

    if (vol > 48) {
        vol = 48;
    }
    if (vol < -207) {
        vol = -207;
    }
    vol += 207;
    return vol;
}


