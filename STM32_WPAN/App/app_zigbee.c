
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/app_zigbee.c
  * @author  MCD Application Team
  * @brief   Zigbee Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_entry.h"
#include "dbg_trace.h"
#include "app_zigbee.h"
#include "zigbee_interface.h"
#include "shci.h"
#include "stm_logging.h"
#include "app_conf.h"
#include "stm32wbxx_core_interface_def.h"
#include "zigbee_types.h"
#include "tx_api.h"

/* Private includes -----------------------------------------------------------*/
#include <assert.h>
#include "zcl/zcl.h"
#include "zcl/general/zcl.identify.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.color.h"
#include "zcl/general/zcl.level.h"

/* USER CODE BEGIN Includes */
#include "main.h"
#include "rgb_driver.h"
#include "zcl.basic.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private defines -----------------------------------------------------------*/
#define APP_ZIGBEE_STARTUP_FAIL_DELAY               500U
#define CHANNEL                                     11

#define SW1_ENDPOINT                                1

/* USER CODE BEGIN PD */
#define ATTR_COLOR_TEMP_MIN   100 /* 10000K */
#define ATTR_COLOR_TEMP_MAX   500 /* 2000K */
#define ATTR_COLOR_TEMP_START 333 /* 3000K */
#define ATTR_ONOFF_TRANS_TIME_DEFAULT 0
/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* External definition -------------------------------------------------------*/
enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config);

/* USER CODE BEGIN ED */
/* USER CODE END ED */

/* Private function prototypes -----------------------------------------------*/
static void APP_ZIGBEE_StackLayersInit(void);
static void APP_ZIGBEE_ConfigEndpoints(void);
static void APP_ZIGBEE_NwkForm(void);

static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode);
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void);

static void Wait_Getting_Ack_From_M0(void);
static void Receive_Ack_From_M0(void);
static void Receive_Notification_From_M0(void);

static void APP_ZIGBEE_ProcessNotifyM0ToM4(ULONG argument);
static void APP_ZIGBEE_ProcessRequestM0ToM4(ULONG argument);
static void APP_ZIGBEE_ProcessNwkForm(ULONG argument);

static void APP_ZIGBEE_TimingElapsed(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
static TL_CmdPacket_t   *p_ZIGBEE_otcmdbuffer;
static TL_EvtPacket_t   *p_ZIGBEE_notif_M0_to_M4;
static TL_EvtPacket_t   *p_ZIGBEE_request_M0_to_M4;
static __IO uint32_t    CptReceiveNotifyFromM0 = 0;
static __IO uint32_t    CptReceiveRequestFromM0 = 0;

static uint8_t  TimerID;

TX_THREAD       OsTaskNotifyM0ToM4Id;
TX_THREAD       OsTaskRequestM0ToM4Id;
TX_THREAD       OsTaskNwkFormId;
TX_MUTEX        MtxZigbeeId;

TX_SEMAPHORE    TransferToM0Semaphore;
TX_SEMAPHORE    StartupEndSemaphore;

TX_SEMAPHORE    NWKFormSemaphore;
TX_SEMAPHORE    NWKFormWaitSemaphore;
TX_SEMAPHORE    NotifyM0ToM4Semaphore;
TX_SEMAPHORE    RequestM0ToM4Semaphore;

PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t ZigbeeOtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t ZigbeeNotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

struct zigbee_app_info
{
  bool has_init;
  struct ZigBeeT *zb;
  enum ZbStartType startupControl;
  enum ZbStatusCodeT join_status;
  uint32_t join_delay;
  bool init_after_join;

  struct ZbZclClusterT *identify_server_1;
  struct ZbZclClusterT *onOff_server_1;
  struct ZbZclClusterT *colorControl_server_1;
  struct ZbZclClusterT *levelControl_server_1;
};
static struct zigbee_app_info zigbee_app_info;

/* OnOff server 1 custom callbacks */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT onOff_server_1_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclOnOffServerCallbacksT OnOffServerCallbacks_1 =
{
  .off = onOff_server_1_off,
  .on = onOff_server_1_on,
  .toggle = onOff_server_1_toggle,
};

/* ColorControl server 1 custom callbacks */
static enum ZclStatusCodeT colorControl_server_1_move_to_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_to_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_color_loop_set(struct ZbZclClusterT *cluster, struct ZbZclColorClientColorLoopSetReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_stop_move_step(struct ZbZclClusterT *cluster, struct ZbZclColorClientStopMoveStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_move_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT colorControl_server_1_step_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclColorServerCallbacksT ColorServerCallbacks_1 =
{
  .move_to_color_xy = colorControl_server_1_move_to_color_xy,
  .move_color_xy = colorControl_server_1_move_color_xy,
  .step_color_xy = colorControl_server_1_step_color_xy,
  .move_to_color_temp = colorControl_server_1_move_to_color_temp,
  .color_loop_set = colorControl_server_1_color_loop_set,
  .stop_move_step = colorControl_server_1_stop_move_step,
  .move_color_temp = colorControl_server_1_move_color_temp,
  .step_color_temp = colorControl_server_1_step_color_temp,
};

/* LevelControl server 1 custom callbacks */
static enum ZclStatusCodeT levelControl_server_1_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT levelControl_server_1_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg);

static struct ZbZclLevelServerCallbacksT LevelServerCallbacks_1 =
{
  .move_to_level = levelControl_server_1_move_to_level,
  .move = levelControl_server_1_move,
  .step = levelControl_server_1_step,
  .stop = levelControl_server_1_stop,
};

/* USER CODE BEGIN PV */
uint8_t manufacturerName[] = "_Cinek2030";
uint8_t modelName[] = "_WS2812A controller";
/* USER CODE END PV */
/* Functions Definition ------------------------------------------------------*/

/* OnOff server off 1 command callback */
static enum ZclStatusCodeT onOff_server_1_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 0 OnOff server 1 off 1 */
  uint8_t endpoint;

  endpoint = ZbZclClusterGetEndpoint(cluster);
  if (endpoint == SW1_ENDPOINT) 
  {  
    APP_DBG("onOff_server_1_off");
    ZbZclAttrIntegerWrite(cluster, ZCL_ONOFF_ATTR_ONOFF, 0);
    tx_event_flags_set(&rgb_driver_flags, RGB_SWITCH_OFF, TX_OR);
  }
  else
  {
    /* unknown endpoint */
    APP_DBG("onOff_server_1_off unknown endpoint");
    return ZCL_STATUS_FAILURE;
  }
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 0 OnOff server 1 off 1 */
}

/* OnOff server on 1 command callback */
static enum ZclStatusCodeT onOff_server_1_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 1 OnOff server 1 on 1 */
  uint8_t endpoint;

  endpoint = ZbZclClusterGetEndpoint(cluster);
  if (endpoint == SW1_ENDPOINT) 
  {  
    APP_DBG("onOff_server_1_on");
    ZbZclAttrIntegerWrite(cluster, ZCL_ONOFF_ATTR_ONOFF, 1);
    tx_event_flags_set(&rgb_driver_flags, RGB_SWITCH_ON, TX_OR);
  }
  else
  {
    /* unknown endpoint */
    APP_DBG("onOff_server_1_on unknown endpoint");
    return ZCL_STATUS_FAILURE;
  }
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 1 OnOff server 1 on 1 */
}

