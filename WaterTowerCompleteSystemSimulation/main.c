/*-------------------------------------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: simulate a water tower system and demonstrate fault injection, fault detection and recovery
 * Note(s):"This material was originally developed as part of an assignment of the Operating systems for embedded systems course delivered at Politecnico di Torino by Prof. Stefano Di Carlo".
 *-------------------------------------------------------------------------------------------------------
 * Copyright 2022 ANDREAS SANDSMARK BAKKE, BIPIN THAPA, HEBA MAHMOUD MOHAMED EMAD HASSAN
 *
 * Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://creativecommons.org/licenses/by-nc/4.0/
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 *-----------------------------------------------------------------------------------------------------
 */


#include "LPC17xx.H" /* LPC17xx definitions                */
#include "core_cm3.h"
#include "RTE_Device.h" // Keil::Device:Startup
#include "includes.h"
#include "BSP.h"

#include "watertank.h"

int down_KEY1;
int down_KEY2;
int down_KEY3;

/* ----------------- APPLICATION GLOBALS ------------------ */

/*Task Control Blocks */
static OS_TCB APP_TSK_TCB;
static OS_TCB GUI_TSK_TCB;
static OS_TCB CONTROLLER_TSK_TCB;
static OS_TCB DETECTOR_TSK_TCB;

/* Task Stacks */
static CPU_STK_SIZE APP_TSK_STACK[APP_CFG_TASK_STK_SIZE];
static CPU_STK_SIZE GUI_TSK_STACK[APP_CFG_TASK_STK_SIZE];
static CPU_STK_SIZE CONTROLLER_TSK_STACK[APP_CFG_TASK_STK_SIZE];
static CPU_STK_SIZE DETECTOR_TSK_STACK[APP_CFG_TASK_STK_SIZE];

/* Timers to start tasks*/
OS_TMR timer_periodic_RT_GUI_DETECTOR;
OS_TMR timer_periodic_CONTROLLER;

/* Application globals */
int bitFlipToggle = 0;
int signal = 0;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/* Generic function */
static int tostring(uint8_t *str, int num);

/* Tasks */
static void APP_TSK(void *p_arg);
static void GUI_TSK(void *p_arg);
static void CONTROLLER_TSK(void *p_arg);
static void DETECTOR_TSK(void *p_arg);

/* Timer callbacks */
static void Wake_Task_GUI_DETECTOR(void);
static void Wake_Task_CONTROLLER(void);

/**********************************************************************************************************
 *                                                main()
 *
 * Description : This is the standard entry point for C code.  It is assumed that your code will call
 *               main() once you have performed all necessary initialization.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : This the main standard entry point.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

int main(void)
{
  OS_ERR err;

  SystemInit();  /*initialize the clock */
  BSP_IntInit(); /*initiialize interrupt*/
  BSP_Start();   /*initialize kernel system tick timer */
  CPU_IntDis();  /*disable interrupt*/
  CPU_Init();    /*init cpu*/
  Mem_Init();    /*Memory initialization*/

  OSInit(&err); /* Initialize "uC/OS-III, The Real-Time Kernel"         */

  /* create timers for tasks */
  OSTmrCreate(
      &timer_periodic_RT_GUI_DETECTOR,
      "timer_periodic_RT_GUI_DETECTOR",
      0,
      1,
      OS_OPT_TMR_PERIODIC,
      (OS_TMR_CALLBACK_PTR)Wake_Task_GUI_DETECTOR,
      (void *)0,
      &err);

  OSTmrCreate(
      &timer_periodic_CONTROLLER,
      "timer_periodic_CONTROLLER",
      0,
      2,
      OS_OPT_TMR_PERIODIC,
      (OS_TMR_CALLBACK_PTR)Wake_Task_CONTROLLER,
      (void *)0,
      &err);

  OSTaskCreate((OS_TCB *)&APP_TSK_TCB, /* Create the start task                                */
               (CPU_CHAR *)"APP_TASK",
               (OS_TASK_PTR)APP_TSK,
               (void *)0,
               (OS_PRIO)APP_CFG_TASK_PRIO,
               (CPU_STK *)&APP_TSK_STACK[0],
               (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE,
               (OS_MSG_QTY)0,
               (OS_TICK)0,
               (void *)0,
               (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR *)&err);

  OSStart(&err); /* Start multitasking (i.e. give control to uC/OS-III). */

  while (DEF_ON)
  { /* Should Never Get Here	*/
  };
}

