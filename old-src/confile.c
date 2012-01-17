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
#include <string.h>
#include "kade.h"

extern struct config kade_conf;

int parsedrc = 0;

/* Removes any \r\n and replaces it with NULL \0 */
void chomp(char *line) {
   char c;
   int len;
   for(len=0;len < strlen(line);len++) 
	if(line[len]=='\r' || line[len]=='\n')
		line[len]='\0';
}

int parse_config(char *file) {
  FILE *fp;
  char *line;
  char *entry;
  
  if(fp=fopen(file,"r")) {
#if DEBUG == 1
	  fprintf(stderr,"Parsing file %s.\n",file);
#endif
     line=(char *)malloc(MAX_BUF_LINE+1);
     while(fgets(line,MAX_BUF_LINE,fp) != NULL) {
	if(line[0]!='#' && line[0]!='\n' && line[0]!='\r') {
	   chomp(line);		
	   /*  Any variables you don't want themes to alter use parsedrc */
	   if((entry=strstr(line,"mame=")) && !parsedrc) {
		entry+=strlen("mame=");
		kade_conf.mame=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.mame,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"mame_args=")) && !parsedrc) {
		entry+=strlen("mame_args=");
		kade_conf.mame_args=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.mame_args,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"mame_snaps=")) && !parsedrc) {
		entry+=strlen("mame_snaps=");
		kade_conf.mame_snaps=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.mame_snaps,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"ttf_font=")) && !parsedrc) {
		entry+=strlen("ttf_font=");
		kade_conf.ttf_font=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.ttf_font,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"fullscreen=")) && !parsedrc) {
		entry+=strlen("fullscreen=");
		if(entry[0]=='y' || entry[0]=='Y' || entry[0]=='1') 
			kade_conf.fullscreen=1;
	   } else if((entry=strstr(line,"sound=")) && !parsedrc) {
		entry+=strlen("sound=");
		if(entry[0]=='n' || entry[0]=='N' || entry[0]=='0') 
			kade_conf.fullscreen=0;
	   } else if((entry=strstr(line,"compression=")) && !parsedrc) {
		entry+=strlen("compression=");
		if(entry[0]=='n' || entry[0]=='N' || entry[0]=='0') 
			kade_conf.compression=0;
	   } else if((entry=strstr(line,"theme_dir=")) && !parsedrc) {
		entry+=strlen("theme_dir=");
		kade_conf.theme_dir=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.theme_dir,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"theme=")) && !parsedrc) {
		entry+=strlen("theme=");
		kade_conf.theme_name=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.theme_name,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"tmpdir=")) && !parsedrc) {
		entry+=strlen("tmpdir=");
		kade_conf.tmp_dir=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.tmp_dir,entry,strlen(entry)+1);
	   } else if(entry=strstr(line,"background=")) {
		entry+=strlen("background=");
		kade_conf.theme.background=(char *)malloc(strlen(entry)+strlen(kade_conf.theme_dir)+2);
		sprintf(kade_conf.theme.background,"%s/%s",kade_conf.theme_dir,entry);
	   } else if(entry=strstr(line,"loading=")) {
		entry+=strlen("loading=");
		kade_conf.theme.loading=(char *)malloc(strlen(entry)+strlen(kade_conf.theme_dir)+2);
		sprintf(kade_conf.theme.loading,"%s/%s",kade_conf.theme_dir,entry);
	   } else if(entry=strstr(line,"noimage=")) {
		entry+=strlen("noimage=");
		kade_conf.theme.noimage=(char *)malloc(strlen(entry)+strlen(kade_conf.theme_dir)+2);
		sprintf(kade_conf.theme.noimage,"%s/%s",kade_conf.theme_dir,entry);
	   } else if(entry=strstr(line,"listbox=")) {
		entry+=strlen("listbox=");
		kade_conf.theme.listbox=(char *)malloc(strlen(entry)+strlen(kade_conf.theme_dir)+2);
		sprintf(kade_conf.theme.listbox,"%s/%s",kade_conf.theme_dir,entry);
	   } 
#if WITH_SQL == 1
	   else if((entry=strstr(line,"dbtype=")) && !parsedrc) {
		entry+=strlen("dbtype=");
		kade_conf.db_type=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.db_type,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"dbhost=")) && !parsedrc) {
		entry+=strlen("dbhost=");
		kade_conf.db_host=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.db_host,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"dbuser=")) && !parsedrc) {
		entry+=strlen("dbuser=");
		kade_conf.db_user=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.db_user,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"dbpass=")) && !parsedrc) {
		entry+=strlen("dbpass=");
		kade_conf.db_pass=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.db_pass,entry,strlen(entry)+1);
	   } else if((entry=strstr(line,"dbname=")) && !parsedrc) {
		entry+=strlen("dbname=");
		kade_conf.db_name=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.db_name,entry,strlen(entry)+1);
	   } else if((entry=strstr(line, "dbport=")) && !parsedrc) {
		entry+=strlen("dbport=");
		sscanf(entry,"%d",&kade_conf.db_port);
	   }