/* OnOff server toggle 1 command callback */
static enum ZclStatusCodeT onOff_server_1_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 2 OnOff server 1 toggle 1 */
  uint8_t endpoint;
  uint8_t attrVal;

  endpoint = ZbZclClusterGetEndpoint(cluster);
  if (endpoint == SW1_ENDPOINT) 
  {  
    APP_DBG("onOff_server_1_toggle");

    if (ZbZclAttrRead(cluster, ZCL_ONOFF_ATTR_ONOFF, NULL, &attrVal, sizeof(attrVal), false) != ZCL_STATUS_SUCCESS) 
    {
      return ZCL_STATUS_FAILURE;
    }
    
    if (attrVal != 0) 
    {
      return onOff_server_1_off(cluster, srcInfo, arg);
    }
    else
    {
      return onOff_server_1_on(cluster, srcInfo, arg);
    }
  }
  else
  {
    /* unknown endpoint */
    APP_DBG("onOff_server_1_toggle unknown endpoint");
    return ZCL_STATUS_FAILURE;
  }
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 2 OnOff server 1 toggle 1 */
}

/* ColorControl server move_to_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 3 ColorControl server 1 move_to_color_xy 1 */
  APP_DBG("colorControl_server_1_move_to_color_xy(x=%d y=%d)", req->color_x, req->color_y);

  (void)ZbZclAttrIntegerWrite(cluster, ZCL_COLOR_ATTR_CURRENT_X, req->color_x);
	(void)ZbZclAttrIntegerWrite(cluster, ZCL_COLOR_ATTR_CURRENT_Y, req->color_y);
	(void)ZbZclAttrIntegerWrite(cluster, ZCL_COLOR_ATTR_REMAINING_TIME, req->transition_time);

  struct XY color_xy = {req->color_x, req->color_y};
  RGB_params.color = convert_xy_to_RGB(color_xy);
	/* execute action if the device is on */
	if(RGB_params.isOn)
	{
		RGB_params.mode = RGB_MODE_STATIC;
		tx_event_flags_set(&rgb_driver_flags, RGB_ACTION_REQUEST, TX_OR);
	}

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 3 ColorControl server 1 move_to_color_xy 1 */
}

/* ColorControl server move_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 4 ColorControl server 1 move_color_xy 1 */
  APP_DBG("colorControl_server_1_move_color_xy");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 4 ColorControl server 1 move_color_xy 1 */
}

/* ColorControl server step_color_xy 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_color_xy(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorXYReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 5 ColorControl server 1 step_color_xy 1 */
  APP_DBG("colorControl_server_1_step_color_xy");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 5 ColorControl server 1 step_color_xy 1 */
}

