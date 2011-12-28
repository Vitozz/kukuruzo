#!/bin/bash
homedir=$HOME
srcdir=${homedir}/kukuruzorepo
builddir=${srcdir}/build
exitdir=${srcdir}/debians
data=`LANG=en date +'%a, %d %b %Y %T %z'`
year=`date +'%Y'`
isloop=1
project=""
section=""
arch="all"
depends=""
description=""
descriptionlong=""
addit=""
builddep=""
docfiles=""
dirs=""

if [ ! -z $USERNAME ]
then
	username=$USERNAME
else 
	if [ ! -z $USER ]
	then
		username=$USER
	else
		exit 0
	fi
fi

if [ ! -z $HOST ]
then
	hostname=$HOST
else 
	if [ ! -z $HOSTNAME ]
	then
		hostname=$HOSTNAME
	else
		exit 0
	fi
fi

Maintainer="${username} <${username}@${hostname}>"

check_dir ()
{
	if [ ! -z "$1" ]
	then
		if [ ! -d "$1" ]
		then
			mkdir "$1"
		fi
	fi
}

get_src ()
{
	git clone git://github.com/Vitozz/kukuruzo.git  ${srcdir}
}

prepare_global ()
{
	check_dir ${srcdir}
	get_src
}

prepare_src ()
{
	check_dir ${builddir}
	rm -rf ${builddir}/*
	check_dir ${exitdir}
	if [ ! -z "$1" ]
	then
		dirname="$1"
		ver=`cat ${srcdir}/${dirname}/version.txt`
		cp ${srcdir}/${dirname} ${builddir}/${dirname}-${ver}
		workdir=${builddir}/${dirname}-${ver}
		cd ${workdir}
		check_dir ${workdir}/debian
	fi
}

build_deb ()
{
	dpkg-buildpackage -rfakeroot
}



close ()
{
	isloop=0
}
#
prepare_specs ()
{
changelog="${project} (${version}) unstable; urgency=low

  * new upsream release

 -- ${Maintainer} ${data}"

compat="7"
control="Source: ${project}
Section: ${section}
Priority: extra
Maintainer: ${Maintainer}
Build-Depends: ${builddep}
Standards-Version: 3.8.1
Homepage: http://sites.google.com/site/thesomeprojects/

Package: ${project}
Architecture: ${arch}
${addit}
Depends: ${depends}
Description: ${description}
 descriptionlong
"
copyright="This package was debianized by:

    ${Maintainer} on ${data}

It was downloaded from:

    http://sites.google.com/site/thesomeprojects/

Upstream Author(s):

    Vitaly Tonkacheyev <thetvg@gmail.com>

License:

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This package is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

On Debian systems, the complete text of the GNU General
Public License version 3 can be found in \"/usr/share/common-licenses/GPL-3\".

The Debian packaging is:

    Copyright (C) ${year} ${Maintainer} 

and is licensed under the GPL version 3, see above.

# Please also look if there are files or directories which have a
# different copyright/license attached and list them here.
"
docs=${docfiles}

pyav_prerem='#!/bin/sh
set -e
# Automatically added by dh_pysupport
if which update-python-modules >/dev/null 2>&1; then
	update-python-modules -c  pyalsavolume.public
fi

# End automatically added section

LIST=`find /home -wholename '*/.config/autostart/pyalsavolume.desktop'`

for ONE_OF_LIST in $LIST
do
if [ -f $ONE_OF_LIST ]
then
rm -f $ONE_OF_LIST
fi
done
'
pyversions="2.7"

rules_py='#!/usr/bin/make -f

DEB_PYTHON_SYSTEM=pysupport

include /usr/share/cdbs/1/rules/debhelper.mk
include /usr/share/cdbs/1/class/python-distutils.mk


# Add here any variable or target overrides you need.
'

rules_qt='#!/usr/bin/make -f
# -*- makefile -*-
# Sample debian/rules that uses debhelper.
# This file was originally written by Joey Hess and Craig Small.
# As a special exception, when this file is copied by dh-make into a
# dh-make output file, you may use that output file without restriction.
# This special exception was added by Craig Small in version 0.37 of dh-make.

# Uncomment this to turn on verbose mode.
#export DH_VERBOSE=1
config.status: configure
	dh_testdir

	# Add here commands to configure the package.
	./configure --host=$(DEB_HOST_GNU_TYPE)
	--build=$(DEB_BUILD_GNU_TYPE)
	--prefix=/usr 
  
include /usr/share/cdbs/1/rules/debhelper.mk
include /usr/share/cdbs/1/class/qmake.mk

# Add here any variable or target overrides you need.
QMAKE=qmake-qt4
CFLAGS=-O3
CXXFLAGS=-O3
'
	if [ ! -z "$1" ]
	then
		type="$1"
		if [ type -eq "py" ]
		then
			rules=${rules_py}
		else 
			if [ type -eq "qt" ]
			then
				rules=${rules_qt}
			else
				if [ type -eq "py2" ]
				then
					prerem=${pyav_prerem}
				fi
			fi
		fi
	fi
	echo "${changelog}" > changelog
	echo "${compat}" > compat
	echo "${control}" > control
	echo "${copyright}" > copyright
	echo "${docs}" > docs
	if [ ! -z ${prerem} ]
	then
		echo ${prerem} > prerem
	fi
	echo "${pyversions}" > pyversions
	echo "${pyversions}" > pyversions
	echo "${rules}" > rules
	echo "${dirs} > dirs
}
#

build_erp ()
{
	dirname="exaile-remote-plugin"
	prepare_src ${dirname}
	debdir=${builddir}/${dirname}-${ver}
	cd ${debdir}
	project="exaile-remote-plugin"
	section="sound"
	arch="all"
	builddep="cdbs, debhelper (>= 7), python-support (>= 0.6)"
	addit="XB-Python-Version: \${python:Versions}"
	depends="\${python:Depends}, python-gtk2, exaile (>=0.3.2.0)"
	description="Control Exaile via tray icons"
	descriptionlong='Control Exaile via tray icons.
 Иконки управления плеером в трее.'
	docfiles=""
	dirs="usr/share/exaile/plugins/remote"
	cd ${debdir}/debian
	prepare_specs "py"
	cd ${debdir}
	build_deb	
}

print_menu ()
{
  local menu_text='Choose action TODO!
[1] - Build exaile-remote-plugin debian package
[2] - Build exaile-tunetopsi-plugin debian package
[3] - Build pyalsavolume debian package
[4] - Build pypoweroff debian package
[5] - Build pysshclient debian package
[6] - Build rb-remote-plugin debian package
[7] - Build rb-restore-plugin debian package
[8] - Build rb-tunetopsi-plugin debian package
[9] - Build regexptest debian package
[0] - Exit'
  echo "${menu_text}"
}

choose_action ()
{
  read vibor
  case ${vibor} in
    "1" ) build_erp;;
    "2" ) build_etp;;
    "3" ) build_pyav;;
    "4" ) build_pypoff;;
    "5" ) build_pyssh;;
    "6" ) build_rbremp;;
    "7" ) build_rbresp;;
    "8" ) build_rbtunp;;
    "9" ) build_regext;;
    "0" ) quit;;
  esac
}

cd ${home}
clear
while [ ${isloop} = 1 ]
do
  print_menu
  choose_action
done
exit 0
