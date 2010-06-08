#include <unistd.h>
#include "minimal.h"

unsigned short _screenbuffer[320*240];
unsigned short* screenbuffer = _screenbuffer;

void gp2x_change_res(int w, int h)
{
	printf("gp2x_change_res %dx%d\n", w, h);
}

unsigned long gp2x_joystick_read(void)
{
	printf("gp2x_joystick_read()\n");
	return 0;
}

void updateScreen()
{
	printf("updateScreen()\n");
}
