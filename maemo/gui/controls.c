// -*- Mode: c; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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

static GtkDialog* dialog;
static int current_player;
static HildonCheckButton* keys_chk;
static HildonButton* keys_btn;
static HildonCheckButton* touch_chk;
static HildonCheckButton* touch_show_chk;

static void load_settings()
{
	gchar key_base[kGConfPlayerPathBufferLen];
	const int key_len = sprintf(key_base, kGConfPlayerPath, current_player);
	gchar *key = key_base + key_len;

	// Keyboard
	strcpy(key, kGConfPlayerKeyboardEnable);
	hildon_check_button_set_active(keys_chk,
		gconf_client_get_bool(gcc, key_base, NULL));

	// Touchscreen
	strcpy(key, kGConfPlayerTouchscreenEnable);
	hildon_check_button_set_active(touch_chk,
		gconf_client_get_bool(gcc, key_base, NULL));
	strcpy(key, kGConfPlayerTouchscreenShow);
	hildon_check_button_set_active(touch_show_chk,
		gconf_client_get_bool(gcc, key_base, NULL));

}

static void save_settings()
{
	gchar key_base[kGConfPlayerPathBufferLen];
	const int key_len = sprintf(key_base, kGConfPlayerPath, current_player);
	gchar *key = key_base + key_len;

	// Keyboard
	strcpy(key, kGConfPlayerKeyboardEnable);
	gconf_client_set_bool(gcc, key_base,
		hildon_check_button_get_active(keys_chk), NULL);

	// Touchscreen
	strcpy(key, kGConfPlayerTouchscreenEnable);
	gconf_client_set_bool(gcc, key_base,
		hildon_check_button_get_active(touch_chk), NULL);
	strcpy(key, kGConfPlayerTouchscreenShow);
	gconf_client_set_bool(gcc, key_base,
		hildon_check_button_get_active(touch_show_chk), NULL);

}

static void keys_btn_callback(GtkWidget * button, gpointer data)
{
	keys_dialog(GTK_WINDOW(dialog), GPOINTER_TO_INT(data));
}

static void cb_dialog_response(GtkWidget * button, gint response, gpointer data)
{
	if (response == GTK_RESPONSE_OK) {
		save_settings();
		settings_update_controls(current_player);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void set_button_layout(HildonButton* button,
 GtkSizeGroup* titles_size_group, GtkSizeGroup* values_size_group)
{
	hildon_button_add_title_size_group(button, titles_size_group);
	hildon_button_add_value_size_group(button, values_size_group);
	hildon_button_set_alignment(button, 0.0, 0.5, 1.0, 0.0);
}

gchar* controls_describe(int player)
{
	static gchar description[256];
	unsigned char count = 0;

	gchar key_base[kGConfPlayerPathBufferLen];
	const int key_len = sprintf(key_base, kGConfPlayerPath, player);
	gchar *key = key_base + key_len;

	description[0] = '\0';

	strcpy(key, kGConfPlayerKeyboardEnable);
	if (gconf_client_get_bool(gcc, key_base, NULL)) {
		strcpy(description, _("Keyboard"));
		count++;
	}

	strcpy(key, kGConfPlayerTouchscreenEnable);
	if (gconf_client_get_bool(gcc, key_base, NULL)) {
		if (count > 0) strcat(description, ", ");
		strcat(description, _("Touchscreen"));
		count++;
	}

	if (count == 0) {
		/* Add some text if there are no enabled controls */
		strcpy(description, _("Disabled"));
	}

	return description;
}

void controls_dialog(GtkWindow* parent, int player)
{
	gchar* title = g_strdup_printf(_("Player %d controls"), player);
	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(title,
		parent, GTK_DIALOG_MODAL,
		GTK_STOCK_SAVE, GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL));
	g_free(title);

	current_player = player;

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

	GtkLabel* separator_1 = GTK_LABEL(gtk_label_new(_("Keys")));
	gtk_label_set_attributes(separator_1, pattrlist);
	gtk_label_set_justify(separator_1, GTK_JUSTIFY_CENTER);

	keys_chk = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(keys_chk), _("Enable keyboard"));
	set_button_layout(HILDON_BUTTON(keys_chk),
		titles_size_group, values_size_group);

	keys_btn = HILDON_BUTTON(hildon_button_new_with_text(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
		HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
		_("Configure keys…"), NULL));
	set_button_layout(HILDON_BUTTON(keys_btn),
		titles_size_group, values_size_group);
	g_signal_connect(G_OBJECT(keys_btn), "clicked",
					G_CALLBACK(keys_btn_callback), GINT_TO_POINTER(player));

	GtkLabel* separator_2 = GTK_LABEL(gtk_label_new(_("Touchscreen")));
	gtk_label_set_attributes(separator_2, pattrlist);
	gtk_label_set_justify(separator_2, GTK_JUSTIFY_CENTER);

	touch_chk = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(touch_chk),
		_("Enable touchscreen buttons"));
	set_button_layout(HILDON_BUTTON(touch_chk),
		titles_size_group, values_size_group);
		
	touch_show_chk = HILDON_CHECK_BUTTON(hildon_check_button_new(
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(touch_show_chk),
		_("Show on-screen button grid"));
	set_button_layout(HILDON_BUTTON(touch_show_chk),
		titles_size_group, values_size_group);

	gtk_box_pack_start(box, GTK_WIDGET(separator_1),
		FALSE, FALSE, HILDON_MARGIN_HALF);
	gtk_box_pack_start(box, GTK_WIDGET(keys_chk),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(keys_btn),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(separator_2),
		FALSE, FALSE, HILDON_MARGIN_HALF);
	gtk_box_pack_start(box, GTK_WIDGET(touch_chk),
		FALSE, FALSE, 0);
	gtk_box_pack_start(box, GTK_WIDGET(touch_show_chk),
		FALSE, FALSE, 0);

	hildon_pannable_area_add_with_viewport(pannable, GTK_WIDGET(box));
	gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox), GTK_WIDGET(pannable));

	pango_attr_list_unref(pattrlist);
	g_object_unref(titles_size_group);
	g_object_unref(values_size_group);

	load_settings();

	gtk_window_resize(GTK_WINDOW(dialog), 800, 360);

	g_signal_connect(G_OBJECT(dialog), "response",
					G_CALLBACK (cb_dialog_response), NULL);
	
	gtk_widget_show_all(GTK_WIDGET(dialog));
}

