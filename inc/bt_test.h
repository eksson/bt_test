#include <bluetooth.h>
#include "bluetooth_internal.h"

#ifndef BT_TEST_H_
#define BT_TEST_H_

#define MAX_BT_DEVICE	5

typedef struct BT_Device {
	char bt_addr[18];
	char bt_name[256];
} bt_device_h;

// -1 : Power ON and not bonded, 0 : Not bonded(but try to bond), 1 : Bonded
typedef enum {
	BT_HID_NOTCONNECTED_POWERON = -1,
	BT_HID_NOTCONNECTED = 0,
	BT_HID_CONNECTED = 1
} bt_hid_bonded_state_e;

// -1 : Not bonded, 0 : TV, 1 : User1, 2 : User2, 3 : User3
typedef enum {
	BT_SPK_NOTCONNECTED = -1,
	BT_SPK_CONNECTED_TV = 0,
	BT_SPK_CONNECTED_USER1,
	BT_SPK_CONNECTED_USER2,
	BT_SPK_CONNECTED_USER3,
	BT_SPK_CONNECTED_TEMP
} bt_speaker_bonded_state_e;

typedef enum {
	SAMSUNG_TV = 0,
	LG_TV,
	ETC_TV,
} bt_tv_brand_e;

typedef enum {
	HID_RETURN = 0,
	HID_ESCAPE,
	HID_SMARTKEY,
	HID_SOURCE,
	HID_CHANNEL_LIST,
	HID_VOLUME_MUTE,
	HID_VOLUME_DOWN,
	HID_VOLUME_UP,
	HID_CHANNEL_DOWN,
	HID_CHANNEL_UP,
	HID_ARROW_RIGHT,
	HID_ARROW_LEFT,
	HID_ARROW_DOWN,
	HID_ARROW_UP
} bt_tv_key_e;

typedef enum {
	AVRCP_ACTION_PLAY_PAUSE = 1,
	AVRCP_ACTION_PREV,
	AVRCP_ACTION_NEXT,
	AVRCP_ACTION_VOLUME_DOWN,
	AVRCP_ACTION_VOLUME_UP
} avrcp_action_e;

typedef struct {
	char title[256];   /**< Title */
	char artist[256];   /**< Artist */
	char album[256];   /**< Album name */
	char genre[256];   /**< Album Genre */
	unsigned int total_tracks;   /**< The total number of tracks */
	unsigned int number;   /**< Track number */
	unsigned int duration;   /**< Duration */
} bt_media_info_s;

bt_adapter_state_e bt_state;
bt_device_h bt_device[MAX_BT_DEVICE];

bool tv_hid_connect_auto;
int tv_hid_connected;

int device_spk_connect_auto;
int device_spk_connected;
bool device_avrcp_state_play_pause;

bt_media_info_s media_info;

int bt_info_init(void *user_data);
bool bt_start_service(void *user_data);
void bt_terminate(void);
bool bt_tv_hid_connect(void *user_data);
bool bt_device_spk_connect(void *user_data, int connected_device);
void tv_hid_key_send(void *user_data, bt_tv_brand_e brand, bt_tv_key_e key);
void avrcp_message_send(void *user_data, avrcp_action_e action);


#endif /* BT_TEST_H_ */
