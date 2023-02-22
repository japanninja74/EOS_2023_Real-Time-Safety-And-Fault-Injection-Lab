/*-------------------------------------------------------------------------------------------------------
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
 
#define DEFAULT_IN_RATE 							5
#define DESIRED_LEVEL 								180
#define DELTA 										40
#define THRESHOLD_HIGH 								DESIRED_LEVEL - DELTA
#define THRESHOLD_LOW 								DESIRED_LEVEL + DELTA

#define THRESHOLD_WARNING 						    15
#define THRESHOLD_DETECTION 					    25
#define THRESHOLD_WARNING_MAX_COUNT 	            3
