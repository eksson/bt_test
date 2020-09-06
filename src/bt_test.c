#include <string.h>
#include <bluetooth.h>
#include "bluetooth_internal.h"

#include "bt_main.h"
#include "bt_test.h"

#define KEYBOARD_RETURN			0x28	// Enter (Samsung TV)
#define KEYBOARD_ESCAPE			0x29	// Back (Samsung TV)
#define KEYBOARD_F5				0x3e	// Smart Hub (Samsung TV)
#define KEYBOARD_F6				0x3f	// Source (Samsung TV)
#define KEYBOARD_F7				0x40	// Channel List (Samsung TV)
#define KEYBOARD_F8				0x41	// Volume Mute (Samsung TV)
#define KEYBOARD_F9				0x42	// Volume Down (Samsung TV)
#define KEYBOARD_F10			0x43	// Volume Up (Samsung TV)
#define KEYBOARD_F11			0x44	// Channel Down (Samsung TV)
#define KEYBOARD_F12			0x45	// Channel Up (Samsung TV)
#define KEYBOARD_RIGHT			0x4f	// Arrow Right (Samsung TV)
#define KEYBOARD_LEFT			0x50	// Arrow Left (Samsung TV)
#define KEYBOARD_DOWN			0x51	// Arrow Down (Samsung TV)
#define KEYBOARD_UP				0x52	// Arrow Up (Samsung TV)

void
media_info_init()
{
	strncpy(media_info.title, "\0", 1);
	strncpy(media_info.artist, "\0", 1);
	strncpy(media_info.album, "\0", 1);
	strncpy(media_info.genre, "\0", 1);
	media_info.total_tracks = 0;
	media_info.number = 0;
	media_info.duration = 0;
}

int
bt_info_init(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_info_init() ===");

	int ret = bt_initialize();

	if ( ret != BT_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "bt_initialize() failed.");
		return ret;
	}

	ret = bt_adapter_get_state(&bt_state);
	if ( ret != BT_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "bt_adapter_get_state() failed.");
		return ret;
	}

	tv_hid_connect_auto = true;
	device_spk_connect_auto = BT_SPK_NOTCONNECTED;

	tv_hid_connected = BT_HID_NOTCONNECTED_POWERON;
	device_spk_connected = BT_SPK_NOTCONNECTED;

	device_avrcp_state_play_pause = false;

	media_info_init();

	// TV device registered by user
	strncpy(bt_device[0].bt_addr, "\0", 1);
	strncpy(bt_device[0].bt_name, "\0", 1);

	// 1st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[1].bt_addr, "AC:E0:10:8C:9F:8A", 18);
	strncpy(bt_device[1].bt_name, "notebook_00", strlen("notebook_00") + 1);

	// 2st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[2].bt_addr, "94:8B:C1:B2:90:BD", 18);
	strncpy(bt_device[2].bt_name, "mobile phone abcd",
			strlen("mobile phone abcd") + 1);

	// 3st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[3].bt_addr, "\0", 1);
	strncpy(bt_device[3].bt_name, "\0", 1);

	// Temporarily bonded BT device (Mobile, PC, MP3, etc)
	strncpy(bt_device[4].bt_addr, "\0", 1);
	strncpy(bt_device[4].bt_name, "\0", 1);

	return BT_ERROR_NONE;
}

void
__bt_device_bond_created_cb(int result,
		bt_device_info_s *device_info, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== __bt_device_bond_created_cb() ===");


}

void
__bt_hid_device_connection_state_changed_cb(int result,
		bool connected, const char *remote_address, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_hid_device_connection_state_changed_cb() ===");

	appdata_s *ad = user_data;

	if (connected == false) {
		tv_hid_connected = BT_HID_NOTCONNECTED;
	} else if (strstr(remote_address, bt_device[BT_SPK_CONNECTED_TV].bt_addr)
			|| strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) < 17) {
		bt_hid_device_disconnect(remote_address);
		tv_hid_connected = BT_HID_NOTCONNECTED;
	} else {
		tv_hid_connected = BT_HID_CONNECTED;
	}

	button_drow(ad);
}

