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

#define HID_AUTO_CONNECT_WAIT_TIME		30	//seconds

static const char *
__bt_get_error_message(bt_error_e err)
{
	const char *err_str = NULL;

	switch (err) {
	case BT_ERROR_NONE:
		err_str = "BT_ERROR_NONE";
		break;
	case BT_ERROR_CANCELLED:
		err_str = "BT_ERROR_CANCELLED";
		break;
	case BT_ERROR_INVALID_PARAMETER:
		err_str = "BT_ERROR_INVALID_PARAMETER";
		break;
	case BT_ERROR_OUT_OF_MEMORY:
		err_str = "BT_ERROR_OUT_OF_MEMORY";
		break;
	case BT_ERROR_RESOURCE_BUSY:
		err_str = "BT_ERROR_RESOURCE_BUSY";
		break;
	case BT_ERROR_TIMED_OUT:
		err_str = "BT_ERROR_TIMED_OUT";
		break;
	case BT_ERROR_NOW_IN_PROGRESS:
		err_str = "BT_ERROR_NOW_IN_PROGRESS";
		break;
	case BT_ERROR_NOT_INITIALIZED:
		err_str = "BT_ERROR_NOT_INITIALIZED";
		break;
	case BT_ERROR_NOT_ENABLED:
		err_str = "BT_ERROR_NOT_ENABLED";
		break;
	case BT_ERROR_ALREADY_DONE:
		err_str = "BT_ERROR_ALREADY_DONE";
		break;
	case BT_ERROR_OPERATION_FAILED:
		err_str = "BT_ERROR_OPERATION_FAILED";
		break;
	case BT_ERROR_NOT_IN_PROGRESS:
		err_str = "BT_ERROR_NOT_IN_PROGRESS";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_BONDED:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_BONDED";
		break;
	case BT_ERROR_AUTH_REJECTED:
		err_str = "BT_ERROR_AUTH_REJECTED";
		break;
	case BT_ERROR_AUTH_FAILED:
		err_str = "BT_ERROR_AUTH_FAILED";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_FOUND:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_FOUND";
		break;
	case BT_ERROR_SERVICE_SEARCH_FAILED:
		err_str = "BT_ERROR_SERVICE_SEARCH_FAILED";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED";
		break;
	case BT_ERROR_PERMISSION_DENIED:
		err_str = "BT_ERROR_PERMISSION_DENIED";
		break;
	case BT_ERROR_SERVICE_NOT_FOUND:
		err_str = "BT_ERROR_SERVICE_NOT_FOUND";
		break;
	case BT_ERROR_NO_DATA:
		err_str = "BT_ERROR_NO_DATA";
		break;
	case BT_ERROR_NOT_SUPPORTED:
		err_str = "BT_ERROR_NOT_SUPPORTED";
		break;
	case BT_ERROR_DEVICE_POLICY_RESTRICTION:
		err_str = "DEVICE_POLICY_RESTRICTION";
		break;
	default:
		err_str = "NOT Defined";
		break;
	}

	return err_str;
}

void
svrcp_media_info_init()
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
bt_info_init()
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== bt_info_init() ===");

	int ret = bt_initialize();

	if ( ret != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "bt_initialize() failed.");
		return ret;
	}

	ret = bt_adapter_get_state(&bt_state);
	if ( ret != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "bt_adapter_get_state() failed.");
		return ret;
	}

	tv_hid_connect_auto = true;
	device_spk_connect_auto = BT_SPK_NOTCONNECTED;
	tv_power_offed = true;

	tv_hid_connected = BT_HID_NOTCONNECTED;
	device_spk_connected = BT_SPK_NOTCONNECTED;

	device_avrcp_state_play_pause = false;

	svrcp_media_info_init();

	// TV device registered by user
	strncpy(bt_device[0].bt_addr, "CC:6E:A4:54:CC:83", 18);
	strncpy(bt_device[0].bt_name, "[TV] Samsung TV", strlen("[TV] Samsung TV") + 1);

	// 1st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[1].bt_addr, "F8:E6:1A:85:B8:BC", 18);
	strncpy(bt_device[1].bt_name, "Mobile Z3", strlen("Mobile Z3") + 1);

	// 2st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[2].bt_addr, "94:8B:C1:B2:90:BD", 18);
	strncpy(bt_device[2].bt_name, "Mobile Galaxy", strlen("Mobile Galaxy") + 1);

	// 3st device registered by user (Mobile, PC, MP3, etc)
	strncpy(bt_device[3].bt_addr, "\0", 1);
	strncpy(bt_device[3].bt_name, "\0", 1);

	// Temporarily bonded BT device (Mobile, PC, MP3, etc)
	strncpy(bt_device[4].bt_addr, "\0", 1);
	strncpy(bt_device[4].bt_name, "\0", 1);

	return BT_ERROR_NONE;
}

