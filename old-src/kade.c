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
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include "system.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "kade.h"

#define EXIT_FAILURE 1

char *xmalloc ();
char *xrealloc ();
char *xstrdup ();

/* GLOBALS */
char *confile;
time_t idle_time;
int is_playing;
int page;
int cur;
int totalroms=0;
SDL_Surface *screen, *image, *no_image, *listbox;
TTF_Font *font;
struct romdata roms[MAX_ROMS];
struct config kade_conf;

/* from confile.c */
extern int parsedrc;

static void usage (int status);

/* The name the program was run with, stripped of any leading path. */
char *program_name;
SDL_Surface * zoomSurface (SDL_Surface *src, double zoomx, double zoomy, int smooth);

/* getopt_long return codes */
enum {DUMMY_CODE=129
};

/* Option flags and variables */


static struct option const long_options[] =
{
  {"help", no_argument, 0, 'h'},
  {"version", no_argument, 0, 'V'},
  {"fullscreen", no_argument, 0, 'f'},
  {"nosound", no_argument, 0, 'n'},
  {"config", required_argument, 0, 'c'},
  {"theme", required_argument, 0, 't'},
  {NULL, 0, NULL, 0}
};

static int decode_switches (int argc, char **argv);

void redraw_screen(void) {
    SDL_BlitSurface(image, NULL, screen, NULL);
    render_totals();
    list_games(page,cur);
}

int HandleKeys(SDL_Event *event) {
	
  SDL_KeyboardEvent *key=&event->key;
  int quit=0;

  switch(key->keysym.sym){
	  case SDLK_BACKSPACE:
		quit=1;
		break;
	  case SDLK_LCTRL:
		redraw_screen();
		break;
	  case SDLK_KP2:
	  case SDLK_DOWN:
		cur++;
		if(cur >= totalroms) {cur=0; page=0;}
		if(cur >= page+MAX_PER_LIST) page=cur;
		list_games(page,cur);
		break;
	  case SDLK_KP8:
	  case SDLK_UP:
		cur--;
		if(cur < 0) {
			cur=totalroms-1;
			page=(int)(cur/MAX_PER_LIST);
			page=page*MAX_PER_LIST;
		}
		if(cur < page) page=cur-MAX_PER_LIST+1;
		list_games(page,cur);
		break;
	  case SDLK_1:
	  case SDLK_RETURN:
		display_loading();
		if(kade_conf.sound)
			stop_bg_music();
		exec_game(cur);
		redraw_screen();
		if(kade_conf.sound)
			start_bg_music();
		break;
	  case SDLK_c:
		srand(SDL_GetTicks());
		/* Fancy High order way to pick a random rom */
		cur=(int) (((float)totalroms)*rand()/(RAND_MAX+1.0));
		page=(int) (cur/MAX_PER_LIST);
		page=page * MAX_PER_LIST;
		list_games(page,cur);
		break;
  }
  return quit;
}

/* This will display a string on a black box */
/* Ment for a black init screen */
void display_string(int x, int y, char *string) {
   SDL_Rect dstrect;
   SDL_Surface *text_string, *blackbox;
   SDL_Color green = { 0x20, 0xEA, 0x20, 0 };

   dstrect.x=x;
   dstrect.y=y;
   
   text_string=TTF_RenderText_Solid(font, string, green);

   dstrect.w=text_string->w;
   dstrect.h=text_string->h;

   blackbox=SDL_CreateRGBSurface(SDL_SWSURFACE, dstrect.w, dstrect.h,
		screen->format->BitsPerPixel,
		screen->format->Rmask,
		screen->format->Gmask,
		screen->format->Bmask,
		screen->format->Amask);
   SDL_FillRect(blackbox, &dstrect, SDL_MapRGB(screen->format, 0, 0, 0));
   SDL_BlitSurface(text_string, NULL, blackbox, NULL);
   SDL_BlitSurface(blackbox, NULL, screen, &dstrect);
   SDL_UpdateRect(screen, dstrect.x, dstrect.y, dstrect.w, dstrect.h); 
   SDL_FreeSurface(text_string);
   SDL_FreeSurface(blackbox);
}