void
__bt_audio_connection_state_changed_cb(int result, bool connected,
	const char *remote_address, bt_audio_profile_type_e type, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_audio_connection_state_changed_cb() ===");

	appdata_s *ad = user_data;
	int i;
	int ret = BT_ERROR_NONE;
	bt_device_info_s *device_info = NULL;

	if (connected == true && result == BT_ERROR_NONE) {

		if (device_spk_connected != BT_SPK_NOTCONNECTED) {
			ret = bt_audio_disconnect(bt_device[device_spk_connected].bt_addr,
					BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
			ret = bt_audio_disconnect(remote_address,
					BT_AUDIO_PROFILE_TYPE_A2DP_SINK);

			media_info_init();
			device_spk_connected = BT_SPK_NOTCONNECTED;

			button_drow(ad);

			return;
		}

		for (i = 0; i < MAX_BT_DEVICE - 1; i++) {
			if (strstr(bt_device[i].bt_addr, remote_address) != NULL) {
				device_spk_connected = i;
				break;
			}
		}

		if (i == MAX_BT_DEVICE - 1) {
			device_spk_connected = MAX_BT_DEVICE - 1;
			strncpy(bt_device[device_spk_connected].bt_addr, remote_address, 18);
			ret = bt_adapter_get_bonded_device_info(remote_address, &device_info);
			strncpy(bt_device[device_spk_connected].bt_name, device_info->remote_name,
					strlen(device_info->remote_name) + 1);
		}

		dlog_print(DLOG_DEBUG, LOG_TAG, "Connected : %s(%d), bt_name : %s",
				bt_device[device_spk_connected].bt_addr, device_spk_connected,
				bt_device[device_spk_connected].bt_name);

	} else {
		device_spk_connected = BT_SPK_NOTCONNECTED;
		dlog_print(DLOG_DEBUG, LOG_TAG, "Disconnected : %s", remote_address);
	}

	free(device_info);

	button_drow(ad);
}

static bool
__bt_adapter_hid_profile_connected_devices_cb(
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_adapter_hid_profile_connected_devices_cb() ===");

	dlog_print(DLOG_DEBUG, LOG_TAG, "HID remote_address : %s", remote_address);

	return true;
}

static bool
__bt_adapter_a2dp_profile_connected_devices_cb(
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_adapter_a2dp_profile_connected_devices_cb() ===");

	dlog_print(DLOG_DEBUG, LOG_TAG, "A2DP remote_address : %s", remote_address);

	return true;
}

void
__bt_avrcp_control_connection_state_changed_cb(bool connected,
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_avrcp_control_connection_state_changed_cb() ===");

}

bool
bt_tv_hid_connect(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_tv_hid_connect() ===");

	int ret;

	if (tv_hid_connected == BT_HID_CONNECTED) {
		ret = bt_hid_device_disconnect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
	} else {
		ret = bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
	}

	if (ret != BT_ATT_ERROR_NONE) {
		return false;
	}

	return true;
}

bool
bt_device_spk_connect(void *user_data, int connected_device)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== bt_device_spk_connect(), %d ===", connected_device);

	int ret;

	if (device_spk_connected == connected_device) {
		ret = bt_audio_disconnect(bt_device[device_spk_connected].bt_addr,
				BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	} else {
		ret = bt_audio_connect(bt_device[connected_device].bt_addr,
				BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	}

	if (ret != BT_ATT_ERROR_NONE) {
		return false;
	}

	return true;
}

void
__adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state,
		void* user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== adapter_state_changed_cb() ===");

	appdata_s *ad = user_data;

	if (result != BT_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG,
        		"[adapter_state_changed_cb] failed! result=%d", result);
        return;
    }

    if (adapter_state == BT_ADAPTER_ENABLED) {
    	bt_state = BT_ADAPTER_ENABLED;
    } else {
    	bt_state = BT_ADAPTER_DISABLED;
    	//Disconnect the all bonded devices

    	tv_hid_connected = BT_HID_NOTCONNECTED;
    	device_spk_connected = BT_SPK_NOTCONNECTED;
    	device_avrcp_state_play_pause = false;
    }

	button_drow(ad);
}

