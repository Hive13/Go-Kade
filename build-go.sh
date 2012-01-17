#!/bin/sh
COMP=6g   # 8g is 32bit
LINKER=6l # 8l is 32bit
rm *.6
$COMP settings.go
$COMP kade.go
$LINKER -o kade kade.6
