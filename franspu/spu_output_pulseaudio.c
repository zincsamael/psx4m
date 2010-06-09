/***************************************************************************
		pulseaudio.c  -  description
		     -------------------
begin                : Thu Feb 04 2010
copyright            : (C) 2010 by Tristin Celestin
email                : cetris1@umbc.edu
comment              : Much of this was taken from simple.c, in the pulseaudio
                       library
***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#include "spu.h"
#include <minimal.h>

#include <pulse/simple.h>

////////////////////////////////////////////////////////////////////////
// pulseaudio globals
////////////////////////////////////////////////////////////////////////
pa_simple *s;

////////////////////////////////////////////////////////////////////////
// SETUP SOUND
////////////////////////////////////////////////////////////////////////
void SetupSound (void)
{
#ifndef NOSOUND
	pa_sample_spec ss;

	ss.format = PA_SAMPLE_S16LE;
	ss.channels = 1;
	ss.rate = 44100;

	s = pa_simple_new(NULL,             // Use the default server.
					"PSX4ALL",          // Our application's name.
					PA_STREAM_PLAYBACK,
					NULL,               // Use the default device.
					"SFX",              // Description of our stream.
					&ss,                // Our sample format.
					NULL,               // Use default channel map
					NULL,               // Use default buffering attributes.
					NULL                // Ignore error code.
	);

	if (s == NULL)
	{
		SysPrintf ("Could not connect PulseAudio stream");
		return;
	}

     SysPrintf ("PulseAudio should be connected");
     return;
#endif
}

////////////////////////////////////////////////////////////////////////
// REMOVE SOUND
////////////////////////////////////////////////////////////////////////
void RemoveSound (void)
{
#ifndef NOSOUND
	pa_simple_free(s);
#endif
}

////////////////////////////////////////////////////////////////////////
// FEED SOUND DATA
////////////////////////////////////////////////////////////////////////

void SoundFeedStreamData (unsigned char *pSound, long lBytes)
{
#ifndef NOSOUND
	if (pa_simple_write (s, pSound, lBytes, NULL) < 0)
	{
		SysPrintf ("Could not perform PA write");
	}
#endif
}

