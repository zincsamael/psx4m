/*
 * This file is part of PSX4All Maemo Edition
 *
 * Copyright (C) 2010 Tomasz Sterna <tomek@xiaoka.com>
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <string.h>
#include <gtk/gtk.h>
#include <hildon/hildon-helper.h>

#include <hildon/hildon-gtk.h>
#include <hildon/hildon-pannable-area.h>
#include <hildon/hildon-button.h>
#include <hildon/hildon-check-button.h>
#include <hildon/hildon-picker-button.h>
#include <hildon/hildon-touch-selector.h>
#include <pango/pango-attributes.h>

#include "plugin.h"
#include "gconf.h"
#include "i18n.h"

extern int skipCountTable[];
extern int skipRateTable[];

static GtkDialog* dialog;
static HildonButton* keys_btn;
static HildonCheckButton* gpustat_check;
static HildonCheckButton* vmem_check;
static HildonCheckButton* nullgpu_check;
static HildonCheckButton* flimit_check;
static HildonCheckButton* fskip_line_check;
static HildonCheckButton* fskip_poly_check;
static HildonCheckButton* fskip_sprite_check;
static HildonCheckButton* fskip_image_check;
static HildonCheckButton* fskip_blit_check;
static HildonCheckButton* wclock_check;
static HildonCheckButton* abbey_check;
static HildonPickerButton* interlace_picker;
static HildonPickerButton* fskip_picker;
static HildonPickerButton* multiplier_picker;

static void load_settings()
{
	hildon_check_button_set_active(gpustat_check,
		gconf_client_get_bool(gcc, kGConfGPUStat, NULL));
	hildon_check_button_set_active(vmem_check,
		gconf_client_get_bool(gcc, kGConfVMEM, NULL));
	hildon_check_button_set_active(nullgpu_check,
		gconf_client_get_bool(gcc, kGConfNullGPU, NULL));
	hildon_check_button_set_active(flimit_check,
		gconf_client_get_bool(gcc, kGConfFrameLimit, NULL));
	hildon_check_button_set_active(fskip_line_check,
		gconf_client_get_bool(gcc, kGConfFrameSkipLine, NULL));
	hildon_check_button_set_active(fskip_poly_check,
		gconf_client_get_bool(gcc, kGConfFrameSkipPoly, NULL));
	hildon_check_button_set_active(fskip_sprite_check,
		gconf_client_get_bool(gcc, kGConfFrameSkipSprite, NULL));
	hildon_check_button_set_active(fskip_image_check,
		gconf_client_get_bool(gcc, kGConfFrameSkipImage, NULL));
	hildon_check_button_set_active(fskip_blit_check,
		gconf_client_get_bool(gcc, kGConfFrameSkipBlit, NULL));
	hildon_check_button_set_active(wclock_check,
		gconf_client_get_bool(gcc, kGConfWallClock, NULL));
	hildon_check_button_set_active(abbey_check,
		gconf_client_get_bool(gcc, kGConfAbbeyFix, NULL));

	hildon_picker_button_set_active(interlace_picker,
		gconf_client_get_int(gcc, kGConfInterlace, NULL));
	hildon_picker_button_set_active(fskip_picker,
		gconf_client_get_int(gcc, kGConfFrameSkip, NULL));
	hildon_picker_button_set_active(multiplier_picker,
		gconf_client_get_int(gcc, kGConfCycleMultiplier, NULL));
}

static void save_settings()
{
	gconf_client_set_bool(gcc, kGConfGPUStat,
		hildon_check_button_get_active(gpustat_check), NULL);
	gconf_client_set_bool(gcc, kGConfVMEM,
		hildon_check_button_get_active(vmem_check), NULL);
	gconf_client_set_bool(gcc, kGConfNullGPU,
		hildon_check_button_get_active(nullgpu_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameLimit,
		hildon_check_button_get_active(flimit_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameSkipLine,
		hildon_check_button_get_active(fskip_line_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameSkipPoly,
		hildon_check_button_get_active(fskip_poly_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameSkipSprite,
		hildon_check_button_get_active(fskip_sprite_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameSkipImage,
		hildon_check_button_get_active(fskip_image_check), NULL);
	gconf_client_set_bool(gcc, kGConfFrameSkipBlit,
		hildon_check_button_get_active(fskip_blit_check), NULL);
	gconf_client_set_bool(gcc, kGConfWallClock,
		hildon_check_button_get_active(wclock_check), NULL);
	gconf_client_set_bool(gcc, kGConfAbbeyFix,
		hildon_check_button_get_active(abbey_check), NULL);

	gconf_client_set_int(gcc, kGConfInterlace,
		hildon_picker_button_get_active(interlace_picker), NULL);
	gconf_client_set_int(gcc, kGConfFrameSkip,
		hildon_picker_button_get_active(fskip_picker), NULL);
	gconf_client_set_int(gcc, kGConfCycleMultiplier,
		hildon_picker_button_get_active(multiplier_picker), NULL);
}

static void cb_dialog_response(GtkWidget * button, gint response, gpointer data)
{
	if (response == GTK_RESPONSE_OK) {
		save_settings();
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void keys_btn_callback(GtkWidget * button, gpointer data)
{
	keys_dialog(GTK_WINDOW(dialog), GPOINTER_TO_INT(data));
}

gchar* get_frameskip_name(int skipValue)
{
	return g_strdup_printf("%d/%d", skipCountTable[skipValue],
				skipRateTable[skipValue]);
}

void set_button_layout(HildonButton* button,
 GtkSizeGroup* titles_size_group, GtkSizeGroup* values_size_group)
{
	hildon_button_add_title_size_group(button, titles_size_group);
	hildon_button_add_value_size_group(button, values_size_group);
	hildon_button_set_alignment(button, 0.0, 0.5, 1.0, 0.0);
}

void settings_dialog(GtkWindow* parent)
{
	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(_("Settings"),
		parent, GTK_DIALOG_MODAL,
		GTK_STOCK_SAVE, GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL));

	GtkBox * box = GTK_BOX(gtk_vbox_new(FALSE, HILDON_MARGIN_HALF));
	HildonPannableArea * pannable =
		HILDON_PANNABLE_AREA(hildon_pannable_area_new());
	GtkSizeGroup * titles_size_group =
		 gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	GtkSizeGroup * values_size_group =
		 gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
	PangoAttrList *pattrlist = pango_attr_list_new();
	PangoAttribute *attr = pango_attr_size_new(22 * PANGO_SCALE);
	attr->start_index = 0;
	attr->end_index = G_MAXINT;
	pango_attr_list_insert(pattrlist, attr);

	GtkLabel* separator_1 = GTK_LABEL(gtk_label_new(_("Controls")));
	gtk_label_set_attributes(separator_1, pattrlist);
	gtk_label_set_justify(separator_1, GTK_JUSTIFY_CENTER);

	keys_btn = HILDON_BUTTON(hildon_button_new_with_text(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
		HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
		_("Configure keys…"), NULL));
	set_button_layout(HILDON_BUTTON(keys_btn),
		titles_size_group, values_size_group);
	g_signal_connect(G_OBJECT(keys_btn), "clicked",
					G_CALLBACK(keys_btn_callback), GINT_TO_POINTER(1));

	GtkLabel* separator_2 = GTK_LABEL(gtk_label_new(_("Graphics")));
	gtk_label_set_attributes(separator_2, pattrlist);
	gtk_label_set_justify(separator_2, GTK_JUSTIFY_CENTER);

	gpustat_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(gpustat_check), _("Show GPU statistics"));
	set_button_layout(HILDON_BUTTON(gpustat_check),
		titles_size_group, values_size_group);

	vmem_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(vmem_check), _("Display Video Memory"));
	set_button_layout(HILDON_BUTTON(vmem_check),
		titles_size_group, values_size_group);

	nullgpu_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(nullgpu_check), _("NULL GPU"));
	set_button_layout(HILDON_BUTTON(nullgpu_check),
		titles_size_group, values_size_group);

	interlace_picker = HILDON_PICKER_BUTTON(hildon_picker_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
		HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(HILDON_BUTTON(interlace_picker), _("Interlace"));
	set_button_layout(HILDON_BUTTON(interlace_picker),
		titles_size_group, values_size_group);

	HildonTouchSelector* interlace_sel =
		HILDON_TOUCH_SELECTOR(hildon_touch_selector_new_text());
	hildon_touch_selector_append_text(interlace_sel, _("0"));
	hildon_touch_selector_append_text(interlace_sel, _("1"));
	hildon_touch_selector_append_text(interlace_sel, _("3"));
	hildon_touch_selector_append_text(interlace_sel, _("7"));
	hildon_picker_button_set_selector(interlace_picker, interlace_sel);

	flimit_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(flimit_check), _("Frame limit"));
	set_button_layout(HILDON_BUTTON(flimit_check),
		titles_size_group, values_size_group);

	fskip_picker = HILDON_PICKER_BUTTON(hildon_picker_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
		HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(HILDON_BUTTON(fskip_picker), _("Frame skip"));
	set_button_layout(HILDON_BUTTON(fskip_picker),
		titles_size_group, values_size_group);

	HildonTouchSelector* fskip_sel =
		HILDON_TOUCH_SELECTOR(hildon_touch_selector_new_text());
	int i=0;
	for (i=0; i<=8; i++) {
		hildon_touch_selector_append_text(fskip_sel,
						get_frameskip_name(i));
	}
	hildon_picker_button_set_selector(fskip_picker, fskip_sel);

	multiplier_picker = HILDON_PICKER_BUTTON(hildon_picker_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
		HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(HILDON_BUTTON(multiplier_picker), _("Cycle multiplier"));
	set_button_layout(HILDON_BUTTON(multiplier_picker),
		titles_size_group, values_size_group);

	HildonTouchSelector* multiplier_sel =
		HILDON_TOUCH_SELECTOR(hildon_touch_selector_new_text());
	hildon_touch_selector_append_text(multiplier_sel, _("1"));
	hildon_touch_selector_append_text(multiplier_sel, _("2"));
	hildon_touch_selector_append_text(multiplier_sel, _("3"));
	hildon_touch_selector_append_text(multiplier_sel, _("4"));
	hildon_touch_selector_append_text(multiplier_sel, _("5"));
	hildon_touch_selector_append_text(multiplier_sel, _("6"));
	hildon_touch_selector_append_text(multiplier_sel, _("7"));
	hildon_touch_selector_append_text(multiplier_sel, _("8"));
	hildon_touch_selector_append_text(multiplier_sel, _("9"));
	hildon_touch_selector_append_text(multiplier_sel, _("10"));
	hildon_picker_button_set_selector(multiplier_picker, multiplier_sel);

	abbey_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(abbey_check), _("Abe's Oddysee fix"));
	set_button_layout(HILDON_BUTTON(abbey_check),
		titles_size_group, values_size_group);

	fskip_line_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(fskip_line_check), _("Frameskip: Line"));
	set_button_layout(HILDON_BUTTON(fskip_line_check),
		titles_size_group, values_size_group);

	fskip_poly_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(fskip_poly_check), _("Frameskip: Poly"));
	set_button_layout(HILDON_BUTTON(fskip_poly_check),
		titles_size_group, values_size_group);

	fskip_sprite_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(fskip_sprite_check), _("Frameskip: Sprite"));
	set_button_layout(HILDON_BUTTON(fskip_sprite_check),
		titles_size_group, values_size_group);

	fskip_image_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(fskip_image_check), _("Frameskip: Image"));
	set_button_layout(HILDON_BUTTON(fskip_image_check),
		titles_size_group, values_size_group);

	fskip_blit_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(fskip_blit_check), _("Frameskip: Blit"));
	set_button_layout(HILDON_BUTTON(fskip_blit_check),
		titles_size_group, values_size_group);

	wclock_check = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(wclock_check), _("Wall Clock timing"));
	set_button_layout(HILDON_BUTTON(wclock_check),
		titles_size_group, values_size_group);

	gtk_box_pack_start(box, GTK_WIDGET(separator_1),
		FALSE, FALSE, HILDON_MARGIN_HALF);
	gtk_box_pack_start(box, GTK_WIDGET(keys_btn),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(separator_2),
		FALSE, FALSE, HILDON_MARGIN_HALF);
	gtk_box_pack_start(box, GTK_WIDGET(gpustat_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(vmem_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(nullgpu_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(interlace_picker),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(multiplier_picker),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(flimit_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_picker),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_line_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_poly_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_sprite_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_image_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(fskip_blit_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(wclock_check),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(abbey_check),
		FALSE, FALSE, 0);

	hildon_pannable_area_add_with_viewport(pannable, GTK_WIDGET(box));
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(pannable));

	pango_attr_list_unref(pattrlist);
	g_object_unref(titles_size_group);
	g_object_unref(values_size_group);

	load_settings();

	gtk_window_resize(GTK_WINDOW(dialog), 800, 380);

	g_signal_connect(G_OBJECT(dialog), "response",
					G_CALLBACK (cb_dialog_response), NULL);
	
	gtk_widget_show_all(GTK_WIDGET(dialog));
}