static int tostring(uint8_t *str, int num)
{
  int i, rem, len = 0, n;
  // handling the case 0
  if (num == 0)
  {
    str[0] = '0';
    str[1] = '\0';
    return 1;
  }
  n = num;
  while (n != 0)
  {
    len++;
    n /= 10;
  }
  for (i = 0; i < len; i++)
  {
    rem = num % 10;
    num = num / 10;
    str[len - (i + 1)] = rem + '0';
  }
  str[len] = '\0';
  return len - 1;
}

static void APP_TSK(void *p_arg)
{
  (void)p_arg; /* See Note #1                                          */
  OS_ERR os_err;

  BSP_OS_TickEnable(); /* Enable the tick timer and interrupt                  */
  LED_init();
  LCD_Initialization();

  // ADC_init();
  joystick_init();

  BUTTON_init();

  // PWM_init();
  // PWM_enable();

  OS_ERR err;
  DrawTankGUI();
  FillTank();

  OSTaskCreate((OS_TCB *)&CONTROLLER_TSK_TCB,
               (CPU_CHAR *)"CONTROLLER_TSK",
               (OS_TASK_PTR)CONTROLLER_TSK,
               (void *)0,
               (OS_PRIO)CONTROLLER_CFG_TASK_PRIO,
               (CPU_STK *)&CONTROLLER_TSK_STACK[0],
               (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE,
               (OS_MSG_QTY)0,
               (OS_TICK)0,
               (void *)0,
               (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR *)&err);

  OSTaskCreate((OS_TCB *)&DETECTOR_TSK_TCB,
               (CPU_CHAR *)"DETECTOR_TSK",
               (OS_TASK_PTR)DETECTOR_TSK,
               (void *)0,
               (OS_PRIO)DETECTOR_CFG_TASK_PRIO,
               (CPU_STK *)&DETECTOR_TSK_STACK[0],
               (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE,
               (OS_MSG_QTY)0,
               (OS_TICK)0,
               (void *)0,
               (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR *)&err);

  OSTaskCreate((OS_TCB *)&GUI_TSK_TCB,
               (CPU_CHAR *)"GUI_TSK",
               (OS_TASK_PTR)GUI_TSK,
               (void *)0,
               (OS_PRIO)GUI_CFG_TASK_PRIO,
               (CPU_STK *)&GUI_TSK_STACK[0],
               (CPU_STK)(APP_CFG_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE)APP_CFG_TASK_STK_SIZE,
               (OS_MSG_QTY)0,
               (OS_TICK)0,
               (void *)0,
               (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (OS_ERR *)&err);

  StartTank();
  OSTmrStart(&timer_periodic_RT_GUI_DETECTOR, &os_err);
  OSTmrStart(&timer_periodic_CONTROLLER, &os_err);

  while (DEF_TRUE)
  {
    OSSchedLock(&os_err);
    UpdateTankGUI();
    OSSchedUnlock(&os_err);

    if ((LPC_GPIO1->FIOPIN & (1 << 29)) == 0) // up
    {
      bitFlipToggle = 1;
    }
    if ((LPC_GPIO1->FIOPIN & (1 << 27)) == 0) // left
    {
      inValve = 0;
    }
    if ((LPC_GPIO1->FIOPIN & (1 << 28)) == 0) // right
    {
      outValve = outValve ^ 1;
    }
    if ((LPC_GPIO1->FIOPIN & (1 << 26)) == 0) // down
    {
      bitFlipToggle = 0;
    }

    if (bitFlipToggle)
    {
      sensorReading ^= (1 << (rand() % 9));
    }

    if (down_KEY1 == 1) // key 1
    {
      down_KEY1 = 0;
      detection = 1;
    }

    if (down_KEY2 == 1) // key 2
    {
      down_KEY2 = 0;
      detection = 0;
    }
    if (down_KEY3 == 1)
    {
      down_KEY3 = 0;
    }
  }
}

static void Wake_Task_GUI_DETECTOR(void)
{
  OS_ERR err;
  OSTaskResume(&GUI_TSK_TCB, &err);
  if (detection)
  {
    OSTaskResume(&DETECTOR_TSK_TCB, &err);
  }
}

static void GUI_TSK(void *p_args)
{
  OS_ERR os_err;
  (void)p_args;

  while (DEF_TRUE)
  {
    OSTaskSuspend((void *)0, &os_err);

    currentLevel = currentLevel + outValve * outRate - inValve * inRate;
    {
      hour = (hour + 1) % 24;
      outRate = (outValveLimit && (hourlyOutFlow[hour] > outValveLimit)) ? outValveLimit : hourlyOutFlow[hour];
    }
  }
}

static void DETECTOR_TSK(void *p_arg)
{
  OS_ERR os_err;
  (void)p_arg;

  int counting = 0;
  int TW_Counter = 0;

  while (DEF_TRUE)
  {
    OSTaskSuspend((void *)0, &os_err);
    if (controller_mode == NORMAL_MODE)
    {
      if (floatSwitch)
      {
        if (counting)
        {
          executionTime++;
        }
        if (!counting)
        {
          counting = 1;
          executionTime = 0;
        }
      }

      if (counting && !floatSwitch)
      {
        if (executionTime > THRESHOLD_WARNING)
        {
          TW_Counter++;
        }
        else
        {
          numTolerated += TW_Counter;
          TW_Counter = 0;
        }
        counting = 0;
      }

      if (executionTime > THRESHOLD_DETECTION)
      {
        ++numDetected;
        signal = ALARM;
        ++numHR;
        counting = 0;
        executionTime = 0;
        TW_Counter = 0;
        Wake_Task_CONTROLLER();
      }
      else if (TW_Counter > THRESHOLD_WARNING_MAX_COUNT)
      {
        ++numDetected;
        signal = WARN;
        ++numGD;

        counting = 0;
        executionTime = 0;
        TW_Counter = 0;
        Wake_Task_CONTROLLER();
      }
      else
      {
        signal = 0;
      }
    }
    else if (currentLevel < THRESHOLD_LOW)
    {
      signal = 0;
    }
  }
}

static void Wake_Task_CONTROLLER(void)
{
  OS_ERR err;
  OSTaskResume(&CONTROLLER_TSK_TCB, &err);
}

static void CONTROLLER_TSK(void *p_args)
{
  OS_ERR os_err;
  (void)p_args;

  while (DEF_TRUE)
  {
    sensorReading = currentLevel;
    OSTaskSuspend((void *)0, &os_err);
    controller_mode = signal;

    inRate = DEFAULT_IN_RATE;
    outValveLimit = 0;
    if (controller_mode == GRACEFUL_DEGRADATION_MODE)
    {
      sensorReading = currentLevel;
    }
    if (controller_mode == HARD_RECOVERY_MODE)
    {
      sensorReading = currentLevel;
      inRate = (DEFAULT_IN_RATE * 120) / 100;
      outValveLimit = inRate;
    }
    if (sensorReading >= THRESHOLD_LOW)
    {
      inValve = 1;
    }
    else if (sensorReading < THRESHOLD_HIGH)
    {
      inValve = 0;
    }
  }
}