void __bt_device_bond_created_cb(int result,
		bt_device_info_s *device_info, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== __bt_device_bond_created_cb() ===");


}

Eina_Bool
hid_connected_try_n_get_state(void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== hid_connected_try_n_get_state() ===");

	if (tv_hid_connected == BT_HID_CONNECTED) {
		return ECORE_CALLBACK_CANCEL;
	}

	bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);

	if (tv_hid_connected == BT_HID_NOTCONNECTED) {
		return ECORE_CALLBACK_RENEW;
	}

	return ECORE_CALLBACK_CANCEL;
}

void
__bt_hid_device_connection_state_changed_cb(int result,
		bool connected, const char *remote_address, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_hid_device_connection_state_changed_cb() ===");
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"result : %d, connected : %d, remote_address : %s",
			result, connected, remote_address);

	appdata_s *ad = user_data;

	if (result != BT_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"HID_CONNECT fail(maybe TV power-off) : %s",
				__bt_get_error_message(result));

		if (connected == true) {
			bt_hid_device_disconnect(remote_address);
		}
		tv_hid_connected = BT_HID_NOTCONNECTED;

		return;
	}

	if (connected == false) {
		if (strstr(bt_device[BT_SPK_CONNECTED_TV].bt_addr,
				remote_address) != NULL) {
			tv_hid_connected = BT_HID_NOTCONNECTED;

			if (tv_power_offed == true) {
				ecore_timer_add(HID_AUTO_CONNECT_WAIT_TIME,
						(void *)hid_connected_try_n_get_state, ad);
			}

		} else {
			if (tv_hid_connected == BT_HID_CONNECTED) {
				bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
				tv_hid_connected = BT_HID_CONNECTED;
			}
		}

	} else if (strstr(bt_device[BT_SPK_CONNECTED_TV].bt_addr,
			remote_address) == NULL) {
		bt_hid_device_disconnect(remote_address);

		if (strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) == 17
				&& tv_hid_connected == BT_HID_CONNECTED) {
			bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
			tv_hid_connected = BT_HID_CONNECTED;
		}

	} else {
		tv_hid_connected = BT_HID_CONNECTED;
	}

	button_drow(ad);
}