/* ColorControl server move_to_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_to_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveToColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 6 ColorControl server 1 move_to_color_temp 1 */
  APP_DBG("colorControl_server_1_move_to_color_temp (temp=%d)", req->color_temp);
  
  struct XY color_xy = color_temperature_to_xy(req->color_temp);
	(void)ZbZclAttrIntegerWrite(cluster, ZCL_COLOR_ATTR_COLOR_TEMP_MIREDS,req->color_temp);
	(void)ZbZclAttrIntegerWrite(cluster, ZCL_COLOR_ATTR_REMAINING_TIME, req->transition_time);
	RGB_params.color = convert_xy_to_RGB(color_xy);
	/* execute action if the device is on */
	if(RGB_params.isOn)
	{
		RGB_params.mode = RGB_MODE_STATIC;
		tx_event_flags_set(&rgb_driver_flags, RGB_ACTION_REQUEST, TX_OR);
	}  

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 6 ColorControl server 1 move_to_color_temp 1 */
}

/* ColorControl server color_loop_set 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_color_loop_set(struct ZbZclClusterT *cluster, struct ZbZclColorClientColorLoopSetReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 7 ColorControl server 1 color_loop_set 1 */
  APP_DBG("colorControl_server_1_color_loop_set; a=%d, d=%d, m=%d, o=%d, h=%d, t=%d, f=%d", req->action, req->direction, req->mask, req->override, req->start_hue, req->transition_time, req->update_flags);
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 7 ColorControl server 1 color_loop_set 1 */
}

/* ColorControl server stop_move_step 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_stop_move_step(struct ZbZclClusterT *cluster, struct ZbZclColorClientStopMoveStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 8 ColorControl server 1 stop_move_step 1 */
  APP_DBG("colorControl_server_1_stop_move_step");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 8 ColorControl server 1 stop_move_step 1 */
}

/* ColorControl server move_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_move_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientMoveColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 9 ColorControl server 1 move_color_temp 1 */
  APP_DBG("colorControl_server_1_move_color_temp");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 9 ColorControl server 1 move_color_temp 1 */
}

/* ColorControl server step_color_temp 1 command callback */
static enum ZclStatusCodeT colorControl_server_1_step_color_temp(struct ZbZclClusterT *cluster, struct ZbZclColorClientStepColorTempReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 10 ColorControl server 1 step_color_temp 1 */
  APP_DBG("colorControl_server_1_step_color_temp");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 10 ColorControl server 1 step_color_temp 1 */
}

/* LevelControl server move_to_level 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_move_to_level(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveToLevelReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 11 LevelControl server 1 move_to_level 1 */
  APP_DBG("levelControl_server_1_move_to_level; lvl=%d, transTime=%d, onOff=%d", req->level, req->transition_time, req->with_onoff);
  RGB_params.targetLevel = req->level;
  RGB_params.transitionTime = req->transition_time;

  if((req->with_onoff) && (RGB_params.isOn == FALSE) && (RGB_params.targetLevel > 0))
  {
    /* switch the device on and change level */
    RGB_params.currentLevel = 0;
    ZbZclAttrIntegerWrite(cluster, ZCL_ONOFF_ATTR_ONOFF, 1);
    APP_DBG("levelControl_server_1_move_to_level -> device on request");
    tx_event_flags_set(&rgb_driver_flags, RGB_SWITCH_ON, TX_OR);
  }
  else if((req->with_onoff) && (RGB_params.isOn == TRUE) && (RGB_params.targetLevel == 0))
  {
    /* change level and switch the device Off */
    ZbZclAttrIntegerWrite(cluster, ZCL_ONOFF_ATTR_ONOFF, 0);
    APP_DBG("levelControl_server_1_move_to_level -> device off request");
    tx_event_flags_set(&rgb_driver_flags, RGB_SWITCH_OFF, TX_OR);
  }
  else if(RGB_params.isOn == TRUE)
  {
    /* change level only */
    tx_event_flags_set(&rgb_driver_flags, RGB_LVL_CHG_REQUEST, TX_OR);
  }
  else
  {
    /* ignore when device is off and with_onoff argument is false */
    APP_DBG("levelControl_server_1_move_to_level ignored - device is off");
  }

  ZbZclAttrIntegerWrite(cluster, ZCL_LEVEL_ATTR_CURRLEVEL, RGB_params.targetLevel);

  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 11 LevelControl server 1 move_to_level 1 */
}

/* LevelControl server move 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_move(struct ZbZclClusterT *cluster, struct ZbZclLevelClientMoveReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 12 LevelControl server 1 move 1 */
  APP_DBG("levelControl_server_1_move");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 12 LevelControl server 1 move 1 */
}

/* LevelControl server step 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_step(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStepReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 13 LevelControl server 1 step 1 */
  APP_DBG("levelControl_server_1_step");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 13 LevelControl server 1 step 1 */
}

/* LevelControl server stop 1 command callback */
static enum ZclStatusCodeT levelControl_server_1_stop(struct ZbZclClusterT *cluster, struct ZbZclLevelClientStopReqT *req, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
  /* USER CODE BEGIN 14 LevelControl server 1 stop 1 */
  APP_DBG("levelControl_server_1_stop");
  return ZCL_STATUS_SUCCESS;
  /* USER CODE END 14 LevelControl server 1 stop 1 */
}

