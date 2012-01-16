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

#if WITH_SQL == 1
#include <stdio.h>
#include "kade.h"

extern struct romdata roms[MAX_ROMS];
extern struct config kade_conf;

#if WITH_MYSQL == 1
#include <mysql/mysql.h>
#endif


int _check_for_roms_mysql(void) {
#if WITH_MYSQL == 1
   MYSQL *connection;
   MYSQL mysql;
   MYSQL_RES *results;
   MYSQL_ROW row;
   int total=0;
   char *query="select count(rom) from roms";

   if(!mysql_init(&mysql)) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return -1;
   }

   connection = mysql_real_connect(&mysql,
		   kade_conf.db_host,
		   kade_conf.db_user,
		   kade_conf.db_pass,
		   kade_conf.db_name, 0, "", 0);
   if(connection == NULL) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return -1;
   }
   mysql_real_query(&mysql, query, strlen(query));
   results=mysql_use_result(&mysql);
   if(results == NULL) {
	   fprintf(stderr, "Query: %s Failed: %s",query, mysql_error(&mysql));
	   return -1;
   }

   row=mysql_fetch_row(results);
   if(row[0]) 
       total=atoi(row[0]);

   mysql_free_result(results);
   mysql_close(&mysql);
   return total;
#else
	return -1;
#endif
}

void _load_roms_from_sql_mysql(void) {
#if WITH_MYSQL == 1
   MYSQL *connection;
   MYSQL mysql;
   MYSQL_RES *results;
   MYSQL_ROW row;
   int  count=0;
   char *display_str;
   char *query="select * from roms order by romname";

   if(!mysql_init(&mysql)) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return;
   }

   connection = mysql_real_connect(&mysql,
		   kade_conf.db_host,
		   kade_conf.db_user,
		   kade_conf.db_pass,
		   kade_conf.db_name, 0, "", 0);
   if(connection == NULL) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return;
   }
   mysql_real_query(&mysql, query, strlen(query));
   results=mysql_use_result(&mysql);
   if(results == NULL) {
	   fprintf(stderr, "Query: %s Failed: %s",query, mysql_error(&mysql));
	   return;
   }

   display_str=(char *)malloc(MAX_BUF_LINE);
   while((row=mysql_fetch_row(results))) {
	roms[count].rom=(char *)malloc(strlen(row[1])+1);
	strncpy(roms[count].rom,row[1],strlen(row[1])+1);
	roms[count].name=(char *)malloc(strlen(row[2])+1);
	strncpy(roms[count].name,row[2],strlen(row[2])+1);
	count++;
	snprintf(display_str, MAX_BUF_LINE, "Loaded MAME roms from SQL: %d", count);
	display_string(250,200,display_str);
   }
   free(display_str);
   mysql_free_result(results);
   mysql_close(&mysql);
 
#endif
}

void _add_rom_to_sql_mysql(char *rom, char *romname) {
#if WITH_MYSQL == 1
   MYSQL *connection;
   MYSQL mysql;
   MYSQL_RES *results;
   char *query_rom="insert into roms set rom='";
   char *query_romname="', romname='";
   int query_len;
   char *query;

   if(!mysql_init(&mysql)) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return;
   }

   connection = mysql_real_connect(&mysql,
		   kade_conf.db_host,
		   kade_conf.db_user,
		   kade_conf.db_pass,
		   kade_conf.db_name, 0, "", 0);
   if(connection == NULL) {
	   fprintf(stderr, "%s\n", mysql_error(&mysql));
	   return;
   }

   query_len=strlen(query_rom)+strlen(rom)+
	   strlen(query_romname)+strlen(romname)+2;
   query=(char *)malloc(query_len+1);
   snprintf(query, query_len, "%s%s%s%s'",query_rom,rom,query_romname,romname);
   mysql_real_query(&mysql, query, strlen(query));
   free(query);
   results=mysql_use_result(&mysql);
   if(results > 0) {
	   fprintf(stderr, "Query: %s Failed: %s",query, mysql_error(&mysql));
	   return;
   }

   mysql_free_result(results);
   mysql_close(&mysql);
 
#endif
}

/********************************************************/
/*   Generic functions that call specific SQL functions */
/********************************************************/

/* Function test connection to DB and sees if any roms are stored */
/* Returns: -1 on SQL error, 0 if no roms, >0 == number of roms */
int check_for_roms(void) {
	if(strncasecmp(kade_conf.db_type,"MySQL",5) == 0) {
		return _check_for_roms_mysql();
	} else {
		fprintf(stderr, "Unknown SQL type: %s\n",kade_conf.db_type);
		return -1;
	}
}

void load_roms_from_sql(void) {
	if(strncasecmp(kade_conf.db_type,"MySQL",5) == 0) 
		_load_roms_from_sql_mysql();
	else 
		fprintf(stderr, "Unknown SQL type: %s\n",kade_conf.db_type);
}

void add_rom_to_sql(char *rom, char *romname) {
	if(strncasecmp(kade_conf.db_type,"MySQL",5) == 0)
		_add_rom_to_sql_mysql(rom,romname);
	else
		fprintf(stderr, "Unkown SQL type: %s\n",kade_conf.db_type);
}

#endif

