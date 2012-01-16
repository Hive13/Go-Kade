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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "kade.h"
#include "zlib.h"

extern struct romdata roms[MAX_ROMS];
extern int totalroms;
extern SDL_Surface *screen;
extern SDL_Surface *image, *no_image, *listbox;
extern TTF_Font *font;
extern struct config kade_conf;

SDL_Surface * zoomSurface (SDL_Surface *src, double zoomx, double zoomy, int smooth);

void load_theme(void) {
    char *theme_ball;
    char *theme_conf;
    gzFile tgzfile;
    //char tmpdir[]="/tmp/kade-XXXXXX";
    char *tmpdir;

    tmpdir=(char *)malloc(strlen(kade_conf.tmp_dir) + 14);
    theme_ball=(char *)malloc(MAX_BUF_LINE+1);
    theme_conf=(char *)malloc(MAX_BUF_LINE+1);

    snprintf(tmpdir,strlen(kade_conf.tmp_dir)+14,"%s/kade-XXXXXX",kade_conf.tmp_dir);

    /* Unzip file */
    if(kade_conf.compression) {
       snprintf(theme_ball,MAX_BUF_LINE,"%s/%s.tgz",kade_conf.theme_dir,kade_conf.theme_name);
       tgzfile=gzopen(theme_ball,"rb");
       if (tgzfile == NULL) {
	    fprintf(stderr, "Couldn't open theme ball: %s\n",theme_ball);
	    exit(1);
       }
       mkdtemp(tmpdir);
       untar(tgzfile, tmpdir);
       kade_conf.theme_dir=(char *)malloc(strlen(tmpdir)+1);
       strncpy(kade_conf.theme_dir,tmpdir,strlen(tmpdir)+1);
       free(theme_ball);
    }

    /* Load theme's config file */
    sprintf(theme_conf,"%s/theme.conf",kade_conf.theme_dir);
    parse_config(theme_conf);
    free(theme_conf);
    free(tmpdir);

    /* Ensure we have enough data */
    if(!kade_conf.theme.background ||
       !kade_conf.theme.loading ||
       !kade_conf.theme.list.x < 0 ||
       !kade_conf.theme.list.y < 0 ||
       !kade_conf.theme.snap.x < 0 ||
       !kade_conf.theme.snap.y < 0 ||
       !kade_conf.theme.snap.w < 0 ||
       !kade_conf.theme.snap.h < 0 ) {
	    fprintf(stderr, ">>>Missing fields in theme.conf can't load!<<<\n"
			    "background=%s\n"
			    "loading=%s\n"
			    "listx=%d listy=%d\n"
			    "snapx=%d snapy=%d snapw=%d snaph=%d\n",
			    kade_conf.theme.background,
			    kade_conf.theme.loading,
			    kade_conf.theme.list.x,
			    kade_conf.theme.list.y,
			    kade_conf.theme.snap.x,
			    kade_conf.theme.snap.y,
			    kade_conf.theme.snap.w,
			    kade_conf.theme.snap.h);
	    exit(1);
    }
}

void draw_listbox(void) {
   SDL_Rect dstrect;
   SDL_Surface *alpha_surf;

   dstrect.x=kade_conf.theme.listboxloc.x;
   dstrect.y=kade_conf.theme.listboxloc.y;
   dstrect.h=listbox->h;
   dstrect.w=listbox->w;

   if(kade_conf.theme.listbox_alpha < 255) {
	alpha_surf = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
			listbox->w, listbox->h, screen->format->BitsPerPixel,
			screen->format->Rmask,
			screen->format->Gmask,
			screen->format->Bmask,
			screen->format->Amask);
	if(alpha_surf != NULL) {
   		SDL_SetAlpha(alpha_surf,SDL_SRCALPHA, kade_conf.theme.listbox_alpha);
		SDL_BlitSurface(listbox, NULL, alpha_surf, NULL);
		SDL_BlitSurface(alpha_surf, NULL, screen, &dstrect);
		SDL_FreeSurface(alpha_surf);
	}
   } else 
   	SDL_BlitSurface(listbox, NULL, screen, &dstrect);

}

