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

/**controller modes**/
#define NORMAL_MODE 0
#define GRACEFUL_DEGRADATION_MODE 1
#define HARD_RECOVERY_MODE 2

/**detection signals**/
#define WARN 1
#define ALARM 2

#define DEFAULT_IN_RATE 5
#define DESIRED_LEVEL 180
#define DELTA 40
#define THRESHOLD_HIGH DESIRED_LEVEL - DELTA

/**comparision threshold for the controller and float switch **/
#define THRESHOLD_LOW DESIRED_LEVEL + DELTA

/**implementation thresholds**/
#define THRESHOLD_WARNING 15
#define THRESHOLD_DETECTION 25
#define THRESHOLD_WARNING_MAX_COUNT 3
