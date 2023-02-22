/*-----------------------------------------------------------------------------
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Creative Common CC BY-NC 4.0 2022 : ANDREAS SANDSMARK BAKKE, BIPIN THAPA, HEBA MAHMOUD MOHAMED EMAD, HASSAN
 *----------------------------------------------------------------------------*/

#include "watertank.h"

int inValve,
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

int prevInValve,
    prevOutValve,
    prevCurrentLevel,
    prevSensorReading,
    prevDetection,
    prevControllerMode;

int hourlyOutFlowPercentage[24] =
    {70,
     50,
     38,
     30,
     38,
     57,
     115,
     140,
     144,
     135,
     118,
     102,
     100,
     98,
     98,
     98,
     105,
     120,
     140,
     148,
     142,
     130,
     110,
     80};

int hourlyOutFlow[24] = {0};

uint8_t *
    val[4];

void tostring3(uint8_t *str, uint16_t num)
{
    str[3] = '\0';
    str[2] = num % 10 + 48;
    num /= 10;
    str[1] = num % 10 + 48;
    num /= 10;
    str[0] = num % 10 + 48;

    return;
}
void tostring2(uint8_t *str, uint16_t num)
{
    str[2] = '\0';
    str[1] = num % 10 + 48;
    num /= 10;
    str[0] = num % 10 + 48;
    return;
}

void LCD_Clear_Window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                      uint16_t color)
{
    for (int i = 0; i < y1 - y0 + 1; ++i)
    {
        LCD_DrawLine(x0, y0 + i, x1, y0 + i, color);
    }
}