#endif
	   else if(entry=strstr(line,"bgmusic=")) {
		entry+=strlen("bgmusic=");
		kade_conf.bg_music=(char *)malloc(strlen(entry)+1);
		strncpy(kade_conf.bg_music,entry,strlen(entry)+1);
	   } else if(entry=strstr(line,"loading_alpha=")) {
		entry+=strlen("loading_alpha=");
		sscanf(entry,"%d",&kade_conf.theme.loading_alpha);
	   } else if(entry=strstr(line,"listbox_alpha=")) {
		entry+=strlen("listbox_alpha=");
		sscanf(entry,"%d",&kade_conf.theme.listbox_alpha);
	   } else if(entry=strstr(line,"listx=")) {
		entry+=strlen("listx=");
		sscanf(entry,"%d",&kade_conf.theme.list.x);
	   } else if(entry=strstr(line,"listy=")) {
		entry+=strlen("listy=");
		sscanf(entry,"%d",&kade_conf.theme.list.y);
	   } else if(entry=strstr(line,"listw=")) {
		entry+=strlen("listw=");
		sscanf(entry,"%d",&kade_conf.theme.list.w);
	   } else if(entry=strstr(line,"listh=")) {
		entry+=strlen("listh=");
		sscanf(entry,"%d",&kade_conf.theme.list.h);
	   } else if(entry=strstr(line,"listboxx=")) {
		entry+=strlen("listboxx=");
		sscanf(entry,"%d",&kade_conf.theme.listboxloc.x);
	   } else if(entry=strstr(line,"listboxy=")) {
		entry+=strlen("listboxy=");
		sscanf(entry,"%d",&kade_conf.theme.listboxloc.y);
	   } else if(entry=strstr(line,"loadingx=")) {
		entry+=strlen("loadingx=");
		sscanf(entry,"%d",&kade_conf.theme.load.x);
	   } else if(entry=strstr(line,"loadingy=")) {
		entry+=strlen("loadingy=");
		sscanf(entry,"%d",&kade_conf.theme.load.y);
	   } else if(entry=strstr(line,"snapx=")) {
		entry+=strlen("snapx=");
		sscanf(entry,"%d",&kade_conf.theme.snap.x);
	   } else if(entry=strstr(line,"snapy=")) {
		entry+=strlen("snapy=");
		sscanf(entry,"%d",&kade_conf.theme.snap.y);
	   } else if(entry=strstr(line,"snapw=")) {
		entry+=strlen("snapw=");
		sscanf(entry,"%d",&kade_conf.theme.snap.w);
	   } else if(entry=strstr(line,"snaph=")) {
		entry+=strlen("snaph=");
		sscanf(entry,"%d",&kade_conf.theme.snap.h);
	   } else if(entry=strstr(line,"totalx=")) {
		entry+=strlen("totalx=");
		sscanf(entry,"%d",&kade_conf.theme.total.x);
	   } else if(entry=strstr(line,"totaly=")) {
		entry+=strlen("totaly=");
		sscanf(entry,"%d",&kade_conf.theme.total.y);
	   } else if((entry=strstr(line,"include ")) && !parsedrc) {
		entry+=strlen("include ");
		parse_config(entry);
	   } else {
		fprintf(stderr, "Unrecognized line in file %s:\n%s\n",
				file,line);
		exit(1);
	   }
	}
     }
     fclose(fp);
  } else {
	fprintf(stderr, "Couldn't open %s, reverting to defaults\n",file);
	return -1;
  }
}

int parse_args(char *str, char ***argvp) {
   char *snew;
   char *temp;
   int numtokens,i;

   snew = str + strspn(str, " ");
   
   if((temp = calloc(strlen(snew) +1, sizeof(char))) == NULL) {
	   *argvp = NULL;
	   numtokens = -1;
   } else {
	   strncpy(temp, snew, strlen(snew)+1);
	   if(strtok(temp, " ") == NULL)
		   numtokens = 0;
	   else
		   for (numtokens = 1; strtok(NULL, " ") != NULL;
				   numtokens++)
			   ;
	   if ((*argvp = calloc(numtokens + 1, sizeof(char *))) == NULL) {
		   free(temp);
		   numtokens = -1;
	   } else {
		   if (numtokens > 0) {
			   strcpy(temp, snew);
			   **argvp = strtok(temp, " ");
			   for (i = 1; i < numtokens + 1; i++)
				   *((*argvp) + i) = strtok(NULL, " ");
		   } else {
			   **argvp = NULL;
			   free(temp);
		   }
	   }
   }
   return numtokens;
}