void
__bt_avrcp_player_state_changed_cb(bt_avrcp_player_state_e state,
		void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_avrcp_player_state_changed_cb() ===");

	appdata_s *ad = user_data;

	switch(state) {
		case BT_AVRCP_PLAYER_STATE_STOPPED:
		case BT_AVRCP_PLAYER_STATE_PAUSED:
			device_avrcp_state_play_pause = false;
			break;

		case BT_AVRCP_PLAYER_STATE_PLAYING:
			device_avrcp_state_play_pause = true;
			break;

		default:
			break;
	}

	button_drow(ad);
}

void
__bt_avrcp_track_info_changed_cb(
		bt_avrcp_metadata_attributes_info_s *metadata, void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== __bt_avrcp_track_info_changed_cb() ===");

	appdata_s *ad = user_data;

	strncpy(media_info.title, metadata->title, strlen(metadata->title) + 1);
	strncpy(media_info.artist, metadata->artist, strlen(metadata->artist) + 1);
	strncpy(media_info.album, metadata->album, strlen(metadata->album) + 1);
	strncpy(media_info.genre, metadata->genre, strlen(metadata->genre) + 1);
	media_info.total_tracks = metadata->total_tracks;
	media_info.number = metadata->number;
	media_info.duration = metadata->duration;

	button_drow(ad);
}

