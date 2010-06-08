#include <unistd.h>
#include "minimal.h"

unsigned short* screenbuffer = NULL;

void gp2x_change_res(int w, int h)
{
	printf("gp2x_change_res %dx%d\n", w, h);
	if (screenbuffer) free(screenbuffer);
	screenbuffer = (unsigned short*) malloc(w * h * sizeof(unsigned short));
}

unsigned long gp2x_joystick_read(void)
{
	printf("gp2x_joystick_read()\n");
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
