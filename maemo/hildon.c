#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <hildon/hildon.h>
#include "minimal.h"

GdkPixbuf *pixbuf;
HildonAnimationActor *actor;
GtkWidget *window, *drawing;
GdkGC *gc;

static gint X_SIZE = 320;
static gint Y_SIZE = 240;

static gint X_POS = 0;
static gint Y_POS = 56;

/* Using 800x424 as fullscreen */
static gdouble X_SCALE = 2.5;  /* width / X_SIZE */
static gdouble Y_SCALE = 1.77; /* height / Y_SIZE */

#define X_RES           800
#define Y_RES           480

int screen_size;
unsigned short* screenbuffer = NULL;

void hildon_init(int *argc, char ***argv)
{
	gtk_init (argc, argv);

	window = hildon_stackable_window_new ();
//	actor = HILDON_ANIMATION_ACTOR (hildon_animation_actor_new());

//	gtk_window_resize (GTK_WINDOW (actor), X_SIZE, Y_SIZE);
//	hildon_animation_actor_set_position (actor, X_POS, Y_POS);
//	hildon_animation_actor_set_parent (actor, GTK_WINDOW (window));
//	hildon_animation_actor_set_scale (actor, X_SCALE, Y_SCALE);

	drawing = gtk_drawing_area_new ();
	gc = gdk_gc_new (drawing->window);

	gtk_container_add (GTK_CONTAINER (window), drawing);
//	gtk_container_add (GTK_CONTAINER (actor), drawing);

//	gtk_widget_show_all (GTK_WIDGET (actor));
	gtk_widget_show_all (GTK_WIDGET (window));
}

void gp2x_change_res(int w, int h)
{
	printf("gp2x_change_res %dx%d\n", w, h);

	if(w <= 0 || h <= 0)
		return;

	if (pixbuf) gdk_pixbuf_unref(pixbuf);
	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, w, h);

	screenbuffer = (unsigned short*) gdk_pixbuf_get_pixels (pixbuf);
	screen_size = w * h * ( gdk_pixbuf_get_bits_per_sample(pixbuf) / 8 );
}

unsigned long gp2x_joystick_read(void)
{
	/* process GTK+ events */
	while (gtk_events_pending())
		gtk_main_iteration();

	return 0;
}

void gp2x_video_RGB_clearscreen16(void)
{
	memset(gp2x_screen16, 0, screen_size);
}

void updateScreen()
{
	gdk_draw_pixbuf(
		drawing->window, NULL,
		pixbuf, 0, 0, 0, 0,
		gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf),
		GDK_RGB_DITHER_NONE, 0, 0);
	gdk_gc_unref(gc);
}
