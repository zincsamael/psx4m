#include "minimal.h"
#include <unistd.h>
#include <fcntl.h>

#define BIT_U		0x1
#define BIT_D		0x10
#define BIT_L 		0x4
#define BIT_R	 	0x40
#define BIT_SEL		(1<<9)
#define BIT_ST		(1<<8)
#define BIT_LPAD	(1<<10)
#define BIT_RPAD	(1<<11)
#define BIT_A		(1<<12)
#define BIT_B		(1<<13)
#define BIT_X		(1<<14)
#define BIT_Y		(1<<15)
#define BIT_VOL_UP	(1<<23)
#define BIT_VOL_DOWN	(1<<22)
#define BIT_PUSH	(1<<27)
#define BIT_MENU	(1<<31)


typedef unsigned char byte;

unsigned short BaseAddress[320*240];
extern unsigned short* screenbuffer;
extern float __audioVolume;
extern void sound_callback(void *userdata, u8 *stream, int len);
extern s8* packfile;

/* Main Resources */

u16* videobuffer;
/* The font is generated from Xorg 6x10-L1.bdf */
static unsigned char gp2x_fontf[256][10] = {
{ 0x00>>2, 0xA8>>2, 0x00>>2, 0x88>>2, 0x00>>2, 0x88>>2, 0x00>>2, 0xA8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0xF8>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, 0xA8>>2, 0x54>>2, },
{ 0x00>>2, 0x90>>2, 0x90>>2, 0xF0>>2, 0x90>>2, 0x90>>2, 0x78>>2, 0x10>>2, 0x10>>2, 0x10>>2, },
{ 0x00>>2, 0xE0>>2, 0x80>>2, 0xC0>>2, 0x80>>2, 0xB8>>2, 0x20>>2, 0x30>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x70>>2, 0x80>>2, 0x80>>2, 0x70>>2, 0x70>>2, 0x48>>2, 0x70>>2, 0x48>>2, 0x48>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x78>>2, 0x40>>2, 0x70>>2, 0x40>>2, 0x40>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x90>>2, 0xD0>>2, 0xD0>>2, 0xB0>>2, 0x90>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x78>>2, },
{ 0x00>>2, 0x90>>2, 0x90>>2, 0x60>>2, 0x40>>2, 0x78>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xE0>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xE0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x3C>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x3C>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xFC>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFF>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x3C>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xE0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xFC>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xFC>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, },
{ 0x00>>2, 0x18>>2, 0x60>>2, 0x80>>2, 0x60>>2, 0x18>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xC0>>2, 0x30>>2, 0x08>>2, 0x30>>2, 0xC0>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x10>>2, 0xF8>>2, 0x20>>2, 0xF8>>2, 0x40>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xE0>>2, 0x40>>2, 0x48>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x70>>2, 0xA0>>2, 0x70>>2, 0x28>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x48>>2, 0xA8>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0xA8>>2, 0x90>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0xA0>>2, 0xA0>>2, 0x40>>2, 0xA8>>2, 0x90>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0xF8>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x30>>2, 0x20>>2, 0x40>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x60>>2, 0xA0>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x08>>2, 0x30>>2, 0x40>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x30>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x30>>2, 0x50>>2, 0x90>>2, 0xF8>>2, 0x10>>2, 0x10>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x40>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x10>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x00>>2, 0x30>>2, 0x20>>2, 0x40>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x10>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x98>>2, 0xA8>>2, 0xB0>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0x70>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x98>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x38>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x90>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x90>>2, 0xA0>>2, 0xC0>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0xD8>>2, 0xA8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0xC8>>2, 0xA8>>2, 0x98>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0x70>>2, 0x08>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x70>>2, 0x08>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x50>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0xA8>>2, 0xD8>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF8>>2, 0x08>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x40>>2, 0x20>>2, 0x10>>2, 0x08>>2, 0x08>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x10>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x00>>2, },
{ 0x20>>2, 0x10>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x08>>2, 0x68>>2, 0x98>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xF0>>2, 0x40>>2, 0x40>>2, 0x40>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x88>>2, 0x88>>2, 0x78>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x08>>2, 0x00>>2, 0x18>>2, 0x08>>2, 0x08>>2, 0x08>>2, 0x48>>2, 0x48>>2, 0x30>>2, },
{ 0x00>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x90>>2, 0xE0>>2, 0x90>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xD0>>2, 0xA8>>2, 0xA8>>2, 0xA8>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x80>>2, 0x80>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x68>>2, 0x98>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x08>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x80>>2, 0x70>>2, 0x08>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x40>>2, 0x40>>2, 0xF0>>2, 0x40>>2, 0x40>>2, 0x48>>2, 0x30>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0xA8>>2, 0xA8>>2, 0x50>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0xF8>>2, 0x10>>2, 0x20>>2, 0x40>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x18>>2, 0x20>>2, 0x10>>2, 0x60>>2, 0x10>>2, 0x20>>2, 0x18>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x60>>2, 0x10>>2, 0x20>>2, 0x18>>2, 0x20>>2, 0x10>>2, 0x60>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x48>>2, 0xA8>>2, 0x90>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x78>>2, 0xA0>>2, 0xA0>>2, 0xA0>>2, 0x78>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x40>>2, 0xE0>>2, 0x40>>2, 0x48>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x70>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x80>>2, 0xE0>>2, 0x90>>2, 0x48>>2, 0x38>>2, 0x08>>2, 0x70>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0xA8>>2, 0xC8>>2, 0xA8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x38>>2, 0x48>>2, 0x58>>2, 0x28>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x24>>2, 0x48>>2, 0x90>>2, 0x48>>2, 0x24>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x08>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0xE8>>2, 0xC8>>2, 0xC8>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0xF8>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x50>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x20>>2, 0x20>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x30>>2, 0x48>>2, 0x10>>2, 0x20>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x70>>2, 0x08>>2, 0x30>>2, 0x08>>2, 0x70>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xC8>>2, 0xB0>>2, 0x80>>2, 0x00>>2, },
{ 0x00>>2, 0x78>>2, 0xE8>>2, 0xE8>>2, 0x68>>2, 0x28>>2, 0x28>>2, 0x28>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x10>>2, 0x20>>2, },
{ 0x20>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x30>>2, 0x48>>2, 0x48>>2, 0x30>>2, 0x00>>2, 0x78>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x90>>2, 0x48>>2, 0x24>>2, 0x48>>2, 0x90>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0xC0>>2, 0x40>>2, 0x40>>2, 0xE4>>2, 0x0C>>2, 0x14>>2, 0x3C>>2, 0x04>>2, 0x00>>2, },
{ 0x40>>2, 0xC0>>2, 0x40>>2, 0x40>>2, 0xE8>>2, 0x14>>2, 0x04>>2, 0x08>>2, 0x1C>>2, 0x00>>2, },
{ 0xC0>>2, 0x20>>2, 0x40>>2, 0x20>>2, 0xC8>>2, 0x18>>2, 0x28>>2, 0x78>>2, 0x08>>2, 0x00>>2, },
{ 0x00>>2, 0x20>>2, 0x00>>2, 0x20>>2, 0x20>>2, 0x40>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x48>>2, 0xB0>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0xF8>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x3C>>2, 0x50>>2, 0x90>>2, 0x9C>>2, 0xF0>>2, 0x90>>2, 0x9C>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x20>>2, 0x40>>2, },
{ 0x40>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0xF8>>2, 0x80>>2, 0x80>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0xF8>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xF0>>2, 0x48>>2, 0x48>>2, 0xE8>>2, 0x48>>2, 0x48>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x88>>2, 0xC8>>2, 0xA8>>2, 0x98>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x50>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x98>>2, 0x98>>2, 0xA8>>2, 0xC8>>2, 0xC8>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x50>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x80>>2, 0xF0>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, 0x80>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x70>>2, 0x88>>2, 0x90>>2, 0xA0>>2, 0x90>>2, 0x88>>2, 0xB0>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x20>>2, 0x70>>2, 0x08>>2, 0x78>>2, 0x88>>2, 0x78>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x14>>2, 0x7C>>2, 0x90>>2, 0x7C>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x80>>2, 0x88>>2, 0x70>>2, 0x20>>2, 0x40>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0xF8>>2, 0x80>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x40>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x60>>2, 0x20>>2, 0x20>>2, 0x20>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0xC0>>2, 0x30>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0xB0>>2, 0xC8>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x28>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x70>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x70>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0xF8>>2, 0x00>>2, 0x20>>2, 0x00>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x00>>2, 0x00>>2, 0x78>>2, 0x98>>2, 0xA8>>2, 0xC8>>2, 0xF0>>2, 0x00>>2, 0x00>>2, },
{ 0x40>>2, 0x20>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x10>>2, 0x20>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x20>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x00>>2, 0x00>>2, },
{ 0x00>>2, 0x10>>2, 0x20>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
{ 0x00>>2, 0x00>>2, 0x80>>2, 0xF0>>2, 0x88>>2, 0x88>>2, 0x88>>2, 0xF0>>2, 0x80>>2, 0x80>>2, },
{ 0x00>>2, 0x50>>2, 0x00>>2, 0x88>>2, 0x88>>2, 0x98>>2, 0x68>>2, 0x08>>2, 0x88>>2, 0x70>>2, },
};

static gp2x_font gp2x_default_font;

void (*gp2x_printfchar)(gp2x_font *f, unsigned char c);

void gp2x_deinit(void)
{
	free(videobuffer);
}

void gp2x_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int hz, int solid_font)
{
	//init font
	gp2x_printf_init(&gp2x_default_font,6,10,gp2x_fontf,0xFFFF,0x0000,solid_font);

	//setpriority(PRIO_PROCESS, 0, -20);
	videobuffer = (u16*)malloc(320*240*2);
}

