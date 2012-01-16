/* 
   kade - MAME frontend designed for cabinets

   Copyright (C) 2003 Craig Smith

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/

#include "config.h"

#include <stdio.h>
#include <sys/stat.h>
#include "SDL.h"
#include "SDL_mixer.h"

#include "kade.h"
   
extern struct config kade_conf;
extern int is_playing;

Mix_Music *music = NULL;

int start_bg_music(void) {
   int audio_channels;
   int audio_buffers;
   Uint16 audio_format;
   int audio_rate;
   struct stat *mstat;
   char *file_loc;

   audio_rate = 22050;
   audio_format = AUDIO_S16;
   audio_channels = 2;
   audio_buffers=4096;
   
   if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
	   fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	   return -1;
   }
   mstat=(struct stat *)malloc(sizeof(struct stat));
   file_loc=(char *)malloc(strlen(KADE_SYSTEM_DIR) + strlen(kade_conf.bg_music) +3);
   snprintf(file_loc,strlen(KADE_SYSTEM_DIR)+strlen(kade_conf.bg_music)+2,
		   "%s/%s",KADE_SYSTEM_DIR, kade_conf.bg_music);
   if(stat(file_loc, mstat) != 0) {
	file_loc=(char *)realloc(file_loc,strlen(kade_conf.bg_music)+3);
	snprintf(file_loc,strlen(kade_conf.bg_music)+3,
			"%s%s","./",kade_conf.bg_music);
	if(stat(file_loc, mstat) != 0) {
		fprintf(stderr, "Couldn't find bg music file %s\n",kade_conf.bg_music);
		free(file_loc);
		free(mstat);
		return -1;
	}
   }
   free(mstat);
   /* At this stage file_loc points to the found music file */
   music = Mix_LoadMUS(file_loc);
   if(music == NULL) {
	   fprintf(stderr, "Couldn't load %s: %s\n", file_loc, SDL_GetError());
	   return -1;
   }
   Mix_FadeInMusic(music,-1,2000);

   is_playing=1;
   return 0;
}

void stop_bg_music(void) {
   Mix_FadeOutMusic(1500);
   Mix_FreeMusic(music);
   music = NULL;
   Mix_CloseAudio();
   is_playing=0;
}