void init_SDL(void) {
  int vid_flags=SDL_HWSURFACE | SDL_DOUBLEBUF;

  /* Initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	  fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
	  exit(1);
  }
  atexit(SDL_Quit);

  if(kade_conf.sound) {
	  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		  fprintf(stderr, "Couldn't initialize sound: %s\n",SDL_GetError());
		  fprintf(stderr, "Sound will be disabled\n");
		  kade_conf.sound=0;
	  }
  }
  
  if( TTF_Init() < 0) {
	  fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
	  exit(2);
  }
  atexit(TTF_Quit);

  font = TTF_OpenFont(kade_conf.ttf_font, TTF_FONT_SIZE);
  if(font == NULL ) {
	  fprintf(stderr, "Couldn't load %d pt font from %s: %s\n",
			  TTF_FONT_SIZE,kade_conf.ttf_font,SDL_GetError());
	  exit(2);
  }

  if(kade_conf.fullscreen)
  	vid_flags |= SDL_FULLSCREEN;
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, BPP, vid_flags);
  if(!screen) {
	  fprintf(stderr, "Coun't set %dx%d video mode: %s\n", 
			  WIDTH, HEIGHT, SDL_GetError());
	  exit(2);
  }

  SDL_ShowCursor(0);

}

void display_init(void) {
	SDL_Surface *init_screen;
	SDL_Rect dstrect;

	dstrect.x=0;
	dstrect.y=0;
	dstrect.w=WIDTH;
	dstrect.h=HEIGHT;

	init_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, dstrect.w, dstrect.h,
			BPP, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	if(init_screen == NULL) {
		fprintf(stderr, "Couldn't create init screen: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_FillRect(init_screen, &dstrect, SDL_MapRGB(init_screen->format, 0,0,0));
	SDL_BlitSurface(init_screen, NULL, screen, &dstrect);
	SDL_FreeSurface(init_screen);
}

void kade_init(void) {
	struct stat *file_stats;
	char *home;

	file_stats=(struct stat *)malloc(sizeof(struct stat));
	
	/* Check for kaderc config files */
	if(confile == NULL) {
	    home=(char *)malloc(MAX_BUF_LINE);
	    snprintf(home,MAX_BUF_LINE-8,"%s/.kaderc",getenv("HOME"));
	    if(stat(home, file_stats) == 0) {
		    confile=home;
	    } else if (stat(SYSTEMRC, file_stats) == 0) {
		    confile=SYSTEMRC;
	    }
	}
	if(confile) {
		parse_config(confile);
		parsedrc=1;
	}
	if(home)
		free(home);
	
	/* Prepares video screen */
	init_SDL();
	
	/* Displays a loading screen for all the init stuff */
	display_init();
	
	/* Start BG Music */
	if(kade_conf.sound) {
		if(start_bg_music() < 0) {
			fprintf(stderr, "Sound Error: Disabling bg music\n");
			Mix_CloseAudio();
			kade_conf.sound=0;
		}
	}
	
	/* Check Mame install */
	if(stat(kade_conf.mame, file_stats) == -1) {
		fprintf(stderr, "Couldn't stat MAME : %s\n", kade_conf.mame);
		exit(4);
	}
	load_roms(kade_conf.mame);
	if(totalroms < 1) {
		fprintf(stderr,"No roms found.  Verify your roms path in mamerc\n");
		exit(1);
	}

	load_theme();
	free(file_stats);
}

void remove_temp (char *theme_dir) {
   DIR *tmpdir;
   struct dirent *file;
   char *tbuf;

   tbuf=(char *)malloc(255);
   /* Only delete themes stored in /tmp */
   if(strncmp("/tmp",theme_dir, 4) == 0) {
	if(tmpdir=opendir(theme_dir)) {
		while(file=readdir(tmpdir)) {
			if(file->d_type == DT_REG) {
				snprintf(tbuf,254,"%s/%s",theme_dir,file->d_name);
				unlink(tbuf);
			}
		}
		closedir(tmpdir);
		rmdir(theme_dir);
	} else {
		fprintf(stderr, "Error couldn't open stale theme dir %s\n",theme_dir);
	}
   }
   free(tbuf);
}

