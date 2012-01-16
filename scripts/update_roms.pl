#!/usr/bin/perl
# Simple script to update your rom collection against the SQL database used by Kade
# (c) Craig Smith under the terms and conditions of the GPL
#
# Parses the .kaderc file for DB info

use strict;
use DBI;
use Env "HOME";
use Getopt::Std;

our ($opt_c,$opt_h);
our $HOME;

my $conf;
my $dbtype="mysql";
my $dbhost="127.0.0.1";
my $dbport="3306";
my $dbuser="kade";
my $dbpass;
my $dbname="kade";
my $xmame;
my %mame_roms;
my %sql_roms;
my $dbh;
my $sth;
my $line;
my $query;
my @row;
my @add_roms;
my @remove_roms;

getopts('hc:');

Usage() if($opt_h);

$conf="/etc/default/kaderc" if -r "/etc/default/kaderc";
$conf="$HOME/.kaderc" if -r "$HOME/.kaderc";
$conf=$opt_c if $opt_c;

if(!$conf) {
   print "Couldnt' find kaderc file...needed to read in DB info.\n";
   print "Perhaps you need the -c option.\n";
   exit(1);
}
   
print "Parsing $conf...\n";
open CONF, $conf || die "Couldn't open $conf: $!";
while(<CONF>) {
   $line=$_;
   $dbhost=$1 if $line=~/^dbhost=(\S*)/;
   $dbport=$1 if $line=~/^dbport=(\S*)/;
   $dbuser=$1 if $line=~/^dbuser=(\S*)/;
   $dbpass=$1 if $line=~/^dbpass=(\S*)/;
   $dbtype=$1 if $line=~/^dbtype=(\S*)/;
   $dbname=$1 if $line=~/^dbname=(\S*)/;
   $xmame=$1 if $line=~/^mame=(\S*)/;
}
close CONF;

$xmame="/usr/games/xmame" if !$xmame;
if (! -x $xmame) {
	print "Couldn't find MAME in $xmame or in kaderc file!\n";
	exit(1);
}

print "Getting installed roms from SQL ...\n";
$dbtype=lc($dbtype);
my $dbistring="DBI:$dbtype:database=$dbname:host=$dbhost:port=$dbport";
$dbh = DBI->connect($dbistring, $dbuser, $dbpass, { RaiseError => 1, AutoCommit => 1 });
$query="select rom from roms";
$sth=$dbh->prepare($query);
$sth->execute;

while(@row=$sth->fetchrow_array){
    $sql_roms{$row[0]}=1;
}

$sth->finish;

print "Getting all verfied MAME roms (takes a while) ...\n";
open MAME, "$xmame -vr 2>/dev/null|" || die "Couldn't run $xmame: $!\n";
while(<MAME>) {
   $line=$_;
   $mame_roms{$1}=1 if $line=~/romset (\w*) correct/
}
close MAME;

foreach my $rom (keys %mame_roms) {
    if(!$sql_roms{$rom}) {
	push @add_roms, $rom;
    }
}
foreach my $rom (keys %sql_roms) {
    if(!$mame_roms{$rom}) {
	push @remove_roms, $rom;
    }
}

print "Removing roms from SQL that don't exist or can't be verified anymore ...\n" if @remove_roms;;
foreach my $rom (@remove_roms) {
	$query="delete from roms where rom='".$rom."'";
	$sth=$dbh->prepare($query);
	$sth->execute;
	print "   ... Deleted $rom\n";
	$sth->finish;
}

if(@add_roms) {
	print "Looking up full names to new rom games ...\n";
	open MAME, "$xmame -lf 2>/dev/null|" || die "Couldn't run $xmame: $!\n";
	while(<MAME>) {
		$line=$_;
		if($line=~/^(\w*)(\s*)\"(.*)\"/) {
			$mame_roms{$1}=$3;
		}
	}
	close MAME;
	print "Adding roms into SQL ... \n";
	foreach my $rom (@add_roms) {
		$mame_roms{$rom}=~s/'/\\'/g;
		$query="insert into roms values(NULL,'".$rom."','".$mame_roms{$rom}."')";
		$sth=$dbh->prepare($query);
		$sth->execute;
		print "   ... Added $rom \"".$mame_roms{$rom}."\"\n";
		$sth->finish;
	}
}
print "SQL Database is now in sync.\n";
print "\nEnjoy KADE!\n";

## SUBS ##
#
sub Usage() {
  print <<EOUSAGE;
  $0 [options]

  -c <kaderc>  Alternate config file
  -h           You're looking at it

EOUSAGE
  exit(1);
}
