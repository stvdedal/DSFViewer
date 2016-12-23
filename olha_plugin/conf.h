#ifndef CONF_H_
#define CONF_H_

#define PLUGIN_NAME "Olha"
#define PLUGIN_SIG  "stvdedal.yak-42.olha"
#define PLUGIN_DESC "Navigation device olha-1"

#define DSF_ROOT_DIR "Global Scenery\\X-Plane 10 Global Scenery\\Earth nav data"
#define TMP_DIR "C:\\Windows\\Temp"

const int MAP_TEXTURE_BUTTOM = 10;
const int MAP_TEXTURE_LEFT = 818;
const int MAP_TEXTURE_WIDTH = 2043 - MAP_TEXTURE_LEFT;
const int MAP_TEXTURE_HEIGHT = 1236 - MAP_TEXTURE_BUTTOM;

#define MAP_WIDTH_CM        15.4
#define MAP_HEIGHT_CM       10.7

#define DREF_DISTANCE_TO_NEAREST_AIROPORT   "stvdedal/yak-42/olha/distance_nearest_airoport"
#define DREF_LED_NEAREST_AIROPORT_APPROACH  "stvdedal/yak-42/olha/led_nearest_airoport"

// 0 : overview
// 1 : route
// 2 : airoport
#define DREF_MAP_TABLET_MODE "stvdedal/yak-42/olha/map_tablet_mode"

// 0 : lighting off
// 1 : lighting on
#define DREF_MAP_TABLET_LIT "stvdedal/yak-42/olha/map_tablet_lit"


// Breaked HZ

// breaked sim/cockpit2/radios/actuators/adf1_left_frequency_hz
// Multiple 100 KHz
#define DREF_ADF1_LEFT_FREQUENCY_HZ_1   "stvdedal/yak-42/adf1_left_frequency_hz_1"
// Multiple 10 KHz
#define DREF_ADF1_LEFT_FREQUENCY_HZ_2   "stvdedal/yak-42/adf1_left_frequency_hz_2"
// Multiple 0.5 KHz
#define DREF_ADF1_LEFT_FREQUENCY_HZ_3   "stvdedal/yak-42/adf1_left_frequency_hz_3"

// breaked sim/cockpit2/radios/actuators/adf1_right_frequency_hz
// Multiple 100 KHz
#define DREF_ADF1_RIGHT_FREQUENCY_HZ_1   "stvdedal/yak-42/adf1_right_frequency_hz_1"
// Multiple 10 KHz
#define DREF_ADF1_RIGHT_FREQUENCY_HZ_2   "stvdedal/yak-42/adf1_right_frequency_hz_2"
// Multiple 0.5 KHz
#define DREF_ADF1_RIGHT_FREQUENCY_HZ_3   "stvdedal/yak-42/adf1_right_frequency_hz_3"

// breaked sim/cockpit2/radios/actuators/adf2_left_frequency_hz
// Multiple 100 KHz
#define DREF_ADF2_LEFT_FREQUENCY_HZ_1   "stvdedal/yak-42/adf2_left_frequency_hz_1"
// Multiple 10 KHz
#define DREF_ADF2_LEFT_FREQUENCY_HZ_2   "stvdedal/yak-42/adf2_left_frequency_hz_2"
// Multiple 0.5 KHz
#define DREF_ADF2_LEFT_FREQUENCY_HZ_3   "stvdedal/yak-42/adf2_left_frequency_hz_3"

// breaked sim/cockpit2/radios/actuators/adf2_right_frequency_hz
// Multiple 100 KHz
#define DREF_ADF2_RIGHT_FREQUENCY_HZ_1   "stvdedal/yak-42/adf2_right_frequency_hz_1"
// Multiple 10 KHz
#define DREF_ADF2_RIGHT_FREQUENCY_HZ_2   "stvdedal/yak-42/adf2_right_frequency_hz_2"
// Multiple 0.5 KHz
#define DREF_ADF2_RIGHT_FREQUENCY_HZ_3   "stvdedal/yak-42/adf2_right_frequency_hz_3"


// RMI

// 0 : Left ADF (ADF1) selected
// 1 : Right ADF (ADF2) selected
#define DREF_RMI_LEFT_POINTER_1_SELECTOR    "stvdedal/yak-42/rmi_left_pointer_1_selector"
#define DREF_RMI_LEFT_POINTER_2_SELECTOR    "stvdedal/yak-42/rmi_left_pointer_2_selector"
#define DREF_RMI_RIGHT_POINTER_1_SELECTOR   "stvdedal/yak-42/rmi_right_pointer_1_selector"
#define DREF_RMI_RIGHT_POINTER_2_SELECTOR   "stvdedal/yak-42/rmi_right_pointer_2_selector"

#define DREF_RMI_LEFT_POINTER_1_DEG         "stvdedal/yak-42/rmi_left_pointer_1_deg"
#define DREF_RMI_LEFT_POINTER_2_DEG         "stvdedal/yak-42/rmi_left_pointer_2_deg"
#define DREF_RMI_RIGHT_POINTER_1_DEG        "stvdedal/yak-42/rmi_right_pointer_1_deg"
#define DREF_RMI_RIGHT_POINTER_2_DEG        "stvdedal/yak-42/rmi_right_pointer_2_deg"


#endif