/**
 * @brief  Zigbee application initialization
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_Init(void)
{
  SHCI_CmdStatus_t ZigbeeInitStatus;
  UINT ThreadXStatus;
  CHAR * pTempBuf = TX_NULL;

  APP_DBG("APP_ZIGBEE_Init");

  /* Check the compatibility with the Coprocessor Wireless Firmware loaded */
  APP_ZIGBEE_CheckWirelessFirmwareInfo();

  /* Register cmdbuffer */
  APP_ZIGBEE_RegisterCmdBuffer(&ZigbeeOtCmdBuffer);

  /* Init config buffer and call TL_ZIGBEE_Init */
  APP_ZIGBEE_TL_INIT();

  /* Initialize the mutex */
  tx_mutex_create(&MtxZigbeeId, "MtxZigbeeId", TX_NO_INHERIT);

  /* Initialize the semaphores */
  tx_semaphore_create(&StartupEndSemaphore, "StartupEndSemaphore", 0); /*< Create the semaphore and make it busy at initialization */
  tx_semaphore_create(&TransferToM0Semaphore, "TransferToM0Semaphore", 0);
  tx_semaphore_create(&NWKFormSemaphore, "NWKFormSemaphore", 0);
  tx_semaphore_create(&NWKFormWaitSemaphore, "NWKFormWaitSemaphore", 0);
  tx_semaphore_create(&NotifyM0ToM4Semaphore, "NotifyM0ToM4Semaphore", 0);
  tx_semaphore_create(&RequestM0ToM4Semaphore, "RequestM0ToM4Semaphore", 0);

  /* Create the Timer service */
  HW_TS_Create(CFG_TIM_PROC_ID_ISR, &TimerID, hw_ts_SingleShot, APP_ZIGBEE_TimingElapsed);

  /* Register task */
  /* Create the different tasks */
  tx_byte_allocate(pBytePool, (VOID**) &pTempBuf, THREADX_STACK_SIZE_LARGE, TX_NO_WAIT);
  ThreadXStatus = tx_thread_create(&OsTaskNotifyM0ToM4Id,
                                   "NotifyM0ToM4Id",
                                   APP_ZIGBEE_ProcessNotifyM0ToM4,
                                   0,
                                   pTempBuf,
                                   THREADX_STACK_SIZE_LARGE,
                                   NOTIFY_M0_TO_M4_PRIORITY,
                                   NOTIFY_M0_TO_M4_PRIORITY,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
  if (ThreadXStatus != TX_SUCCESS)
  {
    APP_ZIGBEE_Error(ERR_ZIGBEE_THREAD_X_FAILED, 1);
  }

  tx_byte_allocate(pBytePool, (VOID**) &pTempBuf, THREADX_STACK_SIZE_LARGE, TX_NO_WAIT);
  ThreadXStatus = tx_thread_create(&OsTaskRequestM0ToM4Id,
                                   "RequestM0ToM4Id",
                                   APP_ZIGBEE_ProcessRequestM0ToM4,
                                   0,
                                   pTempBuf,
                                   THREADX_STACK_SIZE_LARGE,
                                   REQUEST_M0_TO_M4_PRIORITY,
                                   REQUEST_M0_TO_M4_PRIORITY,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
  if (ThreadXStatus != TX_SUCCESS)
  {
    APP_ZIGBEE_Error(ERR_ZIGBEE_THREAD_X_FAILED, 1);
  }

  /* Task associated with network creation process */
  tx_byte_allocate(pBytePool, (VOID**) &pTempBuf, THREADX_STACK_SIZE_LARGE, TX_NO_WAIT);
  ThreadXStatus = tx_thread_create(&OsTaskNwkFormId,
                                   "NwkFormId",
                                   APP_ZIGBEE_ProcessNwkForm,
                                   0,
                                   pTempBuf,
                                   THREADX_STACK_SIZE_LARGE,
                                   NWL_FORM_PRIORITY,
                                   NWL_FORM_PRIORITY,
                                   TX_NO_TIME_SLICE,
                                   TX_AUTO_START);
  if (ThreadXStatus != TX_SUCCESS)
  {
    APP_ZIGBEE_Error(ERR_ZIGBEE_THREAD_X_FAILED, 1);
  }

  /* USER CODE BEGIN APP_ZIGBEE_INIT */
  /* USER CODE END APP_ZIGBEE_INIT */

  /* Start the Zigbee on the CPU2 side */
  ZigbeeInitStatus = SHCI_C2_ZIGBEE_Init();
  /* Prevent unused argument(s) compilation warning */
  UNUSED(ZigbeeInitStatus);

  /* Initialize Zigbee stack layers */
  APP_ZIGBEE_StackLayersInit();

}

/**
 * @brief  Initialize Zigbee stack layers
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_StackLayersInit(void)
{
  APP_DBG("APP_ZIGBEE_StackLayersInit");

  zigbee_app_info.zb = ZbInit(0U, NULL, NULL);
  assert(zigbee_app_info.zb != NULL);

  /* Create the endpoint and cluster(s) */
  APP_ZIGBEE_ConfigEndpoints();

  /* USER CODE BEGIN APP_ZIGBEE_StackLayersInit */
  manufacturerName[0] = strlen(manufacturerName) - 1;
  ZbZclBasicWriteDirect(zigbee_app_info.zb, SW1_ENDPOINT, ZCL_BASIC_ATTR_MFR_NAME, manufacturerName, manufacturerName[0] + 1);
  modelName[0] = strlen(modelName) - 1;
  ZbZclBasicWriteDirect(zigbee_app_info.zb, SW1_ENDPOINT, ZCL_BASIC_ATTR_MODEL_NAME, modelName, modelName[0] + 1);
  /* USER CODE END APP_ZIGBEE_StackLayersInit */

  /* Configure the joining parameters */
  zigbee_app_info.join_status = (enum ZbStatusCodeT) 0x01; /* init to error status */
  zigbee_app_info.join_delay = HAL_GetTick(); /* now */
  zigbee_app_info.startupControl = ZbStartTypeJoin;

  /* Initialization Complete */
  zigbee_app_info.has_init = true;

  /* run the task */
  tx_semaphore_put(&NWKFormSemaphore);
}

