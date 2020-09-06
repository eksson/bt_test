#include "bluetooth_internal.h"

#include "bt_main.h"
#include "bt_test.h"

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

#if 0
static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}
#endif

static void
button_tv_hid_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_tv_hid_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) > 0) {
		bt_tv_hid_connect(ad);
		button_drow(ad);
	}
}

// Add the HID_KEY functions
//

static void
button_tv_hid_volume_up_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== bt_tv_hid_volume_up_clicked_cb() ===");

	appdata_s *ad = data;

	if (tv_hid_connected != BT_HID_CONNECTED) {
		return;
	}

	tv_hid_key_send(ad, SAMSUNG_TV, HID_VOLUME_UP);
}

//
// Add the HID_KEY functions

static void
button_spk_tv_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_spk_tv_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_TV].bt_addr) > 0) {
		bt_device_spk_connect(ad, BT_SPK_CONNECTED_TV);
		button_drow(ad);
	}
}

static void
button_spk_user1_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_spk_user1_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_USER1].bt_addr) > 0) {
		bt_device_spk_connect(ad, BT_SPK_CONNECTED_USER1);
		button_drow(ad);
	}
}

static void
button_spk_user2_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_spk_user2_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_USER2].bt_addr) > 0) {
		bt_device_spk_connect(ad, BT_SPK_CONNECTED_USER2);
		button_drow(ad);
	}
}

static void
button_spk_user3_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_spk_user3_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_USER3].bt_addr) > 0) {
		bt_device_spk_connect(ad, BT_SPK_CONNECTED_USER3);
		button_drow(ad);
	}
}

static void
button_spk_temp_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== bt_spk_user3_clicked_cb() ===");

	appdata_s *ad = data;

	if(strlen(bt_device[BT_SPK_CONNECTED_TEMP].bt_addr) > 0) {
		bt_device_spk_connect(ad, BT_SPK_CONNECTED_TEMP);
		button_drow(ad);
	}
}

static void
button_spk_volume_up_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== bt_spk_volume_up_clicked_cb() ===");

	appdata_s *ad = data;

	if (tv_hid_connected != BT_HID_CONNECTED
			&& device_spk_connected == BT_SPK_NOTCONNECTED) {
		return;
	}

	if (tv_hid_connected == BT_HID_CONNECTED
			|| device_spk_connected == BT_SPK_CONNECTED_TV) {
		tv_hid_key_send(ad, SAMSUNG_TV, HID_VOLUME_UP);
		return;
	} else {
		avrcp_message_send(ad, AVRCP_ACTION_VOLUME_UP);
	}
}

static void
button_avrcp_play_pause_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== bt_avrcp_play_pause_clicked_cb() ===");

	appdata_s *ad = data;

	if (device_spk_connected < BT_SPK_CONNECTED_USER1) {
		return;
	}

	avrcp_message_send(ad, AVRCP_ACTION_PLAY_PAUSE);

	button_drow(ad);
}

static void
button_avrcp_next_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG,
			"=== bt_avrcp_play_pause_clicked_cb() ===");

	appdata_s *ad = data;

	if (device_spk_connected < BT_SPK_CONNECTED_USER1) {
		return;
	}

	avrcp_message_send(ad, AVRCP_ACTION_NEXT);
}