void
__bt_audio_connection_state_changed_cb(int result, bool connected,
	const char *remote_address, bt_audio_profile_type_e type, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_audio_connection_state_changed_cb() ===");
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"result : %d, connected : %d, remote_address : %s",
			result, connected, remote_address);

	appdata_s *ad = user_data;

	bt_device_info_s *device_info = NULL;
	int i;
	int ret;

	if (result != BT_ERROR_NONE) {

		dlog_print(DLOG_ERROR, LOG_TAG, "AUDIO_CONNECT_ERROR : %s",
				__bt_get_error_message(result));

		for (i = 0; i < MAX_BT_DEVICE; i++) {
			if (strlen(bt_device[i].bt_addr) == 17) {
				ret = bt_adapter_get_bonded_device_info(bt_device[i].bt_addr,
						&device_info);
				if (ret == BT_ERROR_NONE) {
					ret = bt_audio_disconnect(bt_device[i].bt_addr,
							BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
				} else {
					dlog_print(DLOG_ERROR, LOG_TAG,
							"bt_adapter_get_bonded_device_info() fail : %s",
							__bt_get_error_message(ret));
				}
			}
		}

		free(device_info);

		return;
	}

	if (connected == true) {

		if (device_spk_connected != BT_SPK_NOTCONNECTED) {
			bt_audio_disconnect(bt_device[device_spk_connected].bt_addr,
					BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
			bt_audio_disconnect(remote_address,
					BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
		} else {

			for (i = 0; i < MAX_BT_DEVICE - 1; i++) {
				if (strstr(bt_device[i].bt_addr, remote_address) != NULL) {
					device_spk_connected = i;
					break;
				}
			}

			if (i == MAX_BT_DEVICE - 1) {
				device_spk_connected = MAX_BT_DEVICE - 1;
				strncpy(bt_device[device_spk_connected].bt_addr,
						remote_address, 18);
				bt_adapter_get_bonded_device_info(remote_address,
						&device_info);
				strncpy(bt_device[device_spk_connected].bt_name,
						device_info->remote_name,
						strlen(device_info->remote_name) + 1);
			}

			dlog_print(DLOG_DEBUG, LOG_TAG, "Connected : %s(%d), bt_name : %s",
					bt_device[device_spk_connected].bt_addr, device_spk_connected,
					bt_device[device_spk_connected].bt_name);
		}

	} else {
		device_spk_connected = BT_SPK_NOTCONNECTED;
	}

	free(device_info);

	button_drow(ad);
}


static bool
__bt_adapter_hid_profile_connected_devices_cb(
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_adapter_hid_profile_connected_devices_cb() ===");

	bt_hid_device_disconnect(remote_address);
	dlog_print(DLOG_DEBUG, LOG_TAG, "HID Disconnect : %s", remote_address);

	return true;
}

static bool
__bt_adapter_a2dp_profile_connected_devices_cb(
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_adapter_a2dp_profile_connected_devices_cb() ===");

	bt_audio_disconnect(remote_address, BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	dlog_print(DLOG_DEBUG, LOG_TAG, "A2DP Disconnect : %s", remote_address);

	return true;
}

static bool
__bt_adapter_avrcp_profile_connected_devices_cb(
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_adapter_avrcp_profile_connected_devices_cb() ===");

	bt_avrcp_control_disconnect(remote_address);
	dlog_print(DLOG_DEBUG, LOG_TAG, "AVRCP Disconnect : %s", remote_address);

	return true;
}

void
__bt_avrcp_control_connection_state_changed_cb(bool connected,
	const char *remote_address, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== __bt_avrcp_control_connection_state_changed_cb() ===");

	dlog_print(DLOG_DEBUG, LOG_TAG, "connected : %d, remote_address : %s",
			connected, remote_address);

	svrcp_media_info_init();
}

bool
bt_tv_hid_connect(void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== bt_tv_hid_connect() ===");

	int ret = BT_ERROR_NONE;

	if (strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) < 17) {
		return false;
	}

	if (tv_hid_connected == BT_HID_CONNECTED) {
		ret = bt_hid_device_disconnect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
		tv_power_offed = false;
	} else {
		if (strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) == 17) {
			ret = bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
			tv_power_offed = true;
		}
	}

	if (ret != BT_ERROR_NONE) {
		return false;
	}
	return true;
}

bool
bt_device_spk_connect(void *user_data, int connected_device)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== bt_device_spk_connect(), %d ===", connected_device);

	int ret;

	if (device_spk_connected == connected_device) {
		ret = bt_audio_disconnect(bt_device[device_spk_connected].bt_addr,
				BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	} else {
		ret = bt_audio_connect(bt_device[connected_device].bt_addr,
				BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	}

	if (ret != BT_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG, "bt_device_spk_connect() failed : %s",
				__bt_get_error_message(ret));
		return false;
	}

	return true;
}

void
bt_device_auto_connect(void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"Auto connection Setting : HID(%d), Audio(%d)",
			tv_hid_connect_auto, device_spk_connect_auto);

	appdata_s *ad = user_data;

	int ret;

	if (tv_hid_connect_auto == true && tv_power_offed == true &&
			strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) == 17) {

		ret = bt_hid_device_connect(bt_device[BT_SPK_CONNECTED_TV].bt_addr);
		ecore_timer_add(HID_AUTO_CONNECT_WAIT_TIME,
				(void *)hid_connected_try_n_get_state, ad);
	}

	if (device_spk_connect_auto > BT_SPK_NOTCONNECTED &&
			strlen(bt_device[device_spk_connect_auto].bt_addr) == 17) {
		bt_audio_connect(bt_device[device_spk_connect_auto].bt_addr,
			BT_AUDIO_PROFILE_TYPE_A2DP_SINK);
	}
}

