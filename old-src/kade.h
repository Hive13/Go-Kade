#define MAX_ROMS 5000

/* Delay in seconds before turning off sound */
#define DELAY_TIME 60

/* KADE Themes */
#define KADE_THEMES "/usr/share/kade/themes"
#define DEFAULT_THEME "default"

/* DEFAULT SETTINGS */
#define KADE_SYSTEM_DIR "/usr/share/kade"
#define DEFAULT_TMPDIR "/tmp"
#define MAME "/usr/games/xmame"
#define MAME_SNAPS "/usr/lib/games/xmame/snaps"
#define DEFAULT_SOUND 1
#define DEFAULT_BGMUSIC "kade.ogg"
#define DEFAULT_DBTYPE "MYSQL"
#define DEFAULT_DBPORT 3306
#define DEFAULT_DBNAME "kade"
#define DEFAULT_DBUSER "kade"

/* Probably don't need to change these */
#define MAME_VERIFY "-vr"
#define MAME_LISTFULL "-lf"
#define SYSTEMRC "/etc/default/kaderc"

/* TTF Settings */
#define TTF_FONT "/var/lib/defoma/gs.d/dirs/fonts/Comic_Sans_MS.ttf"
#define TTF_FONT_SIZE 12

/* VIDEO SETTINGS */
#define WIDTH 800
#define HEIGHT 600
#define BPP 32

/* CONSTANTS */
#define MAX_BUF_LINE 254
#define MAX_PER_LIST 36
#define DEBUG 0

struct romdata
{
  char *rom;
  char *name;
};

struct rectsize
{
  int x;
  int y;
  int h;
  int w;
};

struct themedata
{
  char *background;     /* Background Image */
  char *loading;	/* Loading... Image */
  char *noimage;	/* (opt) Image to display when there are no snaps */
  char *listbox;	/* (opt) Listbox Image */
  int  loading_alpha;	/* (opt) Alpha blend for Loading... Image */
  int  listbox_alpha;	/* (opt) Alpha blend for listbox */
  struct rectsize list;	/* Cords for the listbox */
  struct rectsize snap; /* Cords for snapshots */
  struct rectsize total;/* Cords for total (only needs x,y) */
  struct rectsize load; /* Cords for loading window (optional) */
  struct rectsize listboxloc; /* Cords for listbox */
};

struct config
{
  char *mame;
  char *mame_args;
  char *mame_snaps;
  char *ttf_font;
  char *theme_dir;
  char *theme_name;
  char *bg_music;
  char *tmp_dir;
  struct themedata theme;
  int  fullscreen;
  int  sound;
  int  compression;
#if WITH_SQL == 1
  char *db_type;
  char *db_host;
  int   db_port;
  char *db_user;
  char *db_pass;
  char *db_name;
#endif
};