static void
add_content_in_box(Evas_Object *box, Evas_Object *content)
{
	evas_object_size_hint_weight_set(content, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(content, -1.0, -1.0);
	elm_box_pack_end(box, content);
	evas_object_show(content);
}

void
button_drow(void *data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== button_drow() ===");

	appdata_s *ad = data;

	if (bt_state == BT_ADAPTER_ENABLED) {
		elm_object_text_set(ad->lbl_info,
			_("<font_size=20><color=#FF4500FF><align=center>Bluetooth state : \
					ON</align></color></font_size>"));
	} else {
		elm_object_text_set(ad->lbl_info,
			_("<font_size=20><align=center>Bluetooth state : \
					OFF</align></font_size>"));
	}

	char button_txt[1000];

	/* Button-1 */
	if (tv_hid_connected == BT_HID_CONNECTED
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_hid_tv, EINA_FALSE);
		strncpy(button_txt, "<color=#FF4500FF>TV Remocon (",
				strlen("<color=#FF4500FF>TV Remocon (") + 1);
		strncat(button_txt, bt_device[BT_SPK_CONNECTED_TV].bt_name,
				strlen(bt_device[BT_SPK_CONNECTED_TV].bt_name) + 1);
		strncat(button_txt, ") : Connected</color>",
				strlen(") : Connected</color>") + 1);
		elm_object_text_set(ad->bt_hid_tv, button_txt);
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_hid_tv, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_hid_tv, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_hid_tv, _("TV Remocon : Disconnected"));
	}

	/* Button-2 */
	if (tv_hid_connected == BT_HID_CONNECTED
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_hid_volume_up, EINA_FALSE);
		elm_object_text_set(ad->bt_hid_volume_up,
				_("<color=#FF4500FF>TV volume UP</color>"));
	} else {
		if (bt_state == BT_ADAPTER_DISABLED
				|| tv_hid_connected != BT_HID_CONNECTED) {
			elm_object_disabled_set(ad->bt_hid_volume_up, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_hid_volume_up, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_hid_volume_up, _("TV volume UP"));
	}

	/* Button-3 */
	if (device_spk_connected == BT_SPK_CONNECTED_TV
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_spk_tv, EINA_FALSE);
		elm_object_text_set(ad->bt_spk_tv,
				_("<color=#FF4500FF>TV Speaker Connected</color>"));
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_spk_tv, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_tv, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_tv, _("TV Speaker Disconnected"));
	}

	/* Button-4 */
	if (device_spk_connected == BT_SPK_CONNECTED_USER1
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_spk_user1, EINA_FALSE);
		strncpy(button_txt, "<color=#FF4500FF>Device1 (",
				strlen("<color=#FF4500FF>Device1 (") + 1);
		strncat(button_txt, bt_device[BT_SPK_CONNECTED_USER1].bt_name,
				strlen(bt_device[BT_SPK_CONNECTED_USER1].bt_name) + 1);
		strncat(button_txt, ") : Connected</color>",
				strlen(") : Connected</color>") + 1);
		elm_object_text_set(ad->bt_spk_user1, button_txt);
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_spk_user1, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_user1, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_user1, _("Device1 : Disconnected"));
	}

	/* Button-5 */
	if (device_spk_connected == BT_SPK_CONNECTED_USER2
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_spk_user2, EINA_FALSE);
		strncpy(button_txt, "<color=#FF4500FF>Device2 (",
				strlen("<color=#FF4500FF>Device2 (") + 1);
		strncat(button_txt, bt_device[BT_SPK_CONNECTED_USER2].bt_name,
				strlen(bt_device[BT_SPK_CONNECTED_USER2].bt_name) + 1);
		strncat(button_txt, ") : Connected</color>",
				strlen(") : Connected</color>") + 1);
		elm_object_text_set(ad->bt_spk_user2, button_txt);
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_spk_user2, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_user2, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_user2, _("Device2 : Disconnected"));
	}

	/* Button-6 */
	if (device_spk_connected == BT_SPK_CONNECTED_USER3
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_spk_user3, EINA_FALSE);
		strncpy(button_txt, "<color=#FF4500FF>Device3 (",
				strlen("<color=#FF4500FF>Device3 (") + 1);
		strncat(button_txt, bt_device[BT_SPK_CONNECTED_USER3].bt_name,
				strlen(bt_device[BT_SPK_CONNECTED_USER3].bt_name) + 1);
		strncat(button_txt, ") : Connected</color>",
				strlen(") : Connected</color>") + 1);
		elm_object_text_set(ad->bt_spk_user3, button_txt);
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_spk_user3, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_user3, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_user3, _("Device3 : Disconnected"));
	}

	/* Button-7 */
	if (device_spk_connected == BT_SPK_CONNECTED_TEMP
			&& bt_state == BT_ADAPTER_ENABLED) {
		elm_object_disabled_set(ad->bt_spk_temp, EINA_FALSE);
		strncpy(button_txt, "<color=#FF4500FF>Temp (",
				strlen("<color=#FF4500FF>Temp (") + 1);
		strncat(button_txt, bt_device[BT_SPK_CONNECTED_TEMP].bt_name,
				strlen(bt_device[BT_SPK_CONNECTED_TEMP].bt_name) + 1);
		strncat(button_txt, ") : Connected</color>",
				strlen(") : Connected</color>") + 1);
		elm_object_text_set(ad->bt_spk_temp, button_txt);
	} else {
		if (bt_state == BT_ADAPTER_DISABLED) {
			elm_object_disabled_set(ad->bt_spk_temp, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_temp, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_temp, _("Temp : Disconnected"));
	}

	/* Button-8 */
	if (device_spk_connected >= 0) {
		elm_object_disabled_set(ad->bt_spk_volume_up, EINA_FALSE);
		elm_object_text_set(ad->bt_spk_volume_up,
				_("<color=#FF4500FF>TV/BT Volume up</color>"));
	} else {
		if (bt_state == BT_ADAPTER_DISABLED
				|| device_spk_connected == BT_SPK_NOTCONNECTED) {
			elm_object_disabled_set(ad->bt_spk_volume_up, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_spk_volume_up, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_spk_volume_up,
				_("TV/BT Volume up : Cannot use"));
	}

	/* Button-9 */
	if (device_spk_connected >= 1) {
		elm_object_disabled_set(ad->bt_avrcp_play_pause, EINA_FALSE);
		if ( device_avrcp_state_play_pause == true ) {
			strncpy(button_txt, "<color=#FF4500FF>Music/Video (",
					strlen("<color=#FF4500FF>Music/Video (") + 1);
			strncat(button_txt, media_info.title,
					strlen(media_info.title) + 1);
			strncat(button_txt, ") : Playing > Pause</color>",
					strlen(") : Playing > Pause</color>") + 1);
			elm_object_text_set(ad->bt_avrcp_play_pause, button_txt);
		} else {
			strncpy(button_txt, "<color=#FF4500FF>Music/Video(",
					strlen("<color=#FF4500FF>Music/Video(") + 1);
			strncat(button_txt, media_info.title,
					strlen(media_info.title) + 1);
			strncat(button_txt, ") : Stopping > Play</color>",
					strlen(") : Stopping > Play</color>") + 1);
			elm_object_text_set(ad->bt_avrcp_play_pause, button_txt);
		}
	} else {
		if (bt_state == BT_ADAPTER_DISABLED
				|| device_spk_connected == BT_SPK_NOTCONNECTED
				|| device_spk_connected == 0) {
			elm_object_disabled_set(ad->bt_avrcp_play_pause, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_avrcp_play_pause, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_avrcp_play_pause,
				_("Music/Video () : Stopping > Play : Cannot use"));
	}

	/* Button-10 */
	if (device_spk_connected >= 1) {
		elm_object_disabled_set(ad->bt_avrcp_next, EINA_FALSE);
		elm_object_text_set(ad->bt_avrcp_next,
				_("<color=#FF4500FF>Music/Video : Next</color>"));
	} else {
		if (bt_state == BT_ADAPTER_DISABLED
				|| device_spk_connected == BT_SPK_NOTCONNECTED
				|| device_spk_connected == 0) {
			elm_object_disabled_set(ad->bt_avrcp_next, EINA_TRUE);
		} else {
			elm_object_disabled_set(ad->bt_avrcp_next, EINA_FALSE);
		}
		elm_object_text_set(ad->bt_avrcp_next,
				_("Music/Video : Next, Cannot use"));
	}
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== create_base_gui() ===");

	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win,
				(const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request",
			win_delete_request_cb, NULL);

#if 0
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK,
			win_back_cb, ad);
#endif

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Box */
	ad->box = elm_box_add(ad->win);
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->box);

	/* Label-1 */
	ad->lbl_info = elm_label_add(ad->conform);
	ad->bt_hid_tv = elm_button_add(ad->conform);
	ad->bt_hid_volume_up = elm_button_add(ad->conform);
	ad->bt_spk_tv = elm_button_add(ad->conform);
	ad->bt_spk_user1 = elm_button_add(ad->conform);
	ad->bt_spk_user2 = elm_button_add(ad->conform);
	ad->bt_spk_user3 = elm_button_add(ad->conform);
	ad->bt_spk_temp = elm_button_add(ad->conform);
	ad->bt_spk_volume_up = elm_button_add(ad->conform);
	ad->bt_avrcp_play_pause = elm_button_add(ad->conform);
	ad->bt_avrcp_next = elm_button_add(ad->conform);

	add_content_in_box(ad->box, ad->lbl_info);
	add_content_in_box(ad->box, ad->bt_hid_tv);
	add_content_in_box(ad->box, ad->bt_hid_volume_up);
	add_content_in_box(ad->box, ad->bt_spk_tv);
	add_content_in_box(ad->box, ad->bt_spk_user1);
	add_content_in_box(ad->box, ad->bt_spk_user2);
	add_content_in_box(ad->box, ad->bt_spk_user3);
	add_content_in_box(ad->box, ad->bt_spk_temp);
	add_content_in_box(ad->box, ad->bt_spk_volume_up);
	add_content_in_box(ad->box, ad->bt_avrcp_play_pause);
	add_content_in_box(ad->box, ad->bt_avrcp_next);

	evas_object_smart_callback_add(ad->bt_hid_tv, "clicked",
			button_tv_hid_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_hid_volume_up, "clicked",
			button_tv_hid_volume_up_clicked_cb, ad);

	evas_object_smart_callback_add(ad->bt_spk_tv, "clicked",
			button_spk_tv_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_spk_user1, "clicked",
			button_spk_user1_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_spk_user2, "clicked",
			button_spk_user2_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_spk_user3, "clicked",
			button_spk_user3_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_spk_temp, "clicked",
			button_spk_temp_clicked_cb, ad);

	evas_object_smart_callback_add(ad->bt_spk_volume_up, "clicked",
			button_spk_volume_up_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_avrcp_play_pause, "clicked",
			button_avrcp_play_pause_clicked_cb, ad);
	evas_object_smart_callback_add(ad->bt_avrcp_next, "clicked",
			button_avrcp_next_clicked_cb, ad);

	button_drow(ad);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== app_create() ===");

	appdata_s *ad = data;

	if (bt_info_init(ad) != BT_ERROR_NONE) {
		ui_app_exit();
	}

	bt_start_service(ad);

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== app_control() ===");
	/* Handle the launch request. */
}

#if 0
static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}
#endif

static void
app_terminate(void *data)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "=== app_terminate() ===");
	/* Release all resources. */

	bt_terminate();
}

#if 0
static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/

	int ret;
	char *language;

	ret = app_event_get_language(event_info, &language);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG,
				"app_event_get_language() failed. Err = %d.", ret);
		return;
	}

	if (language != NULL) {
		elm_language_set(language);
		free(language);
	}
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}
#endif

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};

#if 0
	app_event_handler_h handlers[5] = {NULL, };
#endif

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;

#if 0
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
#endif

	event_callback.app_control = app_control;

#if 0
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
#endif

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
