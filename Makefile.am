bin_PROGRAMS=kade

kade_SOURCES=kade.c kade.6 kaderc.7 xmalloc.c system.h aclocal.m4 getopt.c getopt1.c getopt.h mame.c menu.c SDL_rotozoom.c confile.c sound.c tarball.c sql.c

#  uncomment the following if kade requires the math library
#kade_LDADD=-lm

EXTRA_DIST=kade.lsm.in kade.spec.in kade.texinfo

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
info_TEXINFOS = kade.texinfo
kade_TEXINFOS = gpl.texinfo

#  install the man pages
man_MANS=kade.6 kaderc.7

install: kade
	cp -f kade $(bindir)/kade
	chmod 755 $(bindir)/kade
	strip $(bindir)/kade
	mkdir -p $(datadir)/kade/themes
	cp -f themes/*.tgz $(datadir)/kade/themes/
	cp -f kade.ogg $(datadir)/kade/
	cp -f kade.6 $(mandir)/man6/
	cp -f kaderc.7 $(mandir)/man7/
	mkdir -p $(datadir)/doc/kade
	cp -f README $(datadir)/doc/kade/
	cp -f README.Themes $(datadir)/doc/kade/
	cp -f BUGS $(datadir)/doc/kade/
	cp -f TODO $(datadir)/doc/kade/
	cp -f ChangeLog $(datadir)/doc/kade/
	cp -f mysql_kade.sql $(datadir)/doc/kade/

uninstall: kade
	rm -f $(bindir)/kade
	rm -rf $(datadir)/kade
	rm -f $(mandir)/man6/kade.6
	rm -f $(mandir)/man7/kaderc.7
	rm -rf $(datadir)/doc/kade

