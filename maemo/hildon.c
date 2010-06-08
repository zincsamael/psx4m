#include <cairo.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <hildon/hildon.h>
#include "minimal.h"

unsigned short* screenbuffer = NULL;

static gint X_SIZE = 320;
static gint Y_SIZE = 240;

static gint X_POS = 0;
static gint Y_POS = 56;

/* Using 800x424 as fullscreen */
static gdouble X_SCALE = 2.5;  /* width / X_SIZE */
static gdouble Y_SCALE = 1.77; /* height / Y_SIZE */

static void
drawing_realize (GtkWidget *drawing, gpointer user_data)
{
	cairo_t *context;
	context = gdk_cairo_create (GDK_DRAWABLE (drawing->window));
}

static GtkWidget *
get_drawing_widget (void)
{
	GtkWidget *drawing = gtk_drawing_area_new ();
	g_signal_connect (drawing, "realize",
			G_CALLBACK (drawing_realize), NULL);
	return drawing;
}

void hildon_init(int *argc, char ***argv)
{
	HildonAnimationActor *actor;
	GtkWidget *window, *drawing;

	gtk_init (argc, argv);

	window = hildon_stackable_window_new ();
	actor = HILDON_ANIMATION_ACTOR (hildon_animation_actor_new());

	gtk_window_resize (GTK_WINDOW (actor), X_SIZE, Y_SIZE);
	hildon_animation_actor_set_position (actor, X_POS, Y_POS);
	hildon_animation_actor_set_parent (actor, GTK_WINDOW (window));
	hildon_animation_actor_set_scale (actor, X_SCALE, Y_SCALE);

	drawing = get_drawing_widget ();
	gtk_container_add (GTK_CONTAINER (actor), drawing);

	gtk_widget_show_all (GTK_WIDGET (actor));
	gtk_widget_show_all (GTK_WIDGET (window));
}

void gp2x_change_res(int w, int h)
{
	printf("gp2x_change_res %dx%d\n", w, h);
	if (screenbuffer) free(screenbuffer);
	screenbuffer = (unsigned short*) malloc(w * h * sizeof(unsigned short));
}

unsigned long gp2x_joystick_read(void)
{
	printf("gp2x_joystick_read()\n");

	/* process GTK+ events */
	while (gtk_events_pending())
		gtk_main_iteration();

	return 0;
}

void gp2x_video_RGB_clearscreen16(void)
{
	printf("gp2x_video_RGB_clearscreen16()\n");
}

void updateScreen()
{
	printf("updateScreen()\n");
}