bool
bt_start_service(void *user_data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_start_service() ===");

	int ret = BT_ERROR_NONE;

	appdata_s *ad = user_data;

	ret = bt_audio_initialize();

	ret = bt_adapter_set_visibility(
			BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE, 0);

	ret = bt_audio_select_role(BT_A2DP_SINK);

	ret = bt_adapter_set_state_changed_cb(__adapter_state_changed_cb, ad);

	ret = bt_hid_device_activate(
				__bt_hid_device_connection_state_changed_cb, ad);

	ret = bt_audio_set_connection_state_changed_cb(
				__bt_audio_connection_state_changed_cb, ad);

	ret = bt_avrcp_control_initialize(
			__bt_avrcp_control_connection_state_changed_cb, user_data);

	ret = bt_avrcp_set_play_status_changed_cb(__bt_avrcp_player_state_changed_cb,
			user_data);

	ret = bt_avrcp_set_track_info_changed_cb(__bt_avrcp_track_info_changed_cb,
			user_data);

	ret = bt_adapter_foreach_profile_connected_devices(BT_PROFILE_SERVICE_UUID_HID,
			__bt_adapter_hid_profile_connected_devices_cb, user_data);

	ret = bt_adapter_foreach_profile_connected_devices(
			BT_PROFILE_SERVICE_UUID_A2DP_SOURCE,
			__bt_adapter_a2dp_profile_connected_devices_cb, user_data);

	bt_device_info_s *device_info = NULL;

	for (int i = 0; i < MAX_BT_DEVICE; i++) {
		if (strlen(bt_device[i].bt_addr) > 0) {
			ret = bt_adapter_get_bonded_device_info(bt_device[i].bt_addr,
					&device_info);
			if (ret == BT_ERROR_NONE) {
				ret = bt_hid_device_disconnect(bt_device[i].bt_addr);
				ret = bt_audio_disconnect(bt_device[i].bt_addr,
						BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
			}
		}
	}

	free(device_info);

	if (tv_hid_connect_auto == true &&
			strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) > 0) {
		bt_tv_hid_connect(ad);
	}

	if (device_spk_connect_auto > BT_SPK_NOTCONNECTED &&
			strlen(bt_device[device_spk_connect_auto].bt_addr) > 0) {
		ret = bt_audio_connect(bt_device[device_spk_connect_auto].bt_addr,
			BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	}

	return true;
}

void
bt_terminate(void)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_terminate() ===");

	bt_device_info_s *device_info = NULL;

	for (int i = 0; i < MAX_BT_DEVICE; i++) {
		if (strlen(bt_device[i].bt_addr) > 0) {
			bt_adapter_get_bonded_device_info(bt_device[i].bt_addr, &device_info);
			if (device_info->is_connected) {
				bt_hid_device_disconnect(bt_device[i].bt_addr);
				bt_audio_disconnect(bt_device[i].bt_addr,
						BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
			}
		}
	}

	free(device_info);
}

void
tv_hid_key_send(void *user_data, bt_tv_brand_e brand, bt_tv_key_e key)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== tv_hid_key_send(), brand : %d, key : %d ===", brand, key);

	bt_hid_key_data_s send_data;
	char pressedkey1[8] = { 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };
	char pressedkey2[8] = { 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00 };

	switch(brand) {
		case SAMSUNG_TV:
			switch(key) {
				case HID_RETURN:
					pressedkey1[0] = KEYBOARD_RETURN;
					break;
				case HID_ESCAPE:
					pressedkey1[0] = KEYBOARD_ESCAPE;
					break;
				case HID_SMARTKEY:
					pressedkey1[0] = KEYBOARD_F5;
					break;
				case HID_SOURCE:
					pressedkey1[0] = KEYBOARD_F6;
					break;
				case HID_CHANNEL_LIST:
					pressedkey1[0] = KEYBOARD_F7;
					break;
				case HID_VOLUME_MUTE:
					pressedkey1[0] = KEYBOARD_F8;
					break;
				case HID_VOLUME_DOWN:
					pressedkey1[0] = KEYBOARD_F9;
					break;
				case HID_VOLUME_UP:
					pressedkey1[0] = KEYBOARD_F10;
					break;
				case HID_CHANNEL_DOWN:
					pressedkey1[0] = KEYBOARD_F11;
					break;
				case HID_CHANNEL_UP:
					pressedkey1[0] = KEYBOARD_F12;
					break;
				case HID_ARROW_LEFT:
					pressedkey1[0] = KEYBOARD_LEFT;
					break;
				case HID_ARROW_RIGHT:
					pressedkey1[0] = KEYBOARD_RIGHT;
					break;
				case HID_ARROW_DOWN:
					pressedkey1[0] = KEYBOARD_DOWN;
					break;
				case HID_ARROW_UP:
					pressedkey1[0] = KEYBOARD_UP;
					break;
				default:
					break;
			}

			memcpy(send_data.key, pressedkey1, 8);
			send_data.modifier = 0;
			bt_hid_device_send_key_event(
					bt_device[BT_SPK_CONNECTED_TV].bt_addr, &send_data);

			memcpy(send_data.key, pressedkey2, 8);
			bt_hid_device_send_key_event(
					bt_device[BT_SPK_CONNECTED_TV].bt_addr, &send_data);
			break;

		case LG_TV:

			break;

		case ETC_TV:

			break;

		default:
			break;
	}
}

void
avrcp_message_send(void *user_data, avrcp_action_e action)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== avrcp_message_send(), action : %d ===", action);

	if (device_spk_connected < BT_SPK_CONNECTED_USER1) {
		return;
	}

	switch(action) {
		case AVRCP_ACTION_PLAY_PAUSE:
			if (device_avrcp_state_play_pause == false) {
				bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PLAY);
				device_avrcp_state_play_pause = true;
			} else {
				bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PAUSE);
				device_avrcp_state_play_pause = false;
			}
			break;

		case AVRCP_ACTION_PREV:
			bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PREVIOUS);
			break;

		case AVRCP_ACTION_NEXT:
			bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_NEXT);
			break;

		case AVRCP_ACTION_VOLUME_DOWN:
			bt_avrcp_control_decrease_volume();
			break;

		case AVRCP_ACTION_VOLUME_UP:
			bt_avrcp_control_increase_volume();
			break;

		default:

			break;
	}
}
