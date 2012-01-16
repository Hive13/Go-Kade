CREATE TABLE roms (
  romid int(11) NOT NULL auto_increment,
  rom varchar(50) NOT NULL default '',
  romname varchar(255) default NULL,
  PRIMARY KEY  (romid),
  KEY rom (rom)
)