void DrawTankGUI()
{
    inRate = DEFAULT_IN_RATE;
    for (int i = 0; i < 24; ++i)
    {
        hourlyOutFlow[i] = ((inRate * hourlyOutFlowPercentage[i]) / 100);
    }
    hour = 0,
    inValve = 0,
    outValve = 0,
    outValveLimit = 0,
    outRate = hourlyOutFlow[hour],
    currentLevel = TANK_BORDER_BOTTOM,
    sensorReading = currentLevel,
    numTolerated = 0,
    numDetected = 0,
    numFailed = 0,
    numHR = 0, numGD = 0,
    executionTime = 0,
    status = 0,
    controller_mode = 0,
    detection = 0,
    floatSwitch = 0;

    LCD_Clear(WHITE);
    /**parameters for system **/

    // tank border upper
    LCD_DrawLine(START_X, TANK_BORDER_TOP, TANK_BORDER_RIGHT, TANK_BORDER_TOP, BLACK);
    LCD_DrawLine(TANK_BORDER_RIGHT, TANK_BORDER_TOP, TANK_BORDER_RIGHT,
                 TANK_BORDER_BOTTOM - PIPE_HEIGHT, BLACK);
    LCD_DrawLine(TANK_BORDER_RIGHT, TANK_BORDER_BOTTOM - PIPE_HEIGHT,
                 TANK_BORDER_RIGHT + PIPE_LENGTH, TANK_BORDER_BOTTOM - PIPE_HEIGHT, BLACK);
    // tank border lower
    LCD_DrawLine(START_X, TANK_BORDER_TOP + PIPE_HEIGHT, TANK_BORDER_LEFT,
                 TANK_BORDER_TOP + PIPE_HEIGHT, BLACK);
    LCD_DrawLine(TANK_BORDER_LEFT, START_Y + PIPE_HEIGHT, TANK_BORDER_LEFT,
                 TANK_BORDER_BOTTOM, BLACK);
    LCD_DrawLine(TANK_BORDER_LEFT, TANK_BORDER_BOTTOM, TANK_BORDER_RIGHT + PIPE_LENGTH,
                 TANK_BORDER_BOTTOM, BLACK);

    // labels
    LCD_DrawLine(LABEL_LINE_START, DESIRED_LEVEL, LABEL_LINE_END, DESIRED_LEVEL, GREEN);                 // desired level
    LCD_DrawLine(LABEL_LINE_START, THRESHOLD_HIGH, LABEL_LINE_END, THRESHOLD_HIGH, DARKORANGE);          // threshold warning above
    LCD_DrawLine(LABEL_LINE_START, THRESHOLD_LOW, LABEL_LINE_END, THRESHOLD_LOW, DARKORANGE);            // threshold warning below
    LCD_DrawLine(LABEL_LINE_START, START_Y + 1, LABEL_LINE_END, START_Y + 1, RED);                       // threshold fail top
    LCD_DrawLine(LABEL_LINE_START, TANK_BORDER_BOTTOM - 1, LABEL_LINE_END, TANK_BORDER_BOTTOM - 1, RED); // threshold fail bottom

    // pipes initial states
    LCD_Clear_Window(START_X, TANK_BORDER_TOP + 1, TANK_BORDER_LEFT,
                     START_Y + PIPE_HEIGHT - 1,
                     BLUE); // inpipe ready
    LCD_DrawLine(TANK_BORDER_LEFT, TANK_BORDER_TOP + 1, TANK_BORDER_LEFT,
                 TANK_BORDER_TOP + PIPE_HEIGHT,
                 RED); // close inpipe
    LCD_DrawLine(TANK_BORDER_RIGHT, TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1,
                 TANK_BORDER_RIGHT, TANK_BORDER_BOTTOM - 1, RED); // close outpipe

    /*
        GUI_Text(0 + 8 * 8, 10, "000", BLACK, WHITE); // sensor delay
        GUI_Text(0 + 8 * 8, 30, "000", BLACK, WHITE); // sensor
        GUI_Text(0 + 8 * 8, 50, "000", BLACK, WHITE); // actual
        GUI_Text(0 + 8 * 5, 80, "000", BLACK, WHITE);
        GUI_Text(0 + 8 * 10 + 8 * 6, 80, "000", BLACK, WHITE);
        GUI_Text(0 + 8 * 10 * 2 + 8 * 6, 80, "000", BLACK, WHITE);
        GUI_Text(200, 0, "000", BLACK, WHITE);  // warnings
        GUI_Text(200, 20, "000", BLACK, WHITE); // fails
        GUI_Text(200, 40, "000", BLACK, WHITE); // deadlines
        GUI_Text(200, 60, "000", BLACK, WHITE); // bitFlips
    */

    GUI_Text(120 - 5 * 12 / 2, 15 * 0, "00:00", BLACK, WHITE);

    GUI_Text(5, 15 * 1, "SENSOR: 000", BLACK, WHITE);
    GUI_Text(5, 15 * 2, "ACTUAL: 000", BLACK, WHITE);

    GUI_Text(5, 15 * 4, "       NORMAL       ", WHITE, BLUE);
    GUI_Text(5, 15 * 5, " INRATE: 00", BLACK, WHITE);
    GUI_Text(5, 15 * 6, "OUTRATE: 00", BLACK, WHITE);

    GUI_Text(240 - 8 * 9 - 5, 15 * 1, "DETECTION", WHITE, RED);
    GUI_Text(240 - 8 * 7 - 5, 15 * 2, "ET: 000", BLACK, WHITE);
    GUI_Text(240 - 8 * 7 - 5, 15 * 3, "#D: 000", BLACK, WHITE);
    GUI_Text(240 - 8 * 7 - 5, 15 * 4, "#T: 000", BLACK, WHITE);
    GUI_Text(240 - 8 * 7 - 5, 15 * 5, "GD: 000", BLACK, WHITE);
    GUI_Text(240 - 8 * 7 - 5, 15 * 6, "HR: 000", BLACK, WHITE);

    GUI_Text(240 - 8 * 7 - 5, 15 * 8, "#F: 000", BLACK, WHITE);

    prevInValve = inValve,
    prevOutValve = outValve,
    prevCurrentLevel = currentLevel,
    prevSensorReading = sensorReading,
    prevDetection = detection,
    prevControllerMode = controller_mode;
}

