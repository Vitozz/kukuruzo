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
exitdir=${srcdir}/rpms
rpmdir=${homedir}/rpmbuild
specfiles=${rpmdir}/SPECS
srcfiles=${rpmdir}/SOURCES
rpms=${rpmdir}/RPMS
srpms=${rpmdir}/SRPMS
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

Maintainer="Vitaly Tonkacheyev <thetvg@gmail.com>"

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
	cd ${homedir}
	git clone "git://github.com/Vitozz/kukuruzo.git"  ${srcdir}
	cd ${srcdir}
	git init
	git pull
}

rm_all ()
{
	cd ${homedir}
	if [ -d "${srcdir}" ]
	then
		rm -rf ${srcdir}
	fi
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

prepare_global ()
{
	check_dir ${srcdir}
	get_src
}

prepare_src ()
{
	check_dir ${rpmdir}
        check_dir ${specfiles}
        check_dir ${srcfiles}
        check_dir ${builddir}
        check_dir ${exitdir}
        rm -rf ${builddir}/*
	if [ ! -z "$1" ]
	then
		dirname="$1"
		check_dir ${builddir}/${project}-${ver}
		cp -rf ${srcdir}/${dirname}/* ${builddir}/${project}-${ver}/
		workdir=${builddir}/${project}-${ver}
                cd ${builddir}
                package_name=${project}-${ver}.tar.gz
                tar -pczf ${package_name} ${project}-${ver}
                cp -rf ${builddir}/${package_name} ${srcfiles}/
	fi
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
specfile="Summary: ${description}
Name: ${project}
Version: ${ver}
Release: 1
License: GPL-2
Group: ${section}
URL: http://sites.google.com/site/thesomeprojects/
Source0: %{name}-%{version}.tar.gz
Requires: ${depends}
%if %{suse_version} > 1110
BuildRequires:	${builddep}
%endif

%if %{?suse_version: %{suse_version} > 1120} %{!?suse_version:1}
BuildArchitectures: ${arch}
%endif 

BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
${descriptionlong}

%prep
%setup -q

%build
%{__python} setup.py build

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}
%{__python} setup.py install --prefix=%{_prefix} --root=%{buildroot} --record-rpm=INSTALLED_FILES

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files -f INSTALLED_FILES
%defattr(-,root,root)
${addit}
"
echo "${specfile}"> ${specfiles}/${project}.spec       
}
#

prepare_sound_python ()
{
	section="Applications/Multimedia"
	arch="noarch"
	builddep="python-devel, python-setuptools"
}

build_erp ()
{
	project="exaile-remote-plugin"
	dirname="python/exaile-remote-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, python-gtk >= 2.0, exaile >= 0.3.2.0"
	description="Control Exaile via tray icons"
	descriptionlong='Control Exaile via tray icons.'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_etp ()
{
	project="exaile-tunetopsi-plugin"
	dirname="python/exaile-tunetopsi-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, exaile"
	description="Exaile Tune To Psi Plugin"
	descriptionlong='Sends Current Track Information to Psi/Psi+ as Tune.'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_pyav ()
{
	project="pyalsavolume"
	dirname="python/pyalsavolume"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, libgtk-3-0, python-gobject >= 3.0.0, pyalsaaudio >= 0.6"
	description="Tray ALSA volume changer"
	descriptionlong='Simple programm to change the volume of one of the ALSA mixer from the system tray..'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_pypoff ()
{
	project="pypoweroff"
	dirname="python/pypoweroff"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	section="Applications/System"
	arch="noarch"
	builddep="python-devel, python-setuptools"
	depends="python >= 2.6, python-gtk >= 2.0, python-gconf"
	description="Turn-Off Tool"
	descriptionlong='pyPowerOff - запланированное выключение компьютера.'
        addit="%doc COPYING"
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_pyssh ()
{
	project="pysshclient"
	dirname="python/pysshclient"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	section="Applications/Communications"
	arch="noarch"
	builddep="python-devel, python-setuptools"
	depends="python >= 2.6, python-crypto, libgtk-3-0, python-gobject >= 3.0.0, python-pexpect, sshfs, openssh"
	description="SSHFS Connection GUI"
	descriptionlong='pysshfs - it"s a simple program, that provides a graphical interface to connect to a remote directory using SSHFS.
	It is written on Python + PyGTK'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_rbremp ()
{
	project="rb-remote-plugin"
	dirname="python/rb-remote-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, python-gtk >= 2.0, python-gconf, rhythmbox"
	description="Control Rhythmbox via tray icons"
	descriptionlong='Control Rhythmbox via tray icons'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_rbresp ()
{
	project="rb-restore-plugin"
	dirname="python/rb-restore-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, python-gtk >= 2.0, python-gconf, rhythmbox"
	description="Rhythmbox restore last item plugin"
	descriptionlong='Восстанавливает последний воспроизводимый трек при запуске Rhythmbox.
 Restores the last played item when runing Rhythmbox.'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_rbtunp ()
{
	project="rb-tunetopsi-plugin"
	dirname="python/rb-tunetopsi-plugin"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	prepare_sound_python
	depends="python >= 2.6, python-gtk >= 2.0, python-gconf, rhythmbox"
	description="Rhythmbox tune to Psi plugin"
	descriptionlong='Отсылает данные о прослушиваемом треке Jabber-кленту Psi/Psi+
 Sends tune information to Psi or Psi+ jabber client'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_regext ()
{
	check_qconf
	project="regexptest"
	dirname="qt/regexptest"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
        regspecfile="Summary: RegExp Tester
Name: regexptest
Version: ${ver}
Release: 1
License: GPL-2
Group: Development/Tools
URL: http://sites.google.com/site/thesomeprojects/main-1
Source0: %{name}-%{version}.tar.gz
BuildRequires: gcc-c++, zlib-devel
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple tool written on Qt to test regular expressions

%prep
%setup

%build
qconf
./configure --prefix=\"%{_prefix}\" --bindir=\"%{_bindir}\" --qtdir=$QTDIR
%{__make} %{?_smp_mflags}  

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT=\"%{buildroot}\"

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/%{name}
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/%{name}/icons

%{__install} -c -m 755 icons/* %{buildroot}/usr/share/%{name}/icons/
%{__install} -c -m 755 %{name} %{buildroot}/usr/bin/
%{__install} -c -m 755 %{name}.desktop %{buildroot}/usr/share/applications

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%doc docs/regexp_help.html
%{_bindir}/%{name}
%{_datadir}/%{name}/icons/
%{_datadir}/doc/%{name}/html/regexp_help.html
%{_datadir}/applications/"
        echo "${regspecfile}" > ${specfiles}/"regexptest.spec"
	rpmbuild -ba ${specfiles}/"regexptest.spec"
	cp -f ${rpms}/i*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_html2text ()
{
	project="htmltotextgui"
	dirname="python/htmltotextgui"
	ver=`cat ${srcdir}/${dirname}/version.txt`
	prepare_src ${dirname}
	section="Applications/Utility"
	arch="noarch"
	builddep="python-devel, python-setuptools"
	depends="python >= 2.6, libgtk-3-0, python-gobject >= 3.0.0, python-chardet"
	description="HTML To TXT File Converter"
	descriptionlong='Converts ebook-files from HTML to TXT format'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}


print_menu ()
{
  local menu_text='Choose action TODO!
[1] - Build exaile-remote-plugin OpenSUSE RPM package
[2] - Build exaile-tunetopsi-plugin OpenSUSE RPM package
[3] - Build pyalsavolume OpenSUSE RPM package
[4] - Build pypoweroff OpenSUSE RPM package
[5] - Build pysshclient OpenSUSE RPM package
[6] - Build rb-remote-plugin OpenSUSE RPM package
[7] - Build rb-restore-plugin OpenSUSE RPM package
[8] - Build rb-tunetopsi-plugin OpenSUSE RPM package
[9] - Build regexptest OpenSUSE RPM package
[a] - Build html2text OpenSUSE RPM package
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
    "a" ) build_html2text;;
    "0" ) quit;;
    "ra" ) rm_all;;
  esac
}

cd ${home}
clear
check_dir ${srcdir}
get_src
while [ ${isloop} = 1 ]
do
  print_menu
  choose_action
done
exit 0