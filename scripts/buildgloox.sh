#!/bin/bash
homedir=$HOME
glooxsrcdir=${homedir}/gloox-0.9
glooxbuilddir=${homedir}/build_gloox
data=`LANG=en date +'%a, %d %b %Y %T %z'`
year=`date +'%Y'`

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

get_gloox () 
{
cd ${homedir}
	svn co svn://svn.camaya.net/gloox/branches/0.9 gloox-0.9
}

prepare_src ()
{
get_gloox
check_dir ${glooxbuilddir}
rm -rf ${glooxbuilddir}/*
cp -rf ${glooxsrcdir} ${glooxbuilddir}/
}

#DEBFILES
changelog="gloox (0.9-1) unstable; urgency=low

  * Initial release

 -- ${username} <${username}@gmail.com>  ${data}"
compat="8"
control="Source: gloox
Priority: extra
Maintainer: ${username}<${username}@gmail.com>
Build-Depends: debhelper (>= 8.0.0), autotools-dev
Standards-Version: 3.9.2
Section: libs
Homepage: http://camaya.net

Package: gloox-dev
Section: libdevel
Architecture: any
Depends: gloox (= \${binary:Version})
Description: Gloox library development
 Headers for build projects using gloox library

Package: gloox
Section: libs
Architecture: any
Depends: \${shlibs:Depends}, \${misc:Depends}
Description: Gloox library
 Gloox library
"
copyrigth="Format: http://dep.debian.net/deps/dep5
Upstream-Name: gloox
Source: svn://svn.camaya.net/gloox/branches/0.9

Files: *
Copyright: Jakob Schrцter  <js@camaya.net>

License: GPL-2.0+

Files: debian/*
Copyright: ${year}  ${username} <${username}@gmail.com>
License: GPL-2.0+

License: GPL-2.0+
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 .
 On Debian systems, the complete text of the GNU General
 Public License version 3 can be found in \"/usr/share/common-licenses/GPL-2\".

# Please also look if there are files or directories which have a
# different copyright/license attached and list them here.
"
docs='NEWS
README
TODO
'

files='gloox-dev_0.9-1_i386.deb libdevel extra
gloox_0.9-1_i386.deb libs extra
'
glooxdirs="usr/lib"
glooxinst='usr/lib/lib*.so.*
usr/lib/lib*.so
usr/lib/lib*.a
usr/lib/lib*.la
usr/lib/pkgconfig/*
usr/bin/*'
glooxdevdirs="usr/include"
glooxdevinst="usr/include/*"
rules='#!/usr/bin/make -f
# -*- makefile -*-

%:
	dh $@ 
'
substvars='shlibs:Depends=libc6 (>= 2.9), libgcc1 (>= 1:4.1.1), libssl1.0.0 (>= 1.0.0), libstdc++6 (>= 4.6), zlib1g (>= 1:1.1.4)
misc:Depends=
'
devsubstvars='misc:Depends=
'
packagesh='#!/bin/bash
set -e

debuild -us -uc
debuild -S -us -uc
su -c pbuilder build ../gloox-0.9.dsc
'
#END DEBFILES

prepare_deb () 
{
debdir=${glooxbuilddir}/gloox-0.9/debian
check_dir ${debdir}
cd ${debdir}
echo "${changelog}" > ${debdir}/changelog
echo "${compat}" > ${debdir}/compat
echo "${control}" > ${debdir}/control
echo "${copyrigth}" > ${debdir}/copyrigth
echo "${docs}" > ${debdir}/docs
echo "${files}" > ${debdir}/files
echo "${glooxdirs}" > ${debdir}/gloox.dirs
echo "${glooxinst}" > ${debdir}/gloox.install
echo "${glooxdevdirs}" > ${debdir}/gloox-dev.dirs
echo "${glooxdevinst}" > ${debdir}/gloox-dev.install
echo "${rules}" > ${debdir}/rules
echo "${substvars}" > ${debdir}/gloox.substvars
echo "${devsubstvars}" > ${debdir}/gloox-dev.substvars
cd ${glooxbuilddir}/gloox-0.9
echo "${packagesh}" > package.sh
./autogen.sh
dpkg-buildpackage -rfakeroot
}

echo "Do you want to build gloox debian package [y/n]"
read asnw
if [  $answ=="y" ]
then
	cd ${homedir}
	prepare_src
	cd ${glooxsrcdir}
	prepare_deb
else
	exit 0
fi
