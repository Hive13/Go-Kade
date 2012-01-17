# kade.spec
#
# Copyright (c) 2003 Craig Smith craig@academicunderground.org
#
%define name kade
%define version 0.1.0
%define release 1
%define manifest %{_builddir}/%{name}-%{version}-%{release}.manifest

# required items
Name: %{name}
Version: %{version}
Release: %{release}
Copyright: GPL
Group: Application/Misc

# optional items
Vendor: Craig Smith
#Distribution:
#Icon:
URL: http://academicunderground.org/kade/
Packager: Craig Smith craig@academicunderground.org

# source + patches
Source: %{name}-%{version}.tar.gz
#Source1:
#Patch:
#Patch1:

# RPM info
#Provides:
Requires: xmame >= 0.56.1
Requires: libsdl1.2debian >= 1.2.3
Requires: libsdl-mixer1.2 >= 1.2.3
Requires: libsdl-image1.2 >= 1.2.2
Requires: libsdl-ttf1.2 >= 1.2.2
Requires: libvorbis0 >= 1.0.0
Requires: libpng2 >= 1.0.12
Requires: zlib1g >= 1:1.1.4
#Conflicts:
#Prereq:

#Prefix: /usr
BuildRoot: /var/tmp/%{name}-%{version}

Summary: MAME frontend designed for cabinets

%description
MAME frontend designed for cabinets
Generated automatically from kade.spec.in by configure.
KADE is a frontend for the MAME emulator.  KADE was designed to run inside
of homemade cabinets.  The focus of KADE is to be able to use it with
an arcade controller and also to have a game like interface.  KADE supports
themes and a SQL backend.

%prep
%setup -q
#%patch0 -p1

%build
%configure
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
%makeinstall

# __os_install_post is implicitly expanded after the
# %install section... do it now, and then disable it,
# so all work is done before building manifest.

%{?__os_install_post}
%define __os_install_post %{nil}

# build the file list automagically into %{manifest}

cd $RPM_BUILD_ROOT
rm -f %{manifest}
find . -type d \
        | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' >> %{manifest}
find . -type f \
        | sed 's,^\.,\%attr(-\,root\,root) ,' >> %{manifest}
find . -type l \
        | sed 's,^\.,\%attr(-\,root\,root) ,' >> %{manifest}

#%pre
#%post
#%preun
#%postun

%clean
rm -f %{manifest}
rm -rf $RPM_BUILD_ROOT

%files -f %{manifest}
%defattr(-,root,root)
#%doc README
#%docdir
#%config

%changelog