/**
 * @brief  Configure Zigbee application endpoints
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ConfigEndpoints(void)
{
  struct ZbApsmeAddEndpointReqT req;
  struct ZbApsmeAddEndpointConfT conf;

  memset(&req, 0, sizeof(req));

  /* Endpoint: SW1_ENDPOINT */
  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_COLOR_DIMMABLE_LIGHT;
  req.endpoint = SW1_ENDPOINT;
  ZbZclAddEndpoint(zigbee_app_info.zb, &req, &conf);
  assert(conf.status == ZB_STATUS_SUCCESS);

  /* Identify server */
  zigbee_app_info.identify_server_1 = ZbZclIdentifyServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, NULL);
  assert(zigbee_app_info.identify_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.identify_server_1);
  /* OnOff server */
  zigbee_app_info.onOff_server_1 = ZbZclOnOffServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, &OnOffServerCallbacks_1, NULL);
  assert(zigbee_app_info.onOff_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.onOff_server_1);
  /* ColorControl server */
  struct ZbColorClusterConfig colorServerConfig_1 = {
    .callbacks = ColorServerCallbacks_1,
    /* Please complete the other attributes according to your application:
     *          .capabilities           //uint8_t (e.g. ZCL_COLOR_CAP_HS)
     *          .enhanced_supported     //bool
     */
    /* USER CODE BEGIN Color Server Config (endpoint1) */
    .capabilities = ZCL_COLOR_CAP_XY | ZCL_COLOR_CAP_COLOR_LOOP | ZCL_COLOR_CAP_COLOR_TEMP
    /* USER CODE END Color Server Config (endpoint1) */
  };
  zigbee_app_info.colorControl_server_1 = ZbZclColorServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, zigbee_app_info.onOff_server_1, NULL, 0, &colorServerConfig_1, NULL);
  assert(zigbee_app_info.colorControl_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.colorControl_server_1);
  /* LevelControl server */
  zigbee_app_info.levelControl_server_1 = ZbZclLevelServerAlloc(zigbee_app_info.zb, SW1_ENDPOINT, zigbee_app_info.onOff_server_1, &LevelServerCallbacks_1, NULL);
  assert(zigbee_app_info.levelControl_server_1 != NULL);
  ZbZclClusterEndpointRegister(zigbee_app_info.levelControl_server_1);

  /* USER CODE BEGIN CONFIG_ENDPOINT */
    APP_DBG("adding cluster attributes");

  static const struct ZbZclAttrT colorControl_attr_list[] =		/* add optional attributes of color control cluster */
  {
    {
        ZCL_COLOR_ATTR_REMAINING_TIME, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_CURRENT_X, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_CURRENT_Y, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_TEMP_MIREDS, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
      ZCL_COLOR_ATTR_COLOR_MODE, ZCL_DATATYPE_ENUMERATION_8BIT,
      ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
      ZCL_COLOR_ATTR_ENH_COLOR_MODE, ZCL_DATATYPE_ENUMERATION_8BIT,
      ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_LOOP_ACTIVE, ZCL_DATATYPE_UNSIGNED_8BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_LOOP_DIR, ZCL_DATATYPE_UNSIGNED_8BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_LOOP_TIME, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_TEMP_MIN, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_COLOR_TEMP_MAX, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
        ZCL_COLOR_ATTR_STARTUP_COLOR_TEMP, ZCL_DATATYPE_UNSIGNED_16BIT,
        ZCL_ATTR_FLAG_WRITABLE | ZCL_ATTR_FLAG_REPORTABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
  };
  ZbZclAttrAppendList( zigbee_app_info.colorControl_server_1, colorControl_attr_list, ZCL_ATTR_LIST_LEN(colorControl_attr_list));
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.colorControl_server_1, ZCL_COLOR_ATTR_COLOR_MODE, ZCL_COLOR_MODE_XY);
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.colorControl_server_1, ZCL_COLOR_ATTR_ENH_COLOR_MODE, ZCL_COLOR_ENH_MODE_CURR_XY);
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.colorControl_server_1, ZCL_COLOR_ATTR_COLOR_TEMP_MIN, ATTR_COLOR_TEMP_MIN);
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.colorControl_server_1, ZCL_COLOR_ATTR_COLOR_TEMP_MAX, ATTR_COLOR_TEMP_MAX);
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.colorControl_server_1, ZCL_COLOR_ATTR_STARTUP_COLOR_TEMP, ATTR_COLOR_TEMP_START);

  static const struct ZbZclAttrT levelControl_attr_list[] =		/* add optional attributes of level control cluster */
  {
    {
      ZCL_LEVEL_ATTR_ONLEVEL, ZCL_DATATYPE_UNSIGNED_8BIT,
      ZCL_ATTR_FLAG_WRITABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
      ZCL_LEVEL_ATTR_ONOFF_TRANS_TIME, ZCL_DATATYPE_UNSIGNED_16BIT,
      ZCL_ATTR_FLAG_WRITABLE | ZCL_ATTR_FLAG_PERSISTABLE, 0, NULL, {0, 0}, {0, 0}
    },
    {
      ZCL_LEVEL_ATTR_STARTUP_CURRLEVEL, ZCL_DATATYPE_UNSIGNED_8BIT,
      ZCL_ATTR_FLAG_WRITABLE | ZCL_ATTR_FLAG_PERSISTABLE | ZCL_ATTR_FLAG_CB_READ | ZCL_ATTR_FLAG_CB_WRITE, 0, NULL, {0, 0}, {0, 0}
    }
  };
  ZbZclAttrAppendList( zigbee_app_info.levelControl_server_1, levelControl_attr_list, ZCL_ATTR_LIST_LEN(levelControl_attr_list));
  (void)ZbZclAttrIntegerWrite( zigbee_app_info.levelControl_server_1, ZCL_LEVEL_ATTR_ONOFF_TRANS_TIME, ATTR_ONOFF_TRANS_TIME_DEFAULT);

  (void)ZbZclAttrIntegerWrite( zigbee_app_info.onOff_server_1, ZCL_ONOFF_ATTR_ONOFF, 0);
  /* USER CODE END CONFIG_ENDPOINT */
}