void load_snap(int current) {
    char *snap;
    struct stat *snap_stat;
    SDL_Surface *picture, *backdrop;
    SDL_Rect dstrect;
    float zoomx,zoomy;

    snap_stat=(struct stat *)malloc(sizeof(struct stat));
    snap=(char *)malloc(strlen(kade_conf.mame_snaps)+1+strlen(roms[current].rom)+5);

    sprintf(snap,"%s/%s.png",kade_conf.mame_snaps,roms[current].rom);
   
    /* Our box is 304x350ish starting at 437 */
    dstrect.x=kade_conf.theme.snap.x;
    dstrect.y=kade_conf.theme.snap.y;
    dstrect.h=kade_conf.theme.snap.h;
    dstrect.w=kade_conf.theme.snap.w;
 
    /* Sanity Check */
    if(dstrect.x > screen->w) dstrect.x=screen->w;
    if(dstrect.y > screen->h) dstrect.y=screen->h;
    if(dstrect.x < 0) dstrect.x = 0;
    if(dstrect.y < 0) dstrect.y = 0;
 
    if(stat(snap, snap_stat) != -1) {
	backdrop = SDL_CreateRGBSurface(SDL_SWSURFACE, dstrect.w,dstrect.h,
			screen->format->BitsPerPixel,
			screen->format->Rmask,
			screen->format->Gmask,
			screen->format->Bmask,
			screen->format->Amask);
	SDL_FillRect(backdrop,&dstrect, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_BlitSurface(backdrop, NULL, screen, &dstrect);
	SDL_FreeSurface(backdrop);
	picture=IMG_Load(snap);
	zoomx=(float)kade_conf.theme.snap.w / picture->w;
	zoomy=(float)kade_conf.theme.snap.h / picture->h;
	picture=zoomSurface (picture, zoomx, zoomy, 0);
	dstrect.h=picture->h;
	dstrect.w=picture->w;
	SDL_BlitSurface(picture, NULL, screen, &dstrect);
	SDL_FreeSurface(picture);
    } else {  /* Display default snapshot */
	if(no_image != NULL) {
		dstrect.h=no_image->h;
		dstrect.w=no_image->w;
		SDL_BlitSurface(no_image, NULL, screen, &dstrect);
	} else {
		SDL_BlitSurface(image, &dstrect, screen, &dstrect);
	}
    }
 }

void render_totals(void) {
  SDL_Surface *totals_scn;
  SDL_Rect dstrect;
  SDL_Color green  = { 0x20, 0xEA, 0x19, 0 };
  char tstring[100];

  sprintf(tstring, "Total Games: %d",totalroms);

  totals_scn = TTF_RenderText_Solid(font,tstring,green);

  if(kade_conf.theme.total.x)
	  dstrect.x = kade_conf.theme.total.x;
  else
	  dstrect.x = 560;
  if(kade_conf.theme.total.y)
	  dstrect.y = kade_conf.theme.total.y;
  else
  	  dstrect.y = 550;
  dstrect.w = totals_scn->w;
  dstrect.h = totals_scn->h;

  if(totals_scn != NULL) {
	SDL_BlitSurface(totals_scn, NULL, screen, &dstrect);
	SDL_FreeSurface(totals_scn);
  }
}

void list_games(int start, int current) {
  SDL_Surface *text = NULL;
  SDL_Rect dstrect,srcrect;
  SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
  SDL_Color black = { 0x00, 0x00, 0x00, 0 };
  int cnt;
  
  dstrect.x = kade_conf.theme.list.x;
  dstrect.y = kade_conf.theme.list.y;
  if(kade_conf.theme.list.w)
	  dstrect.w = kade_conf.theme.list.w;
  else
  	  dstrect.w = 310; 
  if(kade_conf.theme.list.h)
	  dstrect.h = kade_conf.theme.list.h;
  else
  	  dstrect.h = 517;

  /* Sanity Checks */
  if(current >= totalroms) current=totalroms-1;
  if(start >= totalroms) start=totalroms-1;
  if(current < 0) current=0;
  if(start < 0) start=0;
  if(dstrect.x < 0) dstrect.x = 0;
  if(dstrect.x > screen->w) dstrect.x=screen->w;
  if(dstrect.y < 0) dstrect.y = 0;
  if(dstrect.y > screen->h) dstrect.y=screen->h;

  srcrect.x=0;
  srcrect.y=0;
  srcrect.w=dstrect.w;
  
  load_snap(current);

  /* Optional additional listbox image */
  if(kade_conf.theme.listbox)
	  draw_listbox();
  else
  	SDL_BlitSurface(image, &dstrect, screen, &dstrect);

  for(cnt=start;cnt<start+MAX_PER_LIST&&cnt<totalroms;cnt++) {
	if(cnt == current) 
		TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	else
		TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
	text = TTF_RenderText_Solid(font, roms[cnt].name, black);
		
  	dstrect.w = text->w;
        dstrect.h = text->h;
	srcrect.h = text->h;
	if(text != NULL) {
		SDL_BlitSurface(text, &srcrect, screen, &dstrect);
		SDL_FreeSurface(text);
		dstrect.y+=TTF_FONT_SIZE+2;
		if(dstrect.y > screen->h) dstrect.y=screen->h-TTF_FONT_SIZE-2;
	}
  }
//  SDL_UpdateRect(screen, 0, 0, screen->w,screen->h);
  SDL_Flip(screen);
   
}

void display_loading(void) {
   SDL_Surface *loading,*alpha_surf;
   SDL_Rect dstrect;

   loading=IMG_Load(kade_conf.theme.loading);
   
   if(kade_conf.theme.load.x >= 0)
	dstrect.x = kade_conf.theme.load.x;
   else
	dstrect.x = (int)((image->w/2)-(loading->w/2));
   if(kade_conf.theme.load.y >= 0)
	dstrect.y = kade_conf.theme.load.y;
   else
	dstrect.y = (int)((image->h/2)-(loading->h/2));
   dstrect.w = loading->w;
   dstrect.h = loading->h;

   if(kade_conf.theme.loading_alpha < 255) {
        alpha_surf = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA,
		   loading->w, loading->h, screen->format->BitsPerPixel,
		   screen->format->Rmask,
		   screen->format->Gmask,
		   screen->format->Bmask,
		   screen->format->Amask);
       if(alpha_surf != NULL) {
        	//SDL_FillRect(alpha_surf, NULL, SDL_MapRGB(alpha_surf->format, 0x20, 0xEA, 0x20));
		SDL_SetAlpha(alpha_surf, SDL_SRCALPHA, kade_conf.theme.loading_alpha);
		SDL_BlitSurface(loading, NULL, alpha_surf, NULL);
		SDL_FreeSurface(loading);
		SDL_BlitSurface(alpha_surf, NULL, screen, &dstrect);
		SDL_FreeSurface(alpha_surf);
       } else {
	   fprintf(stderr, "Couldn't create alpha level for Loading msg!\n");
       }
   } else {
	       SDL_BlitSurface(loading, NULL, screen, &dstrect);
	       SDL_FreeSurface(loading);
   }
   SDL_UpdateRect(screen, dstrect.x, dstrect.y, dstrect.w, dstrect.h);
}

