// Package is a list of configuration settings and defaults

package settings

const (
	DELAY_TIME      = 60
	DEFAULT_TMPDIR  = "/tmp"
	MAME            = "/usr/games/xmame"
	KADE_SYSTEM_DIR = "/usr/share/kade"
	MAME_SNAPS      = "/usr/lib/games/xmame/snaps"
	DEFAULT_SOUND   = true
	DEFAULT_BGMUSIC = "kade.ogg"
	DEFAULT_DBTYPE  = "MYSQL"
	DEFAULT_DBPORT  = 3306
	DEFAULT_DBNAME  = "kade"
	DEFAULT_DBUSER  = "kade"
	TTF_FONT        = "/var/lib/defoma/gs.d/dirs/fonts/Comic_Sans_MS.ttf"
	TTF_FONT_SIZE   = 12
)

type Config struct {
	Mame        string
	Mame_args   string
	Mame_snaps  string
	Ttf_font    string
	Bg_music    string
	Tmp_dir     string
	Fullscreen  bool
	Sound       bool
	Compression int
	Db_type     string
	Db_host     string
	Db_port     int
	Db_user     string
	Db_pass     string
	Db_name     string
}
