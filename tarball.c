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

#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <string.h>
#include "zlib.h"
#include "tar.h"
#include "kade.h"

/*This code was stolen form untgz.c sample by Pedro Aranda & Jean-loup Gailly*/

int getoct(char *p,int width)
{
  int result = 0;
  char c;
  
  while (width --)
    {
      c = *p++;
      if (c == ' ')
	continue;
      if (c == 0)
	break;
      result = result * 8 + (c - '0');
    }
  return result;
}



int untar (gzFile in, char *tmpdir)
{
  union  tar_buffer buffer;
  int	 action=TGZ_EXTRACT;
  int    len;
  int    err;
  int    getheader = 1;
  int    remaining = 0;
  FILE   *outfile = NULL;
  char   fname[BLOCKSIZE];
  time_t tartime;
  
  while (1)
    {
      len = gzread(in, &buffer, BLOCKSIZE);
      if (len < 0)
	error (gzerror(in, &err));
      /*
       * Always expect complete blocks to process
       * the tar information.
       */
      if (len != BLOCKSIZE)
	error("gzread: incomplete block read");
      
      /*
       * If we have to get a tar header
       */
      if (getheader == 1)
	{
	  /*
	   * if we met the end of the tar
	   * or the end-of-tar block,
	   * we are done
	   */
	  if ((len == 0)  || (buffer.header.name[0]== 0)) break;

	  tartime = (time_t)getoct(buffer.header.mtime,12);
//	  strcpy(fname,buffer.header.name);
	  snprintf(fname,sizeof(fname),"%s/%s",tmpdir,buffer.header.name);
	  
	  switch (buffer.header.typeflag)
	    {
	    case DIRTYPE:
	      if (action == TGZ_EXTRACT)
		mkdir(fname);
	      break;
	    case REGTYPE:
	    case AREGTYPE:
	      remaining = getoct(buffer.header.size,12);
	      if (action == TGZ_EXTRACT)
		{
		  if (remaining)
		    {
		      outfile = fopen(fname,"wb");
		      if (outfile == NULL) {
			/* try creating directory */
			char *p = strrchr(fname, '/');
			if (p != NULL) {
			  *p = '\0';
			  mkdir(fname);
			  *p = '/';
			  outfile = fopen(fname,"wb");
			}
		      }
#if DEBUG == 1
		      fprintf(stderr,
			      "%s %s\n",
			      (outfile) ? "Extracting" : "Couldn't create",
			      fname);
#endif
		    }
		  else
		    outfile = NULL;
		}
	      /*
	       * could have no contents
	       */
	      getheader = (remaining) ? 0 : 1;
	      break;
	    default:
	      break;
	    }
	}
      else
	{
	  unsigned int bytes = (remaining > BLOCKSIZE) ? BLOCKSIZE : remaining;

	  if ((action == TGZ_EXTRACT) && (outfile != NULL))
	    {
	      if (fwrite(&buffer,sizeof(char),bytes,outfile) != bytes)
		{
		  fprintf(stderr,"Error writing %s skipping...\n",fname);
		  fclose(outfile);
		  unlink(fname);
		}
	    }
	  remaining -= bytes;
	  if (remaining == 0)
	    {
	      getheader = 1;
	      if ((action == TGZ_EXTRACT) && (outfile != NULL))
		{
#ifdef WIN32
		  HANDLE hFile;
		  FILETIME ftm,ftLocal;
		  SYSTEMTIME st;
		  struct tm localt;
 
		  fclose(outfile);

		  localt = *localtime(&tartime);

		  hFile = CreateFile(fname, GENERIC_READ | GENERIC_WRITE,
				     0, NULL, OPEN_EXISTING, 0, NULL);
		  
		  st.wYear = (WORD)localt.tm_year+1900;
		  st.wMonth = (WORD)localt.tm_mon;
		  st.wDayOfWeek = (WORD)localt.tm_wday;
		  st.wDay = (WORD)localt.tm_mday;
		  st.wHour = (WORD)localt.tm_hour;
		  st.wMinute = (WORD)localt.tm_min;
		  st.wSecond = (WORD)localt.tm_sec;
		  st.wMilliseconds = 0;
		  SystemTimeToFileTime(&st,&ftLocal);
		  LocalFileTimeToFileTime(&ftLocal,&ftm);
		  SetFileTime(hFile,&ftm,NULL,&ftm);
		  CloseHandle(hFile);

		  outfile = NULL;
#else
		  struct utimbuf settime;

		  settime.actime = settime.modtime = tartime;

		  fclose(outfile);
		  outfile = NULL;
		  utime(fname,&settime);
#endif
		}
	    }
	}
    }
  
  if (gzclose(in) != Z_OK)
    error("failed gzclose");

  return 0;
}