/**
  * @brief  Callback triggered when the Timer expire
  * @param  None
  * @retval None
  */
static void APP_ZIGBEE_TimingElapsed(void)
{
  APP_DBG("--- APP_ZIGBEE_InitWaitElapsed ---");
  tx_semaphore_put(&NWKFormWaitSemaphore);
}

/**
 * @brief  Handle Zigbee network forming and joining task for ThreadX
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessNwkForm(ULONG argument)
{
  UNUSED(argument);

  for(;;)
  {
    tx_semaphore_get(&NWKFormSemaphore, TX_WAIT_FOREVER);
    if ( zigbee_app_info.join_status != ZB_STATUS_SUCCESS )
    {
      HW_TS_Start(TimerID, APP_ZIGBEE_STARTUP_FAIL_DELAY);
      tx_semaphore_get(&NWKFormWaitSemaphore, TX_WAIT_FOREVER);
    }
    APP_ZIGBEE_NwkForm();
  }
}

/**
 * @brief  Handle Zigbee network forming and joining
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_NwkForm(void)
{
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    struct ZbStartupT config;
    enum ZbStatusCodeT status;

    /* Configure Zigbee Logging */
    ZbSetLogging(zigbee_app_info.zb, ZB_LOG_MASK_LEVEL_5, NULL);

    /* Attempt to join a zigbee network */
    ZbStartupConfigGetProDefaults(&config);

    /* Set the distributed network */
    APP_DBG("Network config : APP_STARTUP_DISTRIBUTED");
    config.startupControl = zigbee_app_info.startupControl;

    /* Set the TC address to be distributed. */
    config.security.trustCenterAddress = ZB_DISTRIBUTED_TC_ADDR;

    /* Using the Uncertified Distributed Global Key (d0:d1:d2:d3:d4:d5:d6:d7:d8:d9:da:db:dc:dd:de:df) */
    memcpy(config.security.distributedGlobalKey, sec_key_distrib_uncert, ZB_SEC_KEYSIZE);

    config.channelList.count = 1;
    config.channelList.list[0].page = 0;
    config.channelList.list[0].channelMask = 1 << CHANNEL; /*Channel in use */

    /* Using ZbStartupWait (blocking) */
    status = ZbStartupWait(zigbee_app_info.zb, &config);

    APP_DBG("ZbStartup Callback (status = 0x%02x)", status);
    zigbee_app_info.join_status = status;

    if (status == ZB_STATUS_SUCCESS)
    {
      zigbee_app_info.join_delay = 0U;
      zigbee_app_info.init_after_join = true;
      APP_DBG("Startup done !\n");
      /* USER CODE BEGIN 15 */

      /* USER CODE END 15 */
    }
    else
    {
      zigbee_app_info.startupControl = ZbStartTypeForm;
      APP_DBG("Startup failed, attempting again after a short delay (%d ms)", APP_ZIGBEE_STARTUP_FAIL_DELAY);
      zigbee_app_info.join_delay = HAL_GetTick() + APP_ZIGBEE_STARTUP_FAIL_DELAY;
      /* USER CODE BEGIN 16 */

      /* USER CODE END 16 */
    }
  }

  /* If Network forming/joining was not successful reschedule the current task to retry the process */
  if (zigbee_app_info.join_status != ZB_STATUS_SUCCESS)
  {
    tx_semaphore_put(&NWKFormSemaphore);
  }
  /* USER CODE BEGIN NW_FORM */
  /* USER CODE END NW_FORM */
}

