#!/bin/bash
#
# ---------README----------------------------------------------------------
# To build debian packages with this script you need
# to install packages: cdbs, debhelper, dpkg-dev, devscripts
# just run command:
# sudo apt-get install cdbs debhelper dpkg-dev devscripts
# to build debian package of regexptest project you also need
# to install packages: libqt4-dev, qconf 
# just run command:
# sudo apt-get install libqt4-dev qconf
# -------------------------------------------------------------------------
#
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

CWDIR=`pwd`
echo "Current workdir ${CWDIR}"

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

Maintainer="Vitaly Tonkacheyev <thetvg@gmail.com>"

run_resloader ()
{
	if [ ! -z "$1" ]
	then
		cmd=$1
		if [ -f "${CWDIR}/resloader.sh" ]
		then
			cd ${CWDIR}
			. ./resloader.sh
			$cmd
		else
			echo "No resloder.sh file found. Please download it with this script in the same dir"
			quit
		fi
	fi
}

rm_all ()
{
	cd ${homedir}
	if [ -d "${srcdir}" ]
	then
		rm -rf ${srcdir}
	fi
}

get_all ()
{
	cd ${homedir}
	run_resloader get_src
	run_resloader get_pyav
	run_resloader get_pypoff
	run_resloader get_pyssh
	run_resloader get_html2text
	run_resloader get_avolume
}

check_qconf ()
{
	if [ -f "/usr/bin/qconf" -o -f "/usr/local/bin/qconf" ]
	then
		qconfcmd=qconf
	fi
	if [ -f "/usr/bin/qt-qconf" -o -f "/usr/local/bin/qt-qconf" ]
	then
		qconfcmd=qt-qconf
	else
		echo "Install Please qconf utility to continue"
	fi
}

prepare ()
{
	run_resloader "set_workdir ${project}-${ver}"
	run_resloader "prepare_src ${dirname}"
	run_resloader "check_dir ${debdir}/debian"
}

build_deb ()
{
	dpkg-buildpackage -rfakeroot
}

quit ()
{
	isloop=0
}
#
prepare_specs ()
{
changelog="${project} (${ver}-1) unstable; urgency=low

  * new upsream release

 -- ${username} <${username}@gmail.com>  ${data}"

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
 ${descriptionlong}
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
CXXFLAGS=-O3'

	if [ "${project}" != "pyalsavolume" -a "${project}" != "regexptest" ]
	then
		echo "${rules_py}" > rules
		echo "${pyversions}" > pyversions
	fi 
	if [ "${project}" == "regexptest" -o "${project}" == "alsavolume" ]
	then
		echo "${rules_qt}" > rules
	fi
	if [ "${project}" == "pyalsavolume" ]
	then
		echo "${rules_py}" > rules
		echo "${pyav_prerem}" > prerm
		echo "${pyversions}" > pyversions
	fi
	echo "${changelog}" > changelog
	echo "${compat}" > compat
	echo "${control}" > control
	echo "${copyright}" > copyright
	echo "${docs}" > docs
	echo "${dirs}" > dirs
}
#

prepare_sound_python ()
{
	section="sound"
	arch="all"
	builddep="cdbs, debhelper (>= 7), python-support (>= 0.6)"
	addit="XB-Python-Version: \${python:Versions}"	
}

