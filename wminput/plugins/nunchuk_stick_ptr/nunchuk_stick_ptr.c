/* Copyright (C) 2007 L. Donnie Smith <donnie.smith@gatech.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "wmplugin.h"

/* Button flags */
#define STICK_KEY_UP		0x0001
#define STICK_KEY_DOWN		0x0002
#define STICK_KEY_RIGHT		0x0004
#define STICK_KEY_LEFT		0x0008
#define STICK_MID_VAL		128
#define STICK_NEUTRAL		20

cwiid_wiimote_t *wiimote;

static unsigned char info_init = 0;
static struct wmplugin_info info;
static struct wmplugin_data data;

wmplugin_info_t wmplugin_info;
wmplugin_init_t wmplugin_init;
wmplugin_exec_t wmplugin_exec;
static void process_nunchuk(struct cwiid_nunchuk_mesg *mesg);

struct wmplugin_info *wmplugin_info() {
	if (!info_init) {
		info.button_count = 4;
		info.button_info[0].name = "Up";
		info.button_info[1].name = "Down";
		info.button_info[2].name = "Right";
		info.button_info[3].name = "Left";
		info.axis_count = 2;

        info.axis_info[0].name = "X";
        info.axis_info[0].type = WMPLUGIN_REL;
        info.axis_info[0].max = 223;
        info.axis_info[0].min = 19;
        info.axis_info[0].fuzz = 0;//what are fuzz and flat?
        info.axis_info[0].flat = 0;

        info.axis_info[1].name = "Y";
        info.axis_info[1].type = WMPLUGIN_REL;
        info.axis_info[1].max = 226;
        info.axis_info[1].min = 27;
        info.axis_info[1].fuzz = 0;//what are fuzz and flat?
        info.axis_info[1].flat = 0;
        info.param_count = 0;

		info_init = 1;
	}
	return &info;
}

int wmplugin_init(int id, cwiid_wiimote_t *arg_wiimote)
{
	wiimote = arg_wiimote;
	data.buttons = 0;
	if (wmplugin_set_rpt_mode(id, CWIID_RPT_NUNCHUK)) {
		return -1;
	}

	return 0;
}

struct wmplugin_data *wmplugin_exec(int mesg_count, union cwiid_mesg mesg[])
{
	int i;
    //cwiid_set_led(wiimote, CWIID_LED1_ON);
	struct wmplugin_data *ret = NULL;

	for (i=0; i < mesg_count; i++) {
		switch (mesg[i].type) {
		case CWIID_MESG_NUNCHUK:
			process_nunchuk(&mesg[i].nunchuk_mesg);
			ret = &data;
			break;
		default:
			break;
		}
	}

	return ret;
}

static void process_nunchuk(struct cwiid_nunchuk_mesg *mesg)
{
    //TODO: use different scale factors for each axis, since max distance traveled is different
    //and implement mouse acceleration using previous stick measurements+current
    int stx = mesg->stick[CWIID_X];
	int sty = mesg->stick[CWIID_Y];

	data.buttons=0;
    data.axes[0].valid= data.axes[1].valid =1;
    
    int xmov = stx-STICK_MID_VAL;
    int ymov = sty-STICK_MID_VAL;

    data.axes[0].value = (xmov>0)? ((xmov*xmov)/223) : -((xmov*xmov)/223);
    data.axes[1].value = (ymov>0)? -((ymov*ymov)/226) : ((ymov*ymov)/226);
    //note that y-axis is inverted to match my preference, flip the sign
    //if you wish
}

