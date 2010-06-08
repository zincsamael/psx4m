#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <hildon/hildon.h>
#include "minimal.h"

GdkImage *image;
HildonAnimationActor *actor;
GtkWidget *window, *drawing;

#define X_RES           800
#define Y_RES           480
#define D_WIDTH			640
#define D_HEIGHT		480
		
int screen_size;
unsigned short* screenbuffer = NULL;

void hildon_init(int *argc, char ***argv)
{
	gtk_init (argc, argv);

	window = hildon_stackable_window_new ();
	gtk_widget_realize (window);
	gtk_window_fullscreen (GTK_WINDOW(window));

	actor = HILDON_ANIMATION_ACTOR (hildon_animation_actor_new());
	hildon_animation_actor_set_position (actor, (X_RES - D_WIDTH)/2, (Y_RES - D_HEIGHT)/2 );
	hildon_animation_actor_set_parent (actor, GTK_WINDOW (window));

	drawing = gtk_image_new ();

	gtk_container_add (GTK_CONTAINER (actor), drawing);

	gtk_widget_show_all (GTK_WIDGET (actor));
	gtk_widget_show_all (GTK_WIDGET (window));
}

void gp2x_change_res(int w, int h)
{
	if (w <= 0 || h <= 0)
		return;

	if (image) gdk_image_destroy(image);
	image = gdk_image_new( GDK_IMAGE_FASTEST, gdk_visual_get_system(), w, h );

	screenbuffer = (unsigned short*) image->mem;
	screen_size = w * h * image->bpp;

	gtk_image_set_from_image (GTK_IMAGE(drawing), image, NULL);

	gtk_window_resize (GTK_WINDOW (actor), w, h);
	hildon_animation_actor_set_scale (actor,
				(gdouble)D_WIDTH / (gdouble)w,
				(gdouble)D_HEIGHT / (gdouble)h
	);
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
	if (gp2x_screen16 && screen_size)
		memset(gp2x_screen16, 0, screen_size);
}

void updateScreen()
{
	gtk_widget_queue_draw (drawing);
}

void gp2x_printfchar15(gp2x_font *f, unsigned char c)
{
  unsigned short *dst=&((unsigned short*)gp2x_screen16)[f->x+f->y*(image->bpl>>1)],w,h=f->h;
//unsigned char  *src=f->data[ (c%16)*f->w + (c/16)*f->h ];
  unsigned char  *src=&f->data[c*10];

 if(f->solid)
         while(h--)
         {
          w=f->wmask;
          while(w)
          {
           if( *src & w ) *dst++=f->fg; else *dst++=f->bg;
           w>>=1;
          }
          src++;    

          dst+=(image->bpl>>1)-(f->w);
         }
 else
         while(h--)
         {
          w=f->wmask;
          while(w)
          {
           if( *src & w ) *dst=f->fg;
           dst++;
           w>>=1;
          }
          src++;

          dst+=(image->bpl>>1)-(f->w);
         }
}

