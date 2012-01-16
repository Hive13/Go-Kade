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
#include <errno.h>
#include "kade.h"

extern struct romdata roms[MAX_ROMS];
extern int totalroms;
extern struct config kade_conf;

/* From sql.c */
int check_for_roms(void);

/* From confile.c */
int parse_args(char *str, char ***argvp);

/* Cheesey Bubble sort...will eventually use a qsort */
void sort_roms(void) {
  struct romdata tmproms;
  int cnt1,cnt2;

  for(cnt1=0;cnt1<totalroms-1;cnt1++){
	for(cnt2=0;cnt2<totalroms;cnt2++) {
		if(strcmp(roms[cnt1].name,roms[cnt2].name) < 0) {
			tmproms = roms[cnt1];
			roms[cnt1] = roms[cnt2];
			roms[cnt2] = tmproms;
		}
	}
  }
}

int load_roms(char *xmame) {
  FILE *mamefd;
  char *cmd;
  char *buf;
  char *romname;
  char *romtitle;
  char *display_str;
  int count=0;
  int cnt;

  cmd=(char *)malloc(strlen(xmame)+strlen(MAME_VERIFY) + 3);
  buf=(char *)malloc(MAX_BUF_LINE);
  romname=(char *)malloc(MAX_BUF_LINE);
  romtitle=(char*)malloc(MAX_BUF_LINE);

#if WITH_SQL == 1
  totalroms=check_for_roms();
#endif

  if(totalroms) {  /* We already have roms just load our array */
#if WITH_SQL == 1
	load_roms_from_sql();
#else
	fprintf(stderr, "Error: Had roms but no SQL support, WTF?\n");
	exit(1);
#endif
  } else {
	  
     sprintf(cmd,"%s %s",xmame, MAME_VERIFY);
     mamefd=popen(cmd,"r");
     if(mamefd == NULL) {
	  fprintf(stderr, "ERROR: Couldn't verify working roms :(\n");
	  exit(5);
     }
     display_str=(char *)malloc(MAX_BUF_LINE+1);
     snprintf(display_str, MAX_BUF_LINE, "Loaded MAME roms: %d", count);
     display_string(250,200,display_str);
     while(fgets(buf,MAX_BUF_LINE,mamefd)) {
	  /* FIXME: This is hokey */
	if(strstr(buf, "romset") && strstr(buf," correct")) {
		if(sscanf(buf,"romset %s correct",romname)) {
			roms[count].rom=(char *)malloc(strlen(romname)+1);
			strncpy(roms[count].rom,romname,strlen(romname)+1);
			count++;
			snprintf(display_str, MAX_BUF_LINE, "Loaded MAME roms: %d", count);
  			display_string(250,200,display_str);
		}
	}
     }
     pclose(mamefd);
     totalroms=count;

#if WITH_SQL == 1
     display_string(250,220,"Referencing rom full names and caching to SQL");
#else
     display_string(250,220,"Referencing rom full names");
#endif
     sprintf(cmd, "%s %s",xmame, MAME_LISTFULL);
     mamefd=popen(cmd,"r");
     while(fgets(buf,MAX_BUF_LINE,mamefd)) {
	  if(index(buf, '"')) {
		romtitle=(char *)index(buf,'"');
		romtitle++;
		cnt=strlen(romtitle);
		romtitle[cnt-2]='\0';
		sscanf(buf,"%s",romname);
		if((cnt=get_rom(romname)) != -1) {
#if DEBUG
			fprintf(stderr, "DEBUG: Have rom (%s) : %s\n",romname, romtitle);
#endif
			roms[cnt].name=(char *)malloc(strlen(romtitle)+1);
			strncpy(roms[cnt].name,romtitle,strlen(romtitle)+1);
#if WITH_SQL == 1
			add_rom_to_sql(roms[cnt].rom,roms[cnt].name);
#endif
		}
	  }
     }
     pclose(mamefd);
  
     display_string(250,240,"Sorting Roms");
     sort_roms();
  }
  return 0;
}

int get_rom(char *romname) {
   int cnt;
   for(cnt=0;cnt<totalroms;cnt++) {
	if(!strncmp(roms[cnt].rom,romname,strlen(romname)) &&
	    strlen(roms[cnt].rom) == strlen(romname)) {
		return cnt;
	}
   }
   return -1;
}

void exec_game(int current) {
  int pid,status, strsize;
  //char *mame_args[] = { kade_conf.mame, MAME_OPTS, roms[current].rom, '\0' };
  char *mame_args;
  char **mame_argvp;

  strsize=(strlen(kade_conf.mame)+ strlen(kade_conf.mame_args)+
	  strlen(roms[current].rom) + 4);
  mame_args=(char *)malloc(strsize);
  snprintf(mame_args,strsize,"%s %s %s",kade_conf.mame, kade_conf.mame_args,
		  roms[current].rom);
  parse_args(mame_args, &mame_argvp);
  free(mame_args);

  pid=fork();
  if(pid==0) {  /* Child */
      if(execv(kade_conf.mame,&mame_argvp[0]) == -1) {
	  fprintf(stderr,"Couldn't execute MAME %s\n",roms[current].rom);
      }
      exit(0); /* Kill child when done */
  } else {
	/* Wait */
	if(waitpid(pid, &status, 0) == -1) {
		if(errno != EINTR) {
			fprintf(stderr, "ERROR: Couldn't fork MAME\n");
			return;
		}
	} 
  }
}