/*************************************************************
 * ZbStartupWait Blocking Call
 *************************************************************/
struct ZbStartupWaitInfo
{
  bool active;
  enum ZbStatusCodeT status;
};

static void ZbStartupWaitCb(enum ZbStatusCodeT status, void *cb_arg)
{
  struct ZbStartupWaitInfo *info = cb_arg;

  info->status = status;
  info->active = false;
  tx_semaphore_put(&StartupEndSemaphore);
}

enum ZbStatusCodeT ZbStartupWait(struct ZigBeeT *zb, struct ZbStartupT *config)
{
  struct ZbStartupWaitInfo *info;
  enum ZbStatusCodeT status;

  info = malloc(sizeof(struct ZbStartupWaitInfo));
  if (info == NULL)
  {
    return ZB_STATUS_ALLOC_FAIL;
  }
  memset(info, 0, sizeof(struct ZbStartupWaitInfo));

  info->active = true;
  status = ZbStartup(zb, config, ZbStartupWaitCb, info);
  if (status != ZB_STATUS_SUCCESS)
  {
    free(info);
    return status;
  }

  tx_semaphore_get(&StartupEndSemaphore, TX_WAIT_FOREVER);
  status = info->status;
  free(info);
  return status;
}

/**
 * @brief  Trace the error or the warning reported.
 * @param  ErrId :
 * @param  ErrCode
 * @retval None
 */
void APP_ZIGBEE_Error(uint32_t ErrId, uint32_t ErrCode)
{
  switch (ErrId)
  {
    case ERR_ZIGBEE_THREAD_X_FAILED:
      APP_ZIGBEE_TraceError("ERROR : ERR_ZIGBEE_THREAD_X_FAILED ", ErrCode);
      break;

    default:
      APP_ZIGBEE_TraceError("ERROR Unknown ", 0);
      break;
  }
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/**
 * @brief  Warn the user that an error has occurred.
 *
 * @param  pMess  : Message associated to the error.
 * @param  ErrCode: Error code associated to the module (Zigbee or other module if any)
 * @retval None
 */
static void APP_ZIGBEE_TraceError(const char *pMess, uint32_t ErrCode)
{
  APP_DBG("**** Fatal error = %s (Err = %d)", pMess, ErrCode);
  /* USER CODE BEGIN TRACE_ERROR */
  /* USER CODE END TRACE_ERROR */

}

/**
 * @brief Check if the Coprocessor Wireless Firmware loaded supports Zigbee
 *        and display associated information
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_CheckWirelessFirmwareInfo(void)
{
  WirelessFwInfo_t wireless_info_instance;
  WirelessFwInfo_t *p_wireless_info = &wireless_info_instance;

  if (SHCI_GetWirelessFwInfo(p_wireless_info) != SHCI_Success)
  {
    APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
  }
  else
  {
    APP_DBG("**********************************************************");
    APP_DBG("WIRELESS COPROCESSOR FW:");
    /* Print version */
    APP_DBG("VERSION ID = %d.%d.%d", p_wireless_info->VersionMajor, p_wireless_info->VersionMinor, p_wireless_info->VersionSub);

    switch (p_wireless_info->StackType)
    {
      case INFO_STACK_TYPE_ZIGBEE_FFD:
        APP_DBG("FW Type : FFD Zigbee stack");
        break;

      case INFO_STACK_TYPE_ZIGBEE_RFD:
        APP_DBG("FW Type : RFD Zigbee stack");
        break;

      default:
        /* No Zigbee device supported ! */
        APP_ZIGBEE_Error((uint32_t)ERR_ZIGBEE_CHECK_WIRELESS, (uint32_t)ERR_INTERFACE_FATAL);
        break;
    }

    /* print the application name */
    char *__PathProject__ = (strstr(__FILE__, "Zigbee") ? strstr(__FILE__, "Zigbee") + 7 : __FILE__);
    char *pdel = NULL;
    if((strchr(__FILE__, '/')) == NULL)
    {
      pdel = strchr(__PathProject__, '\\');
    }
    else
    {
      pdel = strchr(__PathProject__, '/');
    }

    int index = (int)(pdel - __PathProject__);
    APP_DBG("Application flashed: %*.*s", index, index, __PathProject__);

    /* print channel */
    APP_DBG("Channel used: %d", CHANNEL);
    /* print Link Key */
    APP_DBG("Link Key: %.16s", sec_key_ha);
    /* print Link Key value hex */
    char Z09_LL_string[ZB_SEC_KEYSIZE*3+1];
    Z09_LL_string[0] = 0;
    for (int str_index = 0; str_index < ZB_SEC_KEYSIZE; str_index++)
    {
      sprintf(&Z09_LL_string[str_index*3], "%02x ", sec_key_ha[str_index]);
    }

    APP_DBG("Link Key value: %s", Z09_LL_string);
    /* print clusters allocated */
    APP_DBG("Clusters allocated are:");
    APP_DBG("identify Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("onOff Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("colorControl Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("levelControl Server on Endpoint %d", SW1_ENDPOINT);
    APP_DBG("**********************************************************");
  }
}

/*************************************************************
 *
 * WRAP FUNCTIONS
 *
 *************************************************************/

void APP_ZIGBEE_RegisterCmdBuffer(TL_CmdPacket_t *p_buffer)
{
  p_ZIGBEE_otcmdbuffer = p_buffer;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)((TL_EvtPacket_t *)p_ZIGBEE_otcmdbuffer)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_NotificationPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_notif_M0_to_M4)->evtserial.evt.payload;
}

