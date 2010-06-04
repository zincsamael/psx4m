// -*- Mode: c; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <startup_plugin.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <hildon/hildon-file-chooser-dialog.h>
#include <hildon/hildon-note.h>
#include <hildon/hildon-defines.h>

#include <hildon/hildon-button.h>
#include <hildon/hildon-check-button.h>
#include <hildon/hildon-picker-button.h>
#include <hildon/hildon-touch-selector.h>
#include <hildon/hildon-gtk.h>

#include "plugin.h"
#include "gconf.h"
#include "i18n.h"

static GtkWidget * load_plugin(void);
static void unload_plugin(void);
static void write_config(void);
static GtkWidget ** load_menu(guint *);
static void update_menu(void);
static void plugin_callback(GtkWidget * menu_item, gpointer data);

GConfClient * gcc = NULL;
static GameStartupInfo gs;
static GtkWidget * menu_items[2];

static StartupPluginInfo plugin_info = {
	load_plugin,
	unload_plugin,
	write_config,
	load_menu,
	update_menu,
	plugin_callback
};

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, TRUE)

gchar* current_rom_file = 0;
gboolean current_rom_file_exists = FALSE;

//static HildonButton* select_rom_btn;
static HildonCheckButton* sound_check;
//static HildonPickerButton* framerate_picker;
//static HildonCheckButton* display_fps_check;
//static HildonCheckButton* turbo_check;

//static inline void set_rom_label(gchar * text)
//{
//	hildon_button_set_value(select_rom_btn, text);
//}

/*
static void set_rom(const char * rom_file)
{
	if (current_rom_file) g_free(current_rom_file);
	if (!rom_file || strlen(rom_file) == 0) {
		current_rom_file = NULL;
		set_rom_label(_("<no rom selected>"));
		return;
	}

	current_rom_file = g_strdup(rom_file);

	gchar * utf8_filename = g_filename_display_basename(rom_file);
	set_rom_label(utf8_filename);
	g_free(utf8_filename);

	current_rom_file_exists = g_file_test(current_rom_file,
		G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR);

	game_state_update();
	save_clear();
}
*/

static inline GtkWindow* get_parent_window() {
	return GTK_WINDOW(gs.ui->hildon_appview);
}

/*
static void select_rom_callback(GtkWidget * button, gpointer data)
{
	GtkWidget * dialog;
	GtkFileFilter * filter;
	gchar * filename = NULL;

	filter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter, "*.smc");
	gtk_file_filter_add_pattern(filter, "*.sfc");
	gtk_file_filter_add_pattern(filter, "*.fig");
	gtk_file_filter_add_pattern(filter, "*.smc.gz");
	gtk_file_filter_add_pattern(filter, "*.sfc.gz");
	gtk_file_filter_add_pattern(filter, "*.fig.gz");
	gtk_file_filter_add_pattern(filter, "*.zip");

	dialog = hildon_file_chooser_dialog_new_with_properties(
		get_parent_window(),
		"action", GTK_FILE_CHOOSER_ACTION_OPEN,
		"local-only", TRUE,
		"filter", filter,
		NULL);
	hildon_file_chooser_dialog_set_show_upnp(HILDON_FILE_CHOOSER_DIALOG(dialog),
		FALSE);

	if (current_rom_file_exists) {
		// By default open showing the last selected file
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), 
			current_rom_file);
	}

	gtk_widget_show_all(GTK_WIDGET(dialog));
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	}

	gtk_widget_destroy(dialog);

	if (filename) {
		set_rom(filename);
		g_free(filename);
	}
}
*/

//static void controls_item_callback(GtkWidget * button, gpointer data)
//{
//	controls_dialog(get_parent_window(), GPOINTER_TO_INT(data));
//}

static void settings_item_callback(GtkWidget * button, gpointer data)
{
	settings_dialog(get_parent_window());
}

static void about_item_callback(GtkWidget * button, gpointer data)
{
	about_dialog(get_parent_window());
}

/** Called for each of the play/restart/continue buttons */
static void found_ogs_button_callback(GtkWidget *widget, gpointer data)
{
	hildon_gtk_widget_set_theme_size(widget,
		HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_THUMB_HEIGHT);
	gtk_widget_set_size_request(widget, 200, -1);
	gtk_box_set_child_packing(GTK_BOX(data), widget,
		FALSE, FALSE, 0, GTK_PACK_START);
}