void
__adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state,
		void* user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== adapter_state_changed_cb() ===");

	appdata_s *ad = user_data;

	if (result != BT_ERROR_NONE) {
        dlog_print(DLOG_ERROR, LOG_TAG,
        		"__adapter_state_changed_cb() failed : %s",
				__bt_get_error_message(result));
        return;
    }

    if (adapter_state == BT_ADAPTER_ENABLED) {
    	bt_state = BT_ADAPTER_ENABLED;
    	tv_power_offed = true;
    	bt_device_auto_connect(ad);
    } else {
    	bt_state = BT_ADAPTER_DISABLED;
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
	dlog_print(DLOG_INFO, LOG_TAG,
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
	dlog_print(DLOG_INFO, LOG_TAG,
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

void
adapter_visibility_mode_changed_cb(int result,
		bt_adapter_visibility_mode_e visibility_mode, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG,
			"=== adapter_visibility_mode_changed_cb() ===");

    if (result != BT_ERROR_NONE) {
        return;
    }

    if (visibility_mode == BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE) {
        dlog_print(DLOG_DEBUG, LOG_TAG, "None discoverable mode!");
        bt_adapter_set_visibility(
        			BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE, 0);
    } else if (visibility_mode
    		== BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE) {
        dlog_print(DLOG_DEBUG, LOG_TAG, "General discoverable mode!");
    } else {
        dlog_print(DLOG_DEBUG, LOG_TAG, "Limited discoverable mode!");
    }
}

void
bt_device_all_disconnect(void *user_data)
{
	appdata_s *ad = user_data;

	bt_adapter_foreach_profile_connected_devices(BT_PROFILE_SERVICE_UUID_HID,
			__bt_adapter_hid_profile_connected_devices_cb, ad);

	bt_adapter_foreach_profile_connected_devices(
			BT_PROFILE_SERVICE_UUID_A2DP_SOURCE,
			__bt_adapter_a2dp_profile_connected_devices_cb, ad);

	bt_adapter_foreach_profile_connected_devices(
			BT_PROFILE_SERVICE_UUID_AVRCP_TARGET,
			__bt_adapter_avrcp_profile_connected_devices_cb, ad);
}

bool
bt_start_service(void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== bt_start_service() ===");

	int ret = BT_ERROR_NONE;

	appdata_s *ad = user_data;

	ret = bt_audio_initialize();

	ret = bt_adapter_set_visibility_mode_changed_cb(
			adapter_visibility_mode_changed_cb, ad);

	ret = bt_adapter_set_visibility(
			BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE, 0);

	ret = bt_audio_select_role(BT_A2DP_SINK);

	ret = bt_device_set_bond_created_cb(__bt_device_bond_created_cb, ad);

	ret = bt_adapter_set_state_changed_cb(__adapter_state_changed_cb, ad);

	ret = bt_audio_set_connection_state_changed_cb(
			__bt_audio_connection_state_changed_cb, ad);

	ret = bt_hid_device_activate(
			__bt_hid_device_connection_state_changed_cb, ad);

	ret = bt_avrcp_control_initialize(
			__bt_avrcp_control_connection_state_changed_cb, ad);

	ret = bt_avrcp_set_play_status_changed_cb(
			__bt_avrcp_player_state_changed_cb, ad);

	ret = bt_avrcp_set_track_info_changed_cb(
			__bt_avrcp_track_info_changed_cb, ad);

	bt_device_all_disconnect(ad);

	bt_device_auto_connect(ad);

	return true;
}

void
bt_terminate(void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "=== bt_terminate() ===");

	appdata_s *ad = user_data;

	bt_device_all_disconnect(ad);

	bt_hid_device_deactivate();
	bt_deinitialize();
}

void
tv_hid_key_send(void *user_data, bt_tv_brand_e brand, bt_tv_key_e key)
{
	dlog_print(DLOG_INFO, LOG_TAG,
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
	dlog_print(DLOG_INFO, LOG_TAG,
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