Zigbee_Cmd_Request_t * ZIGBEE_Get_M0RequestPayloadBuffer(void)
{
  return (Zigbee_Cmd_Request_t *)(p_ZIGBEE_request_M0_to_M4)->evtserial.evt.payload;
}

/**
 * @brief  This function is used to transfer the commands from the M4 to the M0.
 *
 * @param   None
 * @return  None
 */
void ZIGBEE_CmdTransfer(void)
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)p_ZIGBEE_otcmdbuffer->cmdserial.cmd.payload;

  /* Zigbee OT command cmdcode range 0x280 .. 0x3DF = 352 */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.cmdcode = 0x280U;
  /* Size = otCmdBuffer->Size (Number of OT cmd arguments : 1 arg = 32bits so multiply by 4 to get size in bytes)
   * + ID (4 bytes) + Size (4 bytes) */
  p_ZIGBEE_otcmdbuffer->cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_M0();
}

/**
 * @brief  This function is called when the M0+ acknowledge the fact that it has received a Cmd
 *
 *
 * @param   Otbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Otbuffer);

  Receive_Ack_From_M0();
}

/**
 * @brief  This function is called when notification from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
{
  p_ZIGBEE_notif_M0_to_M4 = Notbuffer;

  Receive_Notification_From_M0();
}

/**
 * @brief  This function is called before sending any ot command to the M0
 *         core. The purpose of this function is to be able to check if
 *         there are no notifications coming from the M0 core which are
 *         pending before sending a new ot command.
 * @param  None
 * @retval None
 */
void Pre_ZigbeeCmdProcessing(void)
{
  tx_mutex_get(&MtxZigbeeId, TX_WAIT_FOREVER);
}

/**
 * @brief  This function waits for getting an acknowledgment from the M0.
 *
 * @param  None
 * @retval None
 */
static void Wait_Getting_Ack_From_M0(void)
{
   tx_semaphore_get(&TransferToM0Semaphore, TX_WAIT_FOREVER);
   tx_mutex_put(&MtxZigbeeId);
}

/**
 * @brief  Receive an acknowledgment from the M0+ core.
 *         Each command send by the M4 to the M0 are acknowledged.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Ack_From_M0(void)
{
  tx_semaphore_put(&TransferToM0Semaphore);
}

/**
 * @brief  Receive a notification from the M0+ through the IPCC.
 *         This function is called under interrupt.
 * @param  None
 * @retval None
 */
static void Receive_Notification_From_M0(void)
{
  CptReceiveNotifyFromM0++;
  tx_semaphore_put(&NotifyM0ToM4Semaphore);
}

/**
 * @brief  This function is called when a request from M0+ is received.
 *
 * @param   Notbuffer : a pointer to TL_EvtPacket_t
 * @return  None
 */
void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
{
  p_ZIGBEE_request_M0_to_M4 = Reqbuffer;

  CptReceiveRequestFromM0++;
  tx_semaphore_put(&RequestM0ToM4Semaphore);
}

/**
 * @brief Perform initialization of TL for Zigbee.
 * @param  None
 * @retval None
 */
void APP_ZIGBEE_TL_INIT(void)
{
  ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&ZigbeeOtCmdBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)ZigbeeNotifRspEvtBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)ZigbeeNotifRequestBuffer;
  TL_ZIGBEE_Init(&ZigbeeConfigBuffer);
}

/**
 * @brief Process the messages coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessNotifyM0ToM4( ULONG argument )
{
  UNUSED(argument);

  for ( ;;)
  {
    tx_semaphore_get(&NotifyM0ToM4Semaphore, TX_WAIT_FOREVER);
    if (CptReceiveNotifyFromM0 != 0)
    {
      CptReceiveNotifyFromM0 = 0;
      Zigbee_CallBackProcessing();
    }
  }
}

/**
 * @brief Process the requests coming from the M0.
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_ProcessRequestM0ToM4( ULONG argument )
{
  UNUSED(argument);

  for ( ;;)
  {
    tx_semaphore_get(&RequestM0ToM4Semaphore, TX_WAIT_FOREVER);

    if (CptReceiveRequestFromM0 != 0)
    {
      CptReceiveRequestFromM0 = 0;
      Zigbee_M0RequestProcessing();
    }
  }
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

/* USER CODE END FD_LOCAL_FUNCTIONS */
