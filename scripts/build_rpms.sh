#!/bin/bash
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
data=$(LANG=en date +'%a, %d %b %Y %T %z')
year=$(date +'%Y')
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
#COLORS
red="\e[0;31m"
green="\e[0;32m"
nocolor="\x1B[0m"
pink="\x1B[01;91m"
yellow="\x1B[01;93m"
blue="\x1B[01;94m"
#
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

CWDIR=$(pwd)
echo "Current workdir ${CWDIR}"

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
			echo -e "${pink}No resloder.sh file found. Please download it with this script in the same dir${nocolor}"
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
	run_resloader "check_dir ${srcdir}"
	run_resloader get_src
}

prepare_tarball ()
{
	if [ ! -z "$1" ]
	then
		echo -e "${blue}Creating tarball archive${nocolor}"
		tar -pczf "$1" "${project}-${ver}"
	fi
}

prepare ()
{
	run_resloader "check_dir ${rpmdir}"
        run_resloader "check_dir ${specfiles}"
        run_resloader "check_dir ${srcfiles}"
        run_resloader "check_dir ${builddir}"
        run_resloader "check_dir ${exitdir}"
        rm -rf ${builddir}/*
	if [ ! -z "$1" ]
	then
		run_resloader "set_workdir ${project}-${ver}"
		run_resloader "prepare_src $1"
                cd ${builddir}
		run_resloader "check_dir ${project}-${ver}"
		if [ "${project}" == "pypoweroff" ] && [ "${ver}" \< "1.3" ]
		then
			patch -d ${project}-${ver} -p1 < ${project}-${ver}/3_to_2.patch
		fi
                prepare_tarball ${project}-${ver}.tar.gz
                cp -rf ${builddir}/${project}-${ver}.tar.gz ${srcfiles}/
	fi
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	run_resloader get_pyav
	project="pyalsavolume"
	dirname="pyalsavolume"
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	if [ "${version}" == "1.2" ]
	then
		ver=$(cat ${srcdir}/${dirname}/version1.2.txt)
	else
		ver=$(cat ${srcdir}/${dirname}/version.txt)
	fi
        run_resloader get_pypoff
	prepare ${dirname}
	section="Applications/System"
	arch="noarch"
	builddep="python-devel, python-setuptools"
	description="Turn-Off Tool"
	descriptionlong='pyPowerOff - simple sheduled PC shutdown / reboot tool.'
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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
qmake
%{__make} %{?_smp_mflags}  

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT=\"%{buildroot}\"

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/%{name}
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/%{name}/icons
mkdir -p %{buildroot}/usr/share/%{name}/langs

%{__install} -c -m 755 icons/* %{buildroot}/usr/share/%{name}/icons/
%{__install} -c -m 755 langs/*.qm %{buildroot}/usr/share/%{name}/langs/
%{__install} -c -m 755 %{name} %{buildroot}/usr/bin/
%{__install} -c -m 755 %{name}.desktop %{buildroot}/usr/share/applications

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%doc docs/regexp_help.html
%{_bindir}/%{name}
%{_datadir}/%{name}/icons/
%{_datadir}/%{name}/langs/
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
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
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

build_hismerger (){
	project="psi-history-merger"
	dirname="python/pyHistoryMerge"
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
	section="Applications/Utility"
	arch="noarch"
	builddep="python-devel, python-setuptools, python-qt4-devel"
	depends="python >= 2.7, python-qt4, python-sip"
	description="Psi/Psi+ history merger"
	descriptionlong='Merges history file of Psi/Psi+ jabber client'
        addit=""
	prepare_specs
	rpmbuild -ba ${specfiles}/${project}.spec
	cp -f ${rpms}/n*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_avolume ()
{
	project="alsavolume"
	dirname="cppAlsaVolume"
	run_resloader get_avolume
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
	builddep="alsa-devel, intltool, gettext-tools"
	echo -e "${blue}Enable pulseaudio support${nocolor} ${pink}[y/n(default)]${nocolor}"
	read ispulse
	if [ "${ispulse}" == "y" ]; then
		build_count=1
		cmake_flags="-DUSE_PULSE=ON"
		builddep="${builddep}, libpulse-devel"
	else
		cmake_flags="-DUSE_PULSE=OFF"
		build_count=2
	fi
        regspecfile="Summary: AlsaVolume
Name: alsavolume
Version: ${ver}
Release: ${build_count}
License: GPL-2
Group: Applications/Multimedia
URL: http://sites.google.com/site/thesomeprojects/main-1
Source0: %{name}-%{version}.tar.gz
BuildRequires: gcc-c++, zlib-devel, gtkmm3-devel, ${builddep}
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple tool written using gtkmm-3 library to set the levels of alsa mixers

%prep
%setup

%build
cmake -DCMAKE_INSTALL_PREFIX=%{buildroot}/usr ${cmake_flags} .
%{__make} %{?_smp_mflags}  

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT=\"%{buildroot}\"

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/%{name}
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/%{name}/icons
mkdir -p %{buildroot}/usr/share/%{name}/gladefiles
mkdir -p %{buildroot}/usr/share/%{name}/locale
mkdir -p %{buildroot}/usr/share/%{name}/locale/ru
mkdir -p %{buildroot}/usr/share/%{name}/locale/ru/LC_MESSAGES
mkdir -p %{buildroot}/usr/share/%{name}/locale/uk
mkdir -p %{buildroot}/usr/share/%{name}/locale/uk/LC_MESSAGES

%{__install} -c -m 755 icons/tb_icon*.png %{buildroot}/usr/share/%{name}/icons/
%{__install} -c -m 755 icons/volume*.png %{buildroot}/usr/share/%{name}/icons/
%{__install} -c -m 755 gladefiles/* %{buildroot}/usr/share/%{name}/gladefiles/
%{__install} -c -m 755 %{name} %{buildroot}/usr/bin/
%{__install} -c -m 755 %{name}.desktop %{buildroot}/usr/share/applications

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/%{name}
%{_datadir}/%{name}/icons/
%{_datadir}/%{name}/gladefiles/
%{_datadir}/%{name}/locale/
%{_datadir}/applications/"
        echo "${regspecfile}" > ${specfiles}/"alsavolume.spec"
	rpmbuild -ba ${specfiles}/"alsavolume.spec"
	cp -f ${rpms}/i*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_qtavolume ()
{
	project="qtalsavolume"
	dirname="qt/qtalsavolume"
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	builddep="alsa-devel"
	echo -e "${blue}Enable pulseaudio support${nocolor} ${pink}[y/n(default)]${nocolor}"
	read ispulse
	if [ "${ispulse}" == "y" ]; then
		build_count=1
		cmake_flags="-DUSE_PULSE=ON"
		builddep="${builddep}, libpulse-devel"
	else
		cmake_flags="-DUSE_PULSE=OFF"
		build_count=2
	fi
	prepare ${dirname}
        regspecfile="Summary: QtAlsaVolume
Name: qtalsavolume
Version: ${ver}
Release: ${build_count}
License: GPL-2
Group: Applications/Multimedia
URL: http://sites.google.com/site/thesomeprojects/main-1
Source0: %{name}-%{version}.tar.gz
BuildRequires: gcc-c++, ${builddep}
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple tool written using Qt4 library to set the levels of alsa mixers

%prep
%setup

%build
cmake -DCMAKE_INSTALL_PREFIX=%{buildroot}/usr ${cmake_flags} .
%{__make} %{?_smp_mflags}   

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT=\"%{buildroot}\"

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/%{name}
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/%{name}/icons
mkdir -p %{buildroot}/usr/share/%{name}/icons/light
mkdir -p %{buildroot}/usr/share/%{name}/icons/dark
mkdir -p %{buildroot}/usr/share/%{name}/languages

%{__install} -c -m 755 icons/light/tb_icon*.png %{buildroot}/usr/share/%{name}/icons/light/
%{__install} -c -m 755 icons/dark/tb_icon*.png %{buildroot}/usr/share/%{name}/icons/dark/
%{__install} -c -m 755 icons/volume*.png %{buildroot}/usr/share/%{name}/icons/
#%{__install} -c -m 755 languages/*.qm %{buildroot}/usr/share/%{name}/languages/
%{__install} -c -m 755 %{name} %{buildroot}/usr/bin/
%{__install} -c -m 755 %{name}.desktop %{buildroot}/usr/share/applications

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/%{name}
%{_datadir}/%{name}/icons/
%{_datadir}/%{name}/icons/light/
%{_datadir}/%{name}/icons/dark/
%{_datadir}/%{name}/languages/
%{_datadir}/applications/"
        echo "${regspecfile}" > ${specfiles}/"qtalsavolume.spec"
	rpmbuild -ba ${specfiles}/"qtalsavolume.spec"
	cp -f ${rpms}/i*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

build_qtpoweroff ()
{
	project="qtpoweroff"
	dirname="qt/qtpoweroff"
	ver=$(cat ${srcdir}/${dirname}/version.txt)
	prepare ${dirname}
        regspecfile="Summary: QtPowerOff
Name: qtpoweroff
Version: ${ver}
Release: 1
License: GPL-2
Group: Applications/System
URL: http://sites.google.com/site/thesomeprojects/main-1
Source0: %{name}-%{version}.tar.gz
BuildRequires: gcc-c++
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Simple tool written using Qt4 library to shedules reboot / shutdown PC

%prep
%setup

%build
cmake -DCMAKE_INSTALL_PREFIX=%{buildroot}/usr ${cmake_flags} .
%{__make} %{?_smp_mflags}   

mkdir -p %{buildroot}/usr/bin
mkdir -p %{buildroot}/usr/share/%{name}
mkdir -p %{buildroot}/usr/share/applications
mkdir -p %{buildroot}/usr/share/%{name}/images
mkdir -p %{buildroot}/usr/share/doc/%{name}
mkdir -p %{buildroot}/usr/share/%{name}/languages

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}

%{__make} install INSTALL_ROOT=\"%{buildroot}\"

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%defattr(-, root, root, 0755)
%{_bindir}/%{name}
%{_datadir}/%{name}/images/
%{_datadir}/%{name}/languages/
%{_datadir}/applications/
%{_datadir}/doc/%{name}/"
        echo "${regspecfile}" > ${specfiles}/"qtalsavolume.spec"
	rpmbuild -ba ${specfiles}/"qtalsavolume.spec"
	cp -f ${rpms}/i*/*.rpm	${exitdir}/
        cp -f ${srpms}/*.rpm	${exitdir}/
}

print_menu ()
{
  echo -e "${blue}Choose action TODO!${nocolor}
${pink}[1]${nocolor} - Build exaile-remote-plugin OpenSUSE RPM package
${pink}[2]${nocolor} - Build exaile-tunetopsi-plugin OpenSUSE RPM package
${pink}[3]${nocolor} - Build pyalsavolume OpenSUSE RPM package
${pink}[4]${nocolor} - Build pypoweroff 1.3.x OpenSUSE RPM package
${pink}[41]${nocolor} - Build pypoweroff 1.2.x OpenSUSE RPM package
${pink}[5]${nocolor} - Build pysshclient OpenSUSE RPM package
${pink}[6]${nocolor} - Build rb-remote-plugin OpenSUSE RPM package
${pink}[7]${nocolor} - Build rb-restore-plugin OpenSUSE RPM package
${pink}[8]${nocolor} - Build rb-tunetopsi-plugin OpenSUSE RPM package
${pink}[9]${nocolor} - Build regexptest OpenSUSE RPM package
${pink}[a]${nocolor} - Build html2text OpenSUSE RPM package
${pink}[b]${nocolor} - Build alsavolume OpenSUSE RPM package
${pink}[c]${nocolor} - Build qtalsavolume OpenSUSE RPM package
${pink}[d]${nocolor} - Build psi-history-merger OpenSUSE RPM package
${pink}[e]${nocolor} - Build qtpoweroff OpenSUSE RPM package
${pink}[0]${nocolor} - Exit"
}

choose_action ()
{
	read vibor
	case ${vibor} in
		"1" ) build_erp;;
		"2" ) build_etp;;
		"3" ) build_pyav;;
		"4" ) echo -e "${blue}Building PyPowerOff-${nocolor}${pink}1.3${nocolor}"
			dirname="pypoweroff"
			depends="python >= 2.6, libgtk-3-0, python-gobject >= 3.0.0"
			version="1.3"
			build_pypoff;;
		"41" ) echo -e "${blue}Building PyPowerOff-${nocolor}${pink}1.2${nocolor}"
			dirname="pypoweroff"
			depends="python >= 2.6, python-gtk >= 2.0"
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
		"d" ) build_hismerger;;
		"e" ) build_qtpoweroff;;
		"0" ) quit;;
		"ra" ) rm_all;;
	esac
}

cd ${home}
run_resloader "check_dir ${srcdir}"
run_resloader get_src && clear
while [ ${isloop} = 1 ]
do
  print_menu
  choose_action
done
exit 0
