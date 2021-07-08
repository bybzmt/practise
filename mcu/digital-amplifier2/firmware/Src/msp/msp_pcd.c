#include "base.h"
#include "msp_clock.h"
#include "usbd_audio.h"

static void msp_pcd_irq(void);
static void msp_pcd_MspInit(PCD_HandleTypeDef *hpcd);
static void msp_pcd_MspDeInit(PCD_HandleTypeDef *hpcd);
static void msp_pcd_SetupStageCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void msp_pcd_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void msp_pcd_SOFCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_ResetCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_SuspendCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_ResumeCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void msp_pcd_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
static void msp_pcd_ConnectCallback(PCD_HandleTypeDef *hpcd);
static void msp_pcd_DisconnectCallback(PCD_HandleTypeDef *hpcd);

PCD_HandleTypeDef hpcd = {
    .MspInitCallback = msp_pcd_MspInit,
    .MspDeInitCallback = msp_pcd_MspDeInit,
};

static void msp_pcd_MspInit(PCD_HandleTypeDef *hpcd)
{
    msp_clock_usb_config();

    hpcd->SetupStageCallback = msp_pcd_SetupStageCallback;
    hpcd->SOFCallback = msp_pcd_SOFCallback;
    hpcd->ResetCallback = msp_pcd_ResetCallback;
    hpcd->SuspendCallback = msp_pcd_SuspendCallback;
    hpcd->ResumeCallback = msp_pcd_ResumeCallback;
    hpcd->DataInStageCallback = msp_pcd_DataInStageCallback;
    hpcd->DataOutStageCallback = msp_pcd_DataOutStageCallback;
    hpcd->ISOOUTIncompleteCallback = msp_pcd_ISOOUTIncompleteCallback;
    hpcd->ISOINIncompleteCallback = msp_pcd_ISOINIncompleteCallback;
    hpcd->ConnectCallback = msp_pcd_ConnectCallback;
    hpcd->DisconnectCallback = msp_pcd_DisconnectCallback;

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
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);

    NVIC_SetVector(OTG_FS_IRQn, (uint32_t)&msp_pcd_irq);

    /* Enable USBFS Interrupt */
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    //usb上拉电阻
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin =  GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);

    printf("pcd initd\n");
}

static void msp_pcd_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    printf("pcd deInit\n");

    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    /* Disable USB FS Clock */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

    //usb上拉电阻
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
}

static void msp_pcd_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SetupStage(hpcd->pData, (uint8_t *)hpcd->Setup);
}

static void msp_pcd_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

static void msp_pcd_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

static void msp_pcd_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SOF(hpcd->pData);
}

static void msp_pcd_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Reset(hpcd->pData);
    USBD_LL_SetSpeed(hpcd->pData, USBD_SPEED_FULL);
}

static void msp_pcd_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Suspend(hpcd->pData);
}

static void msp_pcd_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_Resume(hpcd->pData);
}

static void msp_pcd_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum); */

    USBD_HandleTypeDef *pdev = hpcd->pData;

    if (pdev->dev_state == USBD_STATE_CONFIGURED)
    {
        if(pdev->pClass->IsoOUTIncomplete != NULL)
        {
            pdev->pClass->IsoOUTIncomplete(pdev, epnum);
        }
    }
}

static void msp_pcd_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* USBD_LL_IsoINIncomplete(hpcd->pData, epnum); */

    USBD_HandleTypeDef *pdev = hpcd->pData;

    if (pdev->dev_state == USBD_STATE_CONFIGURED)
    {
        if (pdev->pClass->IsoINIncomplete != NULL)
        {
            pdev->pClass->IsoINIncomplete(pdev, epnum);
        }
    }
}

static void msp_pcd_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevConnected(hpcd->pData);
}

static void msp_pcd_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevDisconnected(hpcd->pData);
}

/***************** 中断 *********************/

static void msp_pcd_irq(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}

/* void OTG_FS_IRQHandler(void) */
/* { */
    /* msp_pcd_irq(); */
/* } */
