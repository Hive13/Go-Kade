/* 
   Go-kade - MAME frontend designed for cabinets

   Copyright (C) 2011 Hive13
   Based on Kade (C) 2003 Craig Smith

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

package main

import (
	"./settings"
	"fmt"
	"flag"
	"os"
)

const VERSION = "0.2.0"
// Globals
var kade_conf = new(settings.Config)
var showVersion = flag.Bool("version", false, "Version info")

func init() {
	/* Set Defaults */
	kade_conf.Tmp_dir = settings.DEFAULT_TMPDIR
	kade_conf.Mame = settings.MAME
	kade_conf.Mame_snaps = settings.MAME_SNAPS
	kade_conf.Ttf_font = settings.TTF_FONT
	kade_conf.Bg_music = settings.DEFAULT_BGMUSIC
	kade_conf.Db_type = settings.DEFAULT_DBTYPE
	kade_conf.Db_host = ""
	kade_conf.Db_user = settings.DEFAULT_DBUSER
	kade_conf.Db_pass = ""
	kade_conf.Db_name = settings.DEFAULT_DBNAME

	flag.IntVar(&kade_conf.Db_port, "dbport", settings.DEFAULT_DBPORT, "Sets the database port number")
	flag.BoolVar(&kade_conf.Sound, "sound", settings.DEFAULT_SOUND, "Enables or disables sound")
	flag.BoolVar(&kade_conf.Fullscreen, "fs", false, "Enables fullscreen mode")
}

func main() {
	flag.Parse()

	if *showVersion {
		fmt.Printf("GoKade v%s\n", VERSION)
		os.Exit(0)
	}

}