int
main (int argc, char **argv)
{
  SDL_Event event;
  int i, done=0;
  float zoomx, zoomy;

  /* Set Defaults */
  kade_conf.tmp_dir=DEFAULT_TMPDIR;
  kade_conf.mame=MAME;
  kade_conf.mame_snaps=MAME_SNAPS;
  kade_conf.ttf_font=TTF_FONT;
  kade_conf.theme_dir=KADE_THEMES;
  kade_conf.theme_name=DEFAULT_THEME;
  kade_conf.fullscreen=0;
  kade_conf.sound=DEFAULT_SOUND;
  kade_conf.bg_music=DEFAULT_BGMUSIC;
  kade_conf.compression=1;
  kade_conf.theme.list.x=-1;
  kade_conf.theme.list.y=-1;
  kade_conf.theme.snap.x=-1;
  kade_conf.theme.snap.y=-1;
  kade_conf.theme.snap.h=-1;
  kade_conf.theme.snap.w=-1;
  kade_conf.theme.load.x=-1;
  kade_conf.theme.load.y=-1;
  kade_conf.theme.loading_alpha=255;
  kade_conf.theme.listbox_alpha=255;
#if WITH_SQL==1
  kade_conf.db_type=DEFAULT_DBTYPE;
  kade_conf.db_host=NULL;
  kade_conf.db_port=DEFAULT_DBPORT;
  kade_conf.db_user=DEFAULT_DBUSER;
  kade_conf.db_pass=NULL;
  kade_conf.db_name=DEFAULT_DBNAME;
#endif
  page=0;
  cur=0;
  totalroms=0;
  idle_time=time(NULL);
  
  program_name = argv[0];

  i = decode_switches (argc, argv);

  kade_init();

  image = IMG_Load(kade_conf.theme.background);
  if( image == NULL) {
	  fprintf(stderr, "Couldn't load %s: %s\n", argv[i], SDL_GetError());
	  exit(3);
  }

  if(kade_conf.theme.noimage) {
  	no_image = IMG_Load(kade_conf.theme.noimage);
  	zoomx=(float)kade_conf.theme.snap.w / no_image->w;
  	zoomy=(float)kade_conf.theme.snap.h / no_image->h;
  	no_image=zoomSurface (no_image, zoomx, zoomy, 0);
  }
  if(kade_conf.theme.listbox) 
	  listbox = IMG_Load(kade_conf.theme.listbox);

  SDL_WM_SetCaption("KADE - MAME frontend for cabinets (c) Craig Smith 2003", "kade");
  if( image->format->palette ) {
	  SDL_SetColors(screen, image->format->palette->colors,
			  0, image->format->palette->ncolors);
  }

  SDL_BlitSurface(image, NULL, screen, NULL);
  SDL_UpdateRect(screen, 0, 0, 0, 0);
  
  render_totals();
  list_games(page,cur);

  SDL_EnableKeyRepeat(0,0);
  /* INPUT LOOP */
  while (!done) {
	  while(SDL_PollEvent(&event)) {
		  switch (event.type) {
			  case SDL_KEYUP:
				  idle_time=time(NULL);
				  if(!is_playing && kade_conf.sound)
					  start_bg_music();
				  done=HandleKeys(&event);
				  break;
		  }
	  }
	  if((time(NULL) - idle_time) > DELAY_TIME && is_playing) 
		  stop_bg_music();
  }
  
  remove_temp(kade_conf.theme_dir);
  SDL_Quit();
  exit (0);
}

/* Set all the option flags according to the switches specified.
   Return the index of the first non-option argument.  */

static int
decode_switches (int argc, char **argv)
{
  int c;
  int option_index = 0;


  while ((c = getopt_long (argc, argv, 
			   "f"  /* full screen */
			   "n"  /* Disables sound */
			   "c:" /* config file */
			   "t:" /* theme */
			   "h"	/* help */
			   "V",	/* version */
			   long_options, &option_index)) != EOF)
    {
      switch (c)
	{
	case 'c':
	  if(optarg)
		  confile=optarg;
	  break;
	case 't':
	  if(optarg)
		  kade_conf.theme_name=optarg;
	  break;
	case 'f':
	  kade_conf.fullscreen=1;
	  break;
	case 'n':
	  kade_conf.sound=0;
	  break;
	case 'V':
	  printf ("kade %s\n", VERSION);
	  exit (0);

	case 'h':
	  usage (0);

	default:
	  usage (EXIT_FAILURE);
	}
    }

  return optind;
}


static void
usage (int status)
{
  printf (_("%s - \
MAME frontend designed for cabinets\n"), program_name);
  printf (_("Usage: %s [OPTION]... [FILE]...\n"), program_name);
  printf (_("\
Options:\n\
  -f, --fullscreen           fullscreen\n\
  -n, --nosound              Disables sound\n\
  -c, --config <kaderc>      Kade config file\n\
  -t, --theme <theme>        Theme name (eg: default)\n\
  -h, --help                 display this help and exit\n\
  -V, --version              output version information and exit\n\
"));
  exit (status);
}
