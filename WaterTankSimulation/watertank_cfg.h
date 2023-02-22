/*-----------------------------------------------------------------------------
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Creative Common CC BY-NC 4.0 2022 : ANDREAS SANDSMARK BAKKE, BIPIN THAPA, HEBA MAHMOUD MOHAMED EMAD, HASSAN
 *----------------------------------------------------------------------------*/

#define DEFAULT_IN_RATE 							5
#define DESIRED_LEVEL 								180
#define DELTA 										40
#define THRESHOLD_HIGH 								DESIRED_LEVEL - DELTA
#define THRESHOLD_LOW 								DESIRED_LEVEL + DELTA

#define THRESHOLD_WARNING 						    15
#define THRESHOLD_DETECTION 					    25
#define THRESHOLD_WARNING_MAX_COUNT 	            3