void UpdateTankGUI()
{
    if (prevCurrentLevel ^ currentLevel)
    {
        floatSwitch = 0;
        if (currentLevel >= TANK_BORDER_BOTTOM - 1)
        {
            currentLevel = TANK_BORDER_BOTTOM - 1;
            floatSwitch = 1;
            if (status != 1)
            {
                GUI_Text(160, 175, " EMPTY ", BLACK, RED);
                ++numFailed;
            }
            status = 1;
        }
        else if (currentLevel >= THRESHOLD_LOW)
        {
            floatSwitch = 1;
            if (status != 2)
                GUI_Text(160, 175, "  LOW  ", BLACK, YELLOW);
            status = 2;
        }
        else if (currentLevel <= TANK_BORDER_TOP + 1)
        {
            currentLevel = TANK_BORDER_TOP + 1;
            if (status != 4)
                GUI_Text(160, 175, " FULL  ", BLACK, RED);
            status = 4;
        }
        else if (currentLevel <= THRESHOLD_HIGH)
        {
            if (status != 3)
                GUI_Text(160, 175, " HIGH  ", BLACK, YELLOW);
            status = 3;
        }
        else
        {
            if (status != 5)
                GUI_Text(160, 175, " OKAY  ", BLACK, GREEN);
            status = 5;
        }
    }

    if (prevCurrentLevel < currentLevel)
    {
        LCD_Clear_Window(prevInValve ? TANK_BORDER_LEFT + 1 + PIPE_LENGTH / 2 + 1 : TANK_BORDER_LEFT + 1, prevCurrentLevel, TANK_BORDER_RIGHT - 1, currentLevel - 1, WHITE);
    }
    else if (prevCurrentLevel > currentLevel)
    {
        LCD_Clear_Window(TANK_BORDER_LEFT + 1, currentLevel, TANK_BORDER_RIGHT - 1, prevCurrentLevel - 1, BLUE);
    }

    // if (prevInValve ^ inValve)
    {
        if (inValve)
        {
            LCD_DrawLine(TANK_BORDER_LEFT, TANK_BORDER_TOP + 1, TANK_BORDER_LEFT,
                         START_Y + PIPE_HEIGHT, BLUE); // open pipe
            LCD_Clear_Window(TANK_BORDER_LEFT + 1, TANK_BORDER_TOP + 1,
                             TANK_BORDER_LEFT + 1 + PIPE_LENGTH / 2, currentLevel - 1,
                             BLUE); // fill
        }
        else if (!inValve)
        {
            LCD_DrawLine(TANK_BORDER_LEFT, TANK_BORDER_TOP + 1, TANK_BORDER_LEFT,
                         TANK_BORDER_TOP + PIPE_HEIGHT,
                         RED); // close pipe
            LCD_Clear_Window(TANK_BORDER_LEFT + 1, START_Y + 2,
                             TANK_BORDER_LEFT + 1 + 10, currentLevel - 1, WHITE); // no fill
        }
    }

    // if (prevOutValve ^ outValve || currentLevel >= TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1)
    {
        if (outValve)
        {
            if (currentLevel < TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1)
                LCD_Clear_Window(TANK_BORDER_RIGHT,
                                 TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1,
                                 TANK_BORDER_RIGHT + PIPE_LENGTH, TANK_BORDER_BOTTOM - 1,
                                 BLUE);
            else
            {
                LCD_Clear_Window(TANK_BORDER_RIGHT,
                                 TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1,
                                 TANK_BORDER_RIGHT + PIPE_LENGTH, currentLevel - 1,
                                 WHITE);
                LCD_Clear_Window(TANK_BORDER_RIGHT, currentLevel,
                                 TANK_BORDER_RIGHT + PIPE_LENGTH, TANK_BORDER_BOTTOM - 1,
                                 BLUE);
            }
        }
        else if (!outValve)
        {
            LCD_DrawLine(TANK_BORDER_RIGHT, TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1,
                         TANK_BORDER_RIGHT, TANK_BORDER_BOTTOM - 1, RED);
            LCD_Clear_Window(TANK_BORDER_RIGHT + 1,
                             TANK_BORDER_BOTTOM - PIPE_HEIGHT + 1,
                             TANK_BORDER_RIGHT + PIPE_LENGTH, TANK_BORDER_BOTTOM - 1,
                             WHITE);
        }
    }

    LCD_DrawLine(LABEL_LINE_START, DESIRED_LEVEL, LABEL_LINE_END, DESIRED_LEVEL,
                 GREEN); // desired level
    LCD_DrawLine(LABEL_LINE_START, THRESHOLD_HIGH, LABEL_LINE_END,
                 THRESHOLD_HIGH,
                 DARKORANGE); // threshold warning above
    LCD_DrawLine(LABEL_LINE_START, THRESHOLD_LOW, LABEL_LINE_END,
                 THRESHOLD_LOW,
                 DARKORANGE);                                                      // threshold warning below
    LCD_DrawLine(LABEL_LINE_START, START_Y + 1, LABEL_LINE_END, START_Y + 1, RED); // threshold fail top
    LCD_DrawLine(LABEL_LINE_START, TANK_BORDER_BOTTOM - 1, LABEL_LINE_END,
                 TANK_BORDER_BOTTOM - 1,
                 RED); // threshold fail bottom

    tostring2((uint8_t *)val, hour);
    GUI_Text(120 - 5 * 12 / 2, 0, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, sensorReading);
    GUI_Text(5 + 8 * 8, 15 * 1, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, currentLevel);
    GUI_Text(5 + 8 * 8, 15 * 2, (uint8_t *)val, BLACK, WHITE);

    if (prevControllerMode ^ controller_mode)
    {
        if (controller_mode == NORMAL_MODE)
        {
            GUI_Text(5, 15 * 4, "       NORMAL       ", WHITE, BLUE);
        }
        if (controller_mode == GRACEFUL_DEGRADATION_MODE)
        {
            GUI_Text(5, 15 * 4, "GRACEFUL_DEGRADATION", BLACK, YELLOW);
        }
        if (controller_mode == HARD_RECOVERY_MODE)
        {
            GUI_Text(5, 15 * 4, "   HARD_RECOVERY    ", WHITE, RED);
        }
    }

    tostring3((uint8_t *)val, inValve ? inRate : 0);
    GUI_Text(5 + 8 * 9, 15 * 5, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, outValve ? outRate : 0);
    GUI_Text(5 + 8 * 9, 15 * 6, (uint8_t *)val, BLACK, WHITE);

    if (prevDetection ^ detection)
    {
        GUI_Text(240 - 8 * 9 - 5, 15 * 1, "DETECTION", WHITE, detection ? GREEN : RED);
    }
    tostring3((uint8_t *)val, executionTime);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 2, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, numDetected);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 3, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, numTolerated);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 4, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, numGD);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 5, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, numHR);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 6, (uint8_t *)val, BLACK, WHITE);
    tostring3((uint8_t *)val, numFailed);
    GUI_Text(240 - 8 * 7 - 5 + 8 * 4, 15 * 8, (uint8_t *)val, RED, WHITE);

    if (prevSensorReading ^ sensorReading)
    {
        if (prevSensorReading > TANK_BORDER_BOTTOM)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, TANK_BORDER_BOTTOM + 4,
                         TANK_BORDER_RIGHT - 1 - 15, TANK_BORDER_BOTTOM + 4, WHITE);
        }
        else if (prevSensorReading == TANK_BORDER_BOTTOM || prevSensorReading == TANK_BORDER_TOP)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, prevSensorReading,
                         TANK_BORDER_RIGHT - 1 - 15, prevSensorReading, BLACK);
        }
        else if (prevSensorReading >= currentLevel)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, prevSensorReading,
                         TANK_BORDER_RIGHT - 1 - 15, prevSensorReading, BLUE);
        }
        else if (prevSensorReading < TANK_BORDER_TOP)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, TANK_BORDER_TOP - 4,
                         TANK_BORDER_RIGHT - 1 - 15, TANK_BORDER_TOP - 4, WHITE);
        }
        else
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, prevSensorReading,
                         TANK_BORDER_RIGHT - 1 - 15, prevSensorReading, WHITE);
        }

        if (sensorReading < TANK_BORDER_TOP)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, TANK_BORDER_TOP - 4,
                         TANK_BORDER_RIGHT - 1 - 15, TANK_BORDER_TOP - 4, RED);
        }
        else if (sensorReading > TANK_BORDER_BOTTOM)
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, TANK_BORDER_BOTTOM + 4,
                         TANK_BORDER_RIGHT - 1 - 15, TANK_BORDER_BOTTOM + 4,
                         RED);
        }
        else
        {
            LCD_DrawLine(TANK_BORDER_LEFT + 1 + 15, sensorReading,
                         TANK_BORDER_RIGHT - 1 - 15, sensorReading,
                         BLACK);
        }
        // prevSensorReading = sensorReading;
    }

    prevInValve = inValve,
    prevOutValve = outValve,
    prevCurrentLevel = currentLevel,
    prevSensorReading = sensorReading,
    prevDetection = detection,
    prevControllerMode = controller_mode;
}

void FillTank()
{
    outValve = 0;
    while (currentLevel > THRESHOLD_HIGH)
    {
        inValve = 1;
        currentLevel = currentLevel + outValve * outRate - inValve * inRate;
        UpdateTankGUI();
        hour = 0;
    }
    inValve = 0;
    outValve = 1;
}

void StartTank()
{
    outValve = 1;
}

void IncrementHour(void)
{
    hour = (hour + 1) % 24;
}