static GtkWidget * load_plugin(void)
{
	g_type_init();
	gcc = gconf_client_get_default();

	GtkWidget* parent = gtk_vbox_new(FALSE, HILDON_MARGIN_DEFAULT);

/* Select ROM button */
	sound_check =
		HILDON_CHECK_BUTTON(hildon_check_button_new(
			HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
	gtk_button_set_label(GTK_BUTTON(sound_check), _("Sound"));

	// Ugly hacks: resize the Osso-Games-Startup buttons
	GtkBox* button_box =
		GTK_BOX(gtk_widget_get_parent(gs.ui->play_button));
	gtk_box_set_spacing(button_box, HILDON_MARGIN_DEFAULT);
	gtk_container_foreach(GTK_CONTAINER(button_box),
		found_ogs_button_callback, button_box);

	// Ugly hacks: move the sound button to the right.
	gtk_box_pack_start_defaults(button_box, GTK_WIDGET(sound_check));
	gtk_box_reorder_child(button_box, GTK_WIDGET(sound_check), 10);

	/* First row of widgets */
	GtkBox* opt_hbox1 = GTK_BOX(gtk_hbox_new(FALSE, HILDON_MARGIN_DEFAULT));

	gtk_box_pack_start(GTK_BOX(parent), GTK_WIDGET(opt_hbox1), FALSE, FALSE, 0);


	/* Load current configuration from GConf */
	hildon_check_button_set_active(sound_check,
		gconf_client_get_bool(gcc, kGConfSound, NULL));
//	hildon_check_button_set_active(display_fps_check,
//		gconf_client_get_bool(gcc, kGConfDisplayFramerate, NULL));

//	set_rom(gconf_client_get_string(gcc, kGConfRomFile, NULL));

	// Connect signals
//	g_signal_connect(G_OBJECT(select_rom_btn), "clicked",
//					G_CALLBACK(select_rom_callback), NULL);

	return parent;
}

static void unload_plugin(void)
{
	if (current_rom_file) {
		g_free(current_rom_file);
		current_rom_file = 0;
	}
	game_state_clear();
	save_clear();
	g_object_unref(gcc);
}

static void write_config(void)
{
	/* Write current settings to gconf */
	gconf_client_set_bool(gcc, kGConfSound,
		hildon_check_button_get_active(sound_check), NULL);

//	if (current_rom_file) {
//		gconf_client_set_string(gcc, kGConfRomFile, current_rom_file, NULL);
//	}
}

static GtkWidget **load_menu(guint *nitems)
{
	const HildonSizeType button_size =
		HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH;
	menu_items[0] = hildon_gtk_button_new(button_size);
	gtk_button_set_label(GTK_BUTTON(menu_items[0]), _("Settings…"));
	menu_items[1] = hildon_gtk_button_new(button_size);
	gtk_button_set_label(GTK_BUTTON(menu_items[1]), _("About…"));
	*nitems = 2;

	g_signal_connect(G_OBJECT(menu_items[0]), "clicked",
					G_CALLBACK(settings_item_callback), NULL);
	g_signal_connect(G_OBJECT(menu_items[1]), "clicked",
					G_CALLBACK(about_item_callback), NULL);

	return menu_items;
}

static void update_menu(void)
{
	// Nothing to update in the current menu
}

// From osso-games-startup
#define MA_GAME_PLAY 1
#define MA_GAME_RESTART 2
#define MA_GAME_OPEN 3
#define MA_GAME_SAVE 4
#define MA_GAME_SAVE_AS 5
#define MA_GAME_HELP 6
#define MA_GAME_RECENT_1 7
#define MA_GAME_RECENT_2 8
#define MA_GAME_RECENT_3 9
#define MA_GAME_RECENT_4 10
#define MA_GAME_RECENT_5 11
#define MA_GAME_RECENT_6 12
#define MA_GAME_CLOSE 13
#define MA_GAME_HIGH_SCORES 14
#define MA_GAME_RESET 15
#define MA_GAME_CHECKSTATE 16
#define MA_GAME_SAVEMENU_REFERENCE 17
#define ME_GAME_OPEN     20
#define ME_GAME_SAVE     21
#define ME_GAME_SAVE_AS  22
#define MA_GAME_PLAYING_START 30
#define MA_GAME_PLAYING 31

static void plugin_callback(GtkWidget * menu_item, gpointer data)
{
	switch ((gint) data) {
		case ME_GAME_OPEN:
			save_load(get_parent_window());
			break;
		case ME_GAME_SAVE:
			save_save(get_parent_window());
			break;
		case ME_GAME_SAVE_AS:
			save_save_as(get_parent_window());
			break;
		case MA_GAME_PLAYING_START:
			if (!menu_item) {
				// Avoid duplicate message
				break;
			}
			if (!current_rom_file) {
				GtkWidget* note = hildon_note_new_information(get_parent_window(),
					_("No ROM selected"));
				gtk_dialog_run(GTK_DIALOG(note));
				gtk_widget_destroy(note);
			} else if (!current_rom_file_exists) {
				GtkWidget* note = hildon_note_new_information(get_parent_window(),
					_("ROM file does not exist"));
				gtk_dialog_run(GTK_DIALOG(note));
				gtk_widget_destroy(note);
			}
			break;
	}
}

