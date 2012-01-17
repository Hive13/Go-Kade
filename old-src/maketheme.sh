#!/bin/sh
THEME=kadeos
THEMEDIR=kadeos
cd $THEMEDIR
tar cvfz $THEME.tgz .
mv $THEME.tgz ../themes