#ifndef MAEMO_CHANGES
void gp2x_video_RGB_clearscreen16(void)
{
  memset(gp2x_screen16, 0, 320*240*2);
}
#endif

u64 gp2x_timer_read(void)
{
  struct timeval current_time;
  gettimeofday(&current_time, NULL);

  return (((u64)current_time.tv_sec * 1000000 + current_time.tv_usec) / 1000);
}

#ifndef MAEMO_CHANGES
void gp2x_printfchar15(gp2x_font *f, unsigned char c)
{
  unsigned short *dst=&((unsigned short*)gp2x_screen16)[f->x+f->y*(320)],w,h=f->h;
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

          dst+=(320)-(f->w);
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

          dst+=(320)-(f->w);
         }
}
#endif

void gp2x_printf(gp2x_font *f, int x, int y, const char *format, ...)
{
 char buffer[4096]; int c; gp2x_font *g=&gp2x_default_font;
 va_list  args;

 va_start(args, format);
 vsprintf(buffer, format, args);

 if(f!=NULL) g=f;

 if(x<0) x=g->x; else g->x=x; 
 if(y<0) y=g->y; else g->y=y;

 for(c=0;buffer[c];c++)
 {
  switch(buffer[c])
  {
   case '\b': g->x=x;g->y=y; break;

   case '\n': g->y+=g->h;
   case '\r': g->x=x;
              break;

   default:   gp2x_printfchar(g, (unsigned char)buffer[c]);
              g->x+=g->w;
              break;
  }
 }

 //gp2x_video_flip_single();
}

void gp2x_printf_init(gp2x_font *f, int w, int h, void *data, int fg, int bg, int solid)
{
 gp2x_printfchar=gp2x_printfchar15;
 f->x=f->y=0;
 f->wmask=1<<(w-1);
 f->w=w;
 f->h=h;
 f->data=(unsigned char *)data;
 f->fg=fg;
 f->bg=bg;
 f->solid=solid;
}

extern void set_save_state(void)
{
	char buffer[260];
	char filename[260];
	time_t curtime;
	struct tm *loctime;
	
	curtime = time (NULL);
	loctime = localtime (&curtime);
	strftime (buffer, 260, "%y%m%d-%I%M%S", loctime);
	sprintf(filename, "%s-%s.svs", packfile, buffer);
	
	GPU_freeze(2, NULL);
	SaveState(filename);
}

void gp2x_video_flip(void)
{
	//memcpy(screenbuffer, videobuffer, 320*240*2);
  updateScreen();
}
