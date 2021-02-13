#include "base.h"

static bsp_usb_clock_config(void);

static void MY_PCD_MspInit(PCD_HandleTypeDef *hpcd);
static void MY_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);
static void MY_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void MY_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void MY_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void MY_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void MY_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
static void MY_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

PCD_HandleTypeDef hpcd = {
  .MspInitCallback = MY_PCD_MspInit,
  .MspDeInitCallback = MY_PCD_MspDeInit,
};

static void MY_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    bsp_usb_clock_config();

    hpcd->SetupStageCallback = MY_PCD_SetupStageCallback;
    hpcd->SOFCallback = MY_PCD_SOFCallback;
    hpcd->ResetCallback = MY_PCD_ResetCallback;
    hpcd->SuspendCallback = MY_PCD_SuspendCallback;
    hpcd->ResumeCallback = MY_PCD_ResumeCallback;
    hpcd->DataInStageCallback = MY_PCD_DataInStageCallback;
    hpcd->DataOutStageCallback = MY_PCD_DataOutStageCallback;
    hpcd->ISOOUTIncompleteCallback = MY_PCD_ISOOUTIncompleteCallback;
    hpcd->ISOINIncompleteCallback = MY_PCD_ISOINIncompleteCallback;
    hpcd->ConnectCallback = MY_PCD_ConnectCallback;
    hpcd->DisconnectCallback = MY_PCD_DisconnectCallback;

    GPIO_InitTypeDef  GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure DM DP Pins */
    GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Enable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Set USBFS Interrupt priority */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);

    /* Enable USBFS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    //usb上拉电阻
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin =  GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
    HAL_Delay(10);
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);

    printf("pcd initd\n");
}

static void MY_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    printf("pcd deInit\n");
    /* Disable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

    //usb上拉电阻
    HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);
}

static bsp_usb_clock_config(void)
{
    RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
    HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
    RCC_ExCLKInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIM = 8;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIN = 384;
    RCC_ExCLKInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
}

static void MY_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);
}

static void MY_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

static void MY_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

static void MY_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SOF(hpcd->pData);
}

static void MY_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Reset(hpcd->pData);
    USBD_LL_SetSpeed(hpcd->pData, USBD_SPEED_FULL);
}

static void MY_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Suspend(hpcd->pData);
}

static void MY_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Resume(hpcd->pData);
}

static void MY_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

static void MY_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

static void MY_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    printf("pcd con\n");
    USBD_LL_DevConnected(hpcd->pData);
}

static void MY_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    printf("pcd dis\n");
    USBD_LL_DevDisconnected(hpcd->pData);
}

/***************** 中断 *********************/

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}