build_erp ()
{
	project="exaile-remote-plugin"
	dirname="python/exaile-remote-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, python-gtk2, exaile (>=0.3.2.0)"
	description="Control Exaile via tray icons"
	descriptionlong='Control Exaile via tray icons.
 Иконки управления плеером в трее.'
	docfiles=""
	dirs="usr/share/exaile/plugins/remote"
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_etp ()
{
	project="exaile-tunetopsi-plugin"
	dirname="python/exaile-tunetopsi-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, exaile"
	description="Exaile Tune To Psi Plugin"
	descriptionlong='Sends Current Track Information to Psi/Psi+ as Tune.
 Отсылает данные о прослушиваемом треке jabber-клиенту Psi/Psi+.'
	docfiles=""
	dirs="usr/share/exaile/plugins/tunetopsi"
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_pyav ()
{
	run_resloader get_pyav
	project="pyalsavolume"
	dirname="pyalsavolume"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, libgtk-3-0, python-gobject (>=3.0.0), python-alsaaudio (>=0.6)"
	description="Tray ALSA volume changer"
	descriptionlong=' Простая программа для изменения громкости звука одного из микшеров ALSA из системного трея.
 Simple programm to change the volume of one of the ALSA mixer from the system tray..'
	docfiles="Readme"
	dirs='usr/bin
usr/share/pyalsavolume
usr/share/pyalsavolume/icons
usr/share/pyalsavolume/lang
usr/share/doc/pyalsavolume
usr/share/applications
'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_pypoff ()
{
	run_resloader get_pypoff
	project="pypoweroff"
	if [ "${version}" == "1.2" ]
	then
		ver=`cat ${srcdir}/${dirname}/version1.2.txt`
	else
		ver=`cat ${srcdir}/${dirname}/version.txt`
	fi
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	if [ "${version}" == "1.2" ]
	then
		patch -p1 < 3_to_2.patch
	fi
	section="misc"
	arch="all"
	builddep="cdbs, debhelper (>= 7), python-support (>= 0.6)"
	addit="XB-Python-Version: \${python:Versions}"
	description="Turn-Off Tool"
	descriptionlong='pyPowerOff - запланированное выключение компьютера.'
	docfiles=""
	dirs='usr/bin
usr/share/pypoweroff
usr/share/pypoweroff/images
usr/share/pypoweroff/glades
usr/share/pypoweroff/lang
usr/share/applications

'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_pyssh ()
{
	run_resloader get_pyssh
	project="pysshclient"
	dirname="pysshclient"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	section="misc"
	arch="all"
	builddep="cdbs, debhelper (>= 7), python-support (>= 0.6)"
	addit="XB-Python-Version: \${python:Versions}"
	depends="\${python:Depends}, python-crypto, libgtk-3-0, python-gobject (>=3.0.0), python-pexpect, sshfs, openssh-client"
	description="SSHFS Connection GUI"
	descriptionlong='pysshfs - it"s a simple program, that provides a graphical interface to connect to a remote directory using SSHFS.
	It is written on Python + PyGTK
'
	docfiles=""
	dirs='usr/bin
usr/share/pysshclient
usr/share/applications
'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_rbremp ()
{
	project="rb-remote-plugin"
	dirname="python/rb-remote-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, \${misc:Depends}, python-gtk2, python-glade2, python-gconf, rhythmbox"
	description="Control Rhythmbox via tray icons"
	descriptionlong='Control Rhythmbox via tray icons'
	docfiles=""
	dirs='usr/lib/rhythmbox/plugins/remote'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_rbresp ()
{
	project="rb-restore-plugin"
	dirname="python/rb-restore-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, \${misc:Depends}, python-gtk2, python-glade2, python-gconf, rhythmbox"
	description="Rhythmbox restore last item plugin"
	descriptionlong='Восстанавливает последний воспроизводимый трек при запуске Rhythmbox.
 Restores the last played item when runing Rhythmbox.'
	docfiles=""
	dirs='usr/lib/rhythmbox/plugins/restore'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_rbtunp ()
{
	project="rb-tunetopsi-plugin"
	dirname="python/rb-tunetopsi-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	prepare_sound_python
	depends="\${python:Depends}, \${misc:Depends}, python-gtk2, python-glade2, python-gconf, rhythmbox"
	description="Rhythmbox tune to Psi plugin"
	descriptionlong='Отсылает данные о прослушиваемом треке Jabber-кленту Psi/Psi+
 Sends tune information to Psi or Psi+ jabber client'
	docfiles=""
	dirs='usr/lib/rhythmbox/plugins/tunetopsi'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_regext ()
{
	check_qconf
	project="regexptest"
	dirname="qt/regexptest"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	section="x11"
	arch="any"
	builddep="debhelper (>= 7), cdbs, libqt4-dev, qconf"
	addit="#"
	depends="\${shlibs:Depends}, \${misc:Depends}, libc6 (>=2.7-1), libgcc1 (>=1:4.1.1), libqtcore4 (>=4.4.3), libqtgui4 (>=4.4.3), libstdc++6 (>=4.1.1), libx11-6, zlib1g (>=1:1.1.4)"
	description="RegExp Tester"
	descriptionlong='Simple tool written on Qt to test regular expressions'
	docfiles=""
	dirs="usr/bin
usr/share/regexptest
usr/share/regexptest/icons
usr/share/doc/regexptest
usr/share/doc/regexptest/html
usr/share/applications"
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	$qconfcmd
	./configure --prefix=/usr
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_avolume ()
{
	run_resloader get_avolume
	project="alsavolume"
	dirname="cppAlsaVolume"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	section="sound"
	arch="any"
	builddep="debhelper (>= 7), cdbs, libqt4-dev, libgtkmm-3.0-dev, libasound2-dev, libarchive-dev"
	addit="#"
	depends="\${shlibs:Depends}, \${misc:Depends}, libgtkmm-3.0-1, libasound2, libc6 (>=2.7-1), libgcc1 (>=1:4.1.1), libstdc++6 (>=4.1.1), libx11-6, zlib1g (>=1:1.1.4), libarchive12"
	description="Tray ALSA volume changer"
	descriptionlong='Simple programm to change the volume of one of the ALSA mixer from the system tray.'
	docfiles=""
	dirs="usr/bin
usr/share/alsavolume
usr/share/alsavolume/icons
usr/share/alsavolume/gladefiles
usr/share/applications"
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	qmake
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_qtavolume ()
{
	project="qtalsavolume"
	dirname="qt/qtalsavolume"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	section="sound"
	arch="any"
	builddep="debhelper (>= 7), cdbs, libqt4-dev, libasound2-dev"
	addit="#"
	depends="\${shlibs:Depends}, \${misc:Depends}, libasound2, libc6 (>=2.7-1), libgcc1 (>=1:4.1.1), libstdc++6 (>=4.1.1), libx11-6, zlib1g (>=1:1.1.4)"
	description="Tray ALSA volume changer"
	descriptionlong='Simple programm to change the volume of one of the ALSA mixer from the system tray.'
	docfiles=""
	dirs="usr/bin
usr/share/qtalsavolume
usr/share/qtalsavolume/icons
usr/share/qtalsavolume/languages
usr/share/applications"
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	qmake
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}

build_html2text ()
{
	run_resloader get_html2text
	project="htmltotextgui"
	dirname="htmltotextgui"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	debdir=${builddir}/${project}-${ver}
	prepare
	cd ${debdir}
	section="misc"
	arch="all"
	builddep="cdbs, debhelper (>= 7), python-support (>= 0.6)"
	addit="XB-Python-Version: \${python:Versions}"
	depends="\${python:Depends}, libgtk-3-0, python-gobject (>=3.0.0), python-chardet"
	description="HTML To TXT File Converter"
	descriptionlong='Converts ebook-files from HTML to TXT format'
	docfiles=""
	dirs='usr/bin
usr/share/htmltotextgui
usr/share/applications
'
	cd ${debdir}/debian
	prepare_specs
	cd ${debdir}
	build_deb
	cp -f ${builddir}/*.deb	${exitdir}/
}


print_menu ()
{
  local menu_text='Choose action TODO!
[1] - Build exaile-remote-plugin debian package
[2] - Build exaile-tunetopsi-plugin debian package
[3] - Build pyalsavolume debian package
[4] - Build pypoweroff 1.3.x debian package
[41] - Build pypoweroff 1.2.x debian package
[5] - Build pysshclient debian package
[6] - Build rb-remote-plugin debian package
[7] - Build rb-restore-plugin debian package
[8] - Build rb-tunetopsi-plugin debian package
[9] - Build regexptest debian package
[a] - Build html2text debian package
[b] - Build alsavolume debian package
[c] - Build qtalsavolume debian package
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
		"4" ) echo "Building PyPowerOff 1.3"
			depends="\${python:Depends}, \${misc:Depends}, libgtk-3-0, python-gobject (>=3.0.0)"
			dirname="pypoweroff"
			version="1.3"
			build_pypoff;;
		"41" ) echo "Building PyPowerOff 1.3"
			dirname="pypoweroff"
			depends="\${python:Depends}, \${misc:Depends}, python-gtk2, python-glade2"
			version="1.2"
			build_pypoff;;
		"5" ) build_pyssh;;
		"6" ) build_rbremp;;
		"7" ) build_rbresp;;
		"8" ) build_rbtunp;;
		"9" ) build_regext;;
		"a" ) build_html2text;;
		"b" ) build_avolume;;
		"c" ) build_qtavolume;;
		"0" ) quit;;
		"ra" ) rm_all;;
		"ga" ) get_all;;
	esac
}

clear
run_resloader get_src
run_resloader "check_dir ${srcdir}"
run_resloader "check_dir ${builddir}"
while [ ${isloop} = 1 ]
do
  print_menu
  choose_action
done
exit 0
