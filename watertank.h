/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: simulate a water tower system and demonstrate fault injection, fault detection and recovery
 * Note(s):"This material was originally developed as part of an assignment of the Operating systems for embedded systems course delivered at Politecnico di Torino by Prof. Stefano Di Carlo".
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Creative Common CC BY-NC 4.0 2022 	ANDREAS SANDSMARK BAKKE, BIPIN THAPA, HEBA MAHMOUD MOHAMED EMAD, HASSAN
 *----------------------------------------------------------------------------*/

#include "includes.h"
#include "watertank_cfg.h"

#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define DARKORANGE 0XFC60

#define START_X 10
#define START_Y 120
#define PIPE_LENGTH 20
#define PIPE_HEIGHT 20
#define TANK_DIAMETER 100
#define TANK_BORDER_TOP START_Y
#define TANK_BORDER_BOTTOM 310
#define TANK_BORDER_LEFT START_X + PIPE_LENGTH
#define TANK_BORDER_RIGHT TANK_BORDER_LEFT + TANK_DIAMETER
#define LABEL_LINE_START START_X - 5
#define LABEL_LINE_END TANK_BORDER_RIGHT + PIPE_LENGTH + 5

extern int inValve,
    outValve,
    outValveLimit,
    inRate,
    outRate,
    currentLevel,
    sensorReading,
    numTolerated,
    numDetected,
    numFailed,
    numHR, numGD,
    executionTime,
    status,
    hour,
    controller_mode,
    detection,
    floatSwitch;

extern int hourlyOutFlowPercentage[24], hourlyOutFlow[24];
extern uint8_t *val[4];

void DrawTankGUI(void);
void LCD_Clear_Window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void tostring3(uint8_t *str, uint16_t num);
void tostring2(uint8_t *str, uint16_t num);
void UpdateTankGUI(void);
void FillTank(void);
void StartTank(void);
void IncrementHour(void);
