#ifndef __bt_test_H__
#define __bt_test_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "COZYMA_BT_APP"

#if !defined(PACKAGE)
#define PACKAGE "org.cozyma.bt_test"
#endif

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;

	Evas_Object *box;
	Evas_Object *lbl_info;

	Evas_Object *bt_hid_tv;
	Evas_Object *bt_hid_volume_down;
	Evas_Object *bt_hid_volume_up;
	Evas_Object *bt_hid_channel_down;
	Evas_Object *bt_hid_channel_up;
	Evas_Object *bt_hid_return;
	Evas_Object *bt_hid_esc;
	Evas_Object *bt_hid_smartkey;
	Evas_Object *bt_hid_arrow_left;
	Evas_Object *bt_hid_arrow_right;
	Evas_Object *bt_hid_arrow_up;
	Evas_Object *bt_hid_arrow_down;

	Evas_Object *bt_spk_tv;
	Evas_Object *bt_spk_user1;
	Evas_Object *bt_spk_user2;
	Evas_Object *bt_spk_user3;
	Evas_Object *bt_spk_temp;

	Evas_Object *bt_spk_volume_up;
	Evas_Object *bt_spk_volume_down;

	Evas_Object *bt_avrcp_play_pause;
	Evas_Object *bt_avrcp_prev;
	Evas_Object *bt_avrcp_next;
} appdata_s;

void button_drow(void *data);

#endif /* __bt_test_H__ */
