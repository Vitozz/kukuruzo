#!/bin/bash
#Скрипт сборки пси+ различнымм способами под разные системы RPM/DEB/WIN32
#А также подготовки исходников для разработки
#CONSTANTS/КОНСТАНТЫ
home=${HOME:-/home/$USER} #домашний каталог
#guthub repositories
psi_url="https://github.com/psi-im/psi.git"
psi_plus_url="https://github.com/psi-plus/main.git"
plugins_url="https://github.com/psi-im/plugins.git"
langs_url="https://github.com/psi-plus/psi-plus-l10n.git"
snapshots_url="https://github.com/psi-plus/psi-plus-snapshots.git"
def_prefix="/usr" #префикс для сборки пси+
#
#DEFAULT OPTIONS/ОПЦИИ ПО УМОЛЧАНИЮ
spell_flag="-DUSE_ENCHANT=OFF -DUSE_HUNSPELL=ON"
spellchek_engine="hunspell"
chat_type="webengine" #webkit, basic, webengine
iswebkit=""
isoffline=0
skip_invalid=0
use_plugins="*"
let cpu_count=$(grep -c ^processor /proc/cpuinfo)
devm=0
wbkt=0
pref=0
clear_tmp_on_exit=1 #очищать каталог сборки при выходе из скрипта
#
#COLORS
red="\e[0;31m"
green="\e[0;32m"
nocolor="\x1B[0m"
pink="\x1B[01;91m"
yellow="\x1B[01;93m"
blue="\x1B[01;94m"
#

#VARIABLES/ПЕРЕМЕННЫЕ
#каталог где будет лежать скрипт libpsibuild.sh и каталог buildpsi(по умолчанию)
githubdir=${home}/github
#значение переменной buildpsi по умолчанию
default_buildpsi=${githubdir}/psi 
#имя временного каталога для пакетирования
inst_suffix=tmp
#префикс CMAKE по умолчанию
DEF_CMAKE_INST_PREFIX="${home}/.local"
#каталог плагинов в префиксе по умолчанию
DEF_CMAKE_INST_SUFFIX="share/psi+/plugins"
#список плагинов для сборки через ";" (otrplugin;cleanerplugin и.т.д.)
DEF_PLUG_LIST="ALL"
#тип сборки плагинов
DEF_CMAKE_BUILD_TYPE="Release"
#MXE PATH
mxe_root=${githubdir}/mxe
#i386 mxe prefix
i686_mxe_prefix=${mxe_root}/usr/i686-w64-mingw32.shared
x86_64_mxe_prefix=${mxe_root}/usr/x86_64-w64-mingw32.shared
OLDPATH=${PATH}


#WARNING: следующие переменные будут изменены в процессе работы скрипта автоматически
buildpsi=${default_buildpsi} #инициализация переменной
upstream_src=${buildpsi}/psi #репозиторий Psi
psiplus_src=${buildpsi}/psi-plus #репозиторий Psi+
snapshots_src=${buildpsi}/psi-plus-snapshots  #репозиторий снапшотов
tmp_dir=/tmp/ppbuild #временный каталог для подготовки и сборки
workdir=${tmp_dir}/worksrc
builddir=${tmp_dir}/build
patches=${buildpsi}/psi-plus/patches #путь к патчам psi+, необходим для разработки
inst_path=${buildpsi}/${inst_suffix} #только для пакетирования
rpm_oscodename=$(lsb_release -is)
#

#ENVIRONMENT VARIABLES/ПЕРЕМЕННЫЕ СРЕДЫ
psi_datadir=${home}/.local/share/psi+
psi_cachedir=${home}/.cache/psi+
psi_homeplugdir=${psi_datadir}/plugins
#

#CONFIG FILE PATH/ПУТЬ К ФАЙЛУ НАСТРОЕК
config_file=${home}/.config/psibuild.cfg

#PLUGINS_BUILD_LOG/ЛОГ ФАЙЛ СБОРКИ ПЛАГИНОВ
plugbuild_log=${workdir}/plugins.log
#

#RPM_VARIABLES/ПЕРЕМЕННЫЕ ДЛЯ СБОРКИ RPM ПАКЕТОВ
rpmbuilddir=${home}/rpmbuild
rpmspec=${rpmbuilddir}/SPECS
rpmsrc=${rpmbuilddir}/SOURCES
#
#Скачивание репозитория
fetch_url ()
{
  local last_dir=$(pwd)
  local fetch_dir=""
  local f_url=""
  local fetch_log=${buildpsi}/fetch.log
  if [ ! -z "$2" ]; then
    fetch_dir=$2
  fi
  if [ ! -z "$1" ]; then
    f_url=$1
    if [ "$(ls -A ${fetch_dir})" ]; then
      cd ${fetch_dir}
      git reset --hard
      git pull
      git submodule update
      cd ${last_dir}
    else
      git clone ${f_url} ${fetch_dir}
      cd ${fetch_dir}
      git submodule init
      git submodule update
      cd ${last_dir}
    fi
  fi
}
#Скачивание исходников
fetch_all ()
{
  if [ $isoffline -eq 0 ]; then
    fetch_url ${psi_url} ${upstream_src}
    fetch_url ${psi_plus_url} ${psiplus_src}
    fetch_url ${plugins_url} ${buildpsi}/plugins
    fetch_url ${langs_url} ${buildpsi}/langs
  fi
}
#Скачивание снапшотов
fetch_snapshots ()
{
  if [ $isoffline -eq 0 ]; then
    fetch_url ${snapshots_url} ${snapshots_src}
  fi
}

#Выход
quit ()
{
  clean_tmp_dirs
  exit 0
}
#Чтение опций из файла настроек
read_options ()
{
  local pluginlist=""
  if [ -f ${config_file} ]; then
    local inc=0
    while read -r line; do
      case ${inc} in
      "0" ) iswebkit=$(echo ${line});;
      "1" ) isoffline=$(echo ${line});;
      "2" ) skip_invalid=$(echo ${line});;
      "3" ) pluginlist=$(echo ${line});;
      "4" ) spellchek_engine=$(echo ${line});;
      "5" ) buildpsi=$(echo ${line});;
      esac
      let "inc+=1"
    done < ${config_file}
    if [ "$pluginlist" == "all" ]; then
      use_plugins="*"
    else
      use_plugins=${pluginlist}
    fi
    if [ -z "${buildpsi}" ]; then
      buildpsi=${default_buildpsi}
    fi
    if [ "${buildpsi:0:5}" == "\$HOME" ]; then
      buildpsi=${home}/${buildpsi:6}
    fi
  fi
  update_variables
}
#Обновление переменных 
update_variables ()
{
  upstream_src=${buildpsi}/psi
  psiplus_src=${buildpsi}/psi-plus
  snapshots_src=${buildpsi}/psi-plus-snapshots
  patches=${buildpsi}/psi-plus/patches
  inst_path=${buildpsi}/${inst_suffix}
  if [ "${spellchek_engine}" == "enchant" ]; then
    spell_flag="-DUSE_ENCHANT=ON -DUSE_HUNSPELL=OFF"
  fi
}
#Выход с ошибкой
die() { echo "$@"; exit 1; }
#Создание каталога
check_dir ()
{
  if [ ! -z "$1" ]; then
    if [ ! -d "$1" ]; then
      mkdir -pv "$1"
    fi
  fi
}
#Скачивание всех исходников
down_all ()
{
  check_dir ${upstream_src}
  check_dir ${psiplus_src}
  check_dir ${buildpsi}/plugins
  check_dir ${buildpsi}/langs
  fetch_all
}
#Имя системы
get_os_codename()
{
  rpm_oscodename=$(lsb_release -is)
}
#Применение патчей
patch_psi ()
{
  local patchlist=$(ls ${patches}/ | grep diff)
  local patchnumber=10000
  local bdir=$(pwd)
  local msg=""
  local patchlogfile=${buildpsi}/${bdir##*/}${2##*/}_patch.log
  if [ ! -z "$1" ]; then
    patchnumber=$1
  fi
  echo "--Start patching--">${patchlogfile}
  do_patch ()
  {
    if [ ! -z "$1" ]; then
      echo "==${1##*/}==">>${patchlogfile}
      msg="${green}[OK]${nocolor}"
      patch -p1 --input=$1>>${patchlogfile} || msg="${red}[NO]${nocolor}"
      if [ "${msg}" != "${green}[OK]${nocolor}" ] && [ "${skip_patches}" == "n" ]; then
        die "Patching failed at patch $1"      
      fi
      echo -e "${1##*/} ${msg}"
    fi
  }
  if [ -z "$2" ]; then
    for patchfile in ${patchlist}; do
      if [  ${patchfile:0:4} -lt ${patchnumber} ]; then
        do_patch ${patches}/${patchfile}
      fi
    done
  else
    do_patch $2
  fi
}
#Получение версии пси+
get_psi_plus_version()
{
  local plus_tag=$(cd ${psiplus_src} && git describe --tags | cut -d - -f1)
  local psi_num=$("${upstream_src}/admin/git_revnumber.sh" "${plus_tag}")
  local plus_num=$(cd ${psiplus_src} && git describe --tags | cut -d - -f2)
  local sum_commit=$(expr ${psi_num} + ${plus_num})
  psi_package_version="${plus_tag}.${sum_commit}"
  psi_plus_version=$(${psiplus_src}/admin/psi-plus-nightly-version ${upstream_src})
  echo "SHORT_VERSION = $psi_package_version"
  echo "LONG_VERSION = $psi_plus_version"
}
#Подготовка исходников и обновление подмодулей
prepare_psi_src ()
{
  if [ ! -z "$1" ]; then
    git archive --format=tar HEAD | ( cd $1 ; tar xf - )
    (
      export ddir="$1"
      git submodule foreach "( git archive --format=tar HEAD ) \
| ( cd \"${ddir}/\${path}\" ; tar xf - )"
    )
  fi
}
#Копирование иконок пси+
copy_psiplus_icons()
{
  if [ ! -z "$1" ]; then
    cp -a ${psiplus_src}/iconsets/* $1/iconsets/
    cp -a ${psiplus_src}/app.ico $1/win32/
  fi
}
#Очистка временных каталогов
clean_tmp_dirs()
{
  if [ -d "${tmp_dir}" ] && [ ${clear_tmp_on_exit} -eq 1 ]; then
    rm -rf ${tmp_dir}
  fi
}
#Подготовка каталога сборки
prepare_workspace ()
{
  local last_dir=$(pwd)
  echo "Deleting ${workdir}"
  clean_tmp_dirs
  check_dir ${workdir}
  cd ${upstream_src}
  prepare_psi_src ${workdir}
  cd ${buildpsi}/plugins
  prepare_psi_src ${workdir}/src/plugins
  copy_psiplus_icons ${workdir}
  check_dir ${workdir}/translations
  cp -a ${buildpsi}/langs/translations/*.ts ${workdir}/translations/
  cd ${workdir}
  patch_psi
  get_psi_plus_version
  cd ${psiplus_src}
  local suffix=""
  local builddate=$(LANG=en date +'%F')
  if [ ! -z "${iswebkit}" ]; then
    suffix="-webkit"
  fi
  echo $psi_plus_version > ${workdir}/version
}
#Подготовка исходников из репы снапшотов
prepare_snapshots_workspace ()
{
  fetch_snapshots
  local last_dir=$(pwd)
  echo "Deleting ${workdir}"
  clean_tmp_dirs
  check_dir ${workdir}
  cd ${snapshots_src}
  prepare_psi_src ${workdir}
  check_dir ${workdir}/translations
  cp -a ${buildpsi}/langs/translations/*.ts ${workdir}/translations/
  cd ${workdir}
}

#Подготовка исходников
prepare_src ()
{
  down_all
  prepare_workspace
}
#Очистка каталога сборки
prepare_builddir ()
{
  if [ ! -z $1 ]; then
    local clean_dir=$1
    check_dir ${clean_dir}
    if [ -f "${clean_dir}/CMakeCache.txt" ]; then
      cd ${clean_dir}
      rm -rf ${clean_dir}/*
    fi
  fi
}
#Создание бэкапа исходников архивом tar
backup_tar ()
{
  echo "Backup ${buildpsi##*/} into ${buildpsi%/*}/${buildpsi##*/}.tar.gz started..."
  cd ${buildpsi%/*}
  tar -pczf ${buildpsi##*/}.tar.gz ${buildpsi##*/}
  echo "Backup finished..."; echo " "
}
#Подготовка архива tar
prepare_tar ()
{
  check_dir ${rpmbuilddir}
  check_dir ${rpmsrc}
  check_dir ${rpmspec}
  echo "Preparing Psi+ source package to build RPM..."
  get_psi_plus_version
  local tar_name=psi-plus-${psi_package_version}
  local new_src=${buildpsi}/${tar_name}
  cp -r ${workdir} ${new_src}
  if [ -d ${new_src} ]; then
    cd ${buildpsi}
    tar -czf ${tar_name}.tar.gz ${tar_name}
    rm -r -f ${new_src}
    if [ -d ${rpmsrc} ]; then
      if [ -f "${rpmsrc}/${tar_name}.tar.gz" ]; then
        rm -f ${rpmsrc}/${tar_name}.tar.gz
      fi
      cp -f ${buildpsi}/${tar_name}.tar.gz ${rpmsrc}
    fi
    echo "Preparing completed"
  fi
}
#Сборка исходников CMake-ом
compile_psiplus ()
{
  curd=$(pwd)
  prepare_src
  cd ${workdir}
  local buildlog=${buildpsi}/build.log
  echo "***Build started***">${buildlog}
  prepare_builddir ${builddir}
  cd ${builddir}
  flags="-DCMAKE_BUILD_TYPE=${DEF_CMAKE_BUILD_TYPE} -DPSI_LIBDIR=${buildpsi}/build-plugins"
  if [ ! -z "$1" ]; then
    flags="${flags} -DCMAKE_INSTALL_PREFIX=$1"
  else
    flags="${flags}"
  fi
  if [ -z "${iswebkit}" ]; then
    flags="${flags} -DCHAT_TYPE=basic"
  fi
  cd ${builddir}
  cbuild_path=${workdir}
  if [ ! -z "$2" ]; then
    cbuild_path=$2
  fi
  echo "--Starting cmake 
  cmake ${flags} ${cbuild_path}">>${buildlog}
  cmake ${flags} ${cbuild_path}
  echo "--Starting psi-plus compilation">>${buildlog}
  cmake --build . --target all -- -j${cpu_count} 2>>${buildlog} || echo -e "${red}There were errors. Open ${buildpsi}/build.log to see${nocolor}"
  echo "***Build finished***">>${buildlog}
  if [ -z "$1" ]; then
    cmake --build . --target prepare-bin
    echo "Psi+ installed in ${workdir}">>${buildlog}
  fi
  cd ${curd}
}
#Сборка из репы снапшотов
build_all_psiplus ()
{
  curd=$(pwd)
  prepare_snapshots_workspace
  cd ${workdir}
  local buildlog=${buildpsi}/build-all.log
  echo "***Build started***">${buildlog}
  prepare_builddir ${builddir}
  cd ${builddir}
  flags="-DCMAKE_BUILD_TYPE=${DEF_CMAKE_BUILD_TYPE} -DBUILD_PLUGINS=${DEF_PLUG_LIST} -DENABLE_PLUGINS=ON -DDEV_MODE=ON -DBUILD_DEV_PLUGINS=ON"
  if [ -z "${iswebkit}" ]; then
    flags="${flags} -DCHAT_TYPE=basic"
  fi
  cd ${builddir}
  cbuild_path=${workdir}
  echo "--Starting cmake 
  cmake ${flags} ${cbuild_path}">>${buildlog}
  cmake ${flags} ${cbuild_path}
  echo "--Starting psi-plus compilation">>${buildlog}
  cmake --build . --target all -- -j${cpu_count} 2>>${buildlog} || echo -e "${red}There were errors. Open ${buildpsi}/build.log to see${nocolor}"
  echo "***Build finished***">>${buildlog}
  cmake --build . --target prepare-bin
  echo "Psi+ compiled at ${builddir}">>${buildlog}
  cd ${curd}
}
#Сборка плагинов утилитой cmake
build_cmake_plugins ()
{
  echo_done() {
    echo " "
    echo "********************************"
    echo "Plugins installed succesfully!!!"
    echo "into ${p_inst_path}"
    echo "********************************"
    echo " "
  }
  if [ ! -f "${upstream_src}/psi.pro" ]; then
    prepare_src
  fi
  local plugdir=${buildpsi}/plugins
  check_dir ${plugdir}
  local b_dir=${buildpsi}/build-plugins
  prepare_builddir ${b_dir}
  cd ${b_dir}
  p_inst_path=${b_dir}/plugins
  plug_cmake_flags="-DCMAKE_BUILD_TYPE=${DEF_CMAKE_BUILD_TYPE} -DBUILD_PLUGINS=${DEF_PLUG_LIST} -DBUILD_DEV_PLUGINS=ON -DPLUGINS_ROOT_DIR=${upstream_src}/src/plugins -DPsiPluginsApi_DIR=${plugdir}/cmake/modules"
  echo -e "${blue}Do you want to install psi+ plugins into ${psi_homeplugdir}${nocolor} ${pink}[y/n(default)]${nocolor}"
  read isinstall
  if [ "${isinstall}" == "y" ]; then
    local pl_preffix=${DEF_CMAKE_INST_PREFIX}
    local pl_suffix=${DEF_CMAKE_INST_SUFFIX}
    plug_cmake_flags="${plug_cmake_flags} -DCMAKE_INSTALL_PREFIX=${pl_preffix} -DPLUGINS_PATH=${pl_suffix}"
    p_inst_path=${pl_preffix}/${pl_suffix}
  fi  
  echo " "; echo "Build psi+ plugins using CMAKE started..."; echo " "
  cmake ${plug_cmake_flags} ${plugdir}
  cmake --build . --target all -- -j${cpu_count} && echo_done
  if [ "${isinstall}" == "y" ]; then
    cmake --build . --target install && echo_done
  fi
  cd ${workdir}
}
#

#Сборка deb-пакета при помощи checkinstall
build_deb_package ()
{
  check_qt_deps
  compile_psiplus /usr ${workdir}
  echo "Building Psi+ DEB package with checkinstall"
  local desc='Psi is a cross-platform powerful Jabber client (Qt, C++) designed for the Jabber power users.
Psi+ - Psi IM Mod by psi-dev@conference.jabber.ru.'
  cd ${workdir}
  echo "${desc}" > description-pak
  #make spellcheck
  local spell_dep=""
  if [ "${spellchek_engine}" == "hunspell" ]; then
    spell_dep="libhunspell-1.6-0"
  else
    spell_dep="libenchant1c2a"
  fi
  local webkitdep=""
  if [ ! -z "${iswebkit}" ]; then
    webkitdep=", libqt5webkit5"
  fi
  qt_deps="libqt5dbus5, libqt5network5, libqt5xml5 , libqt5core5a, libqt5gui5, libqt5widgets5, libqt5x11extras5${webkitdep}"
  local requires=" ${spell_dep}, 'libc6 (>=2.7-1)', 'libgcc1 (>=1:4.1.1)', 'libqca2', ${qt_deps}, 'libstdc++6 (>=4.1.1)', 'libx11-6', 'libxext6', 'libxss1', 'zlib1g (>=1:1.1.4)' "
  sudo checkinstall -D --nodoc --pkgname=psi-plus --pkggroup=net --pkgversion=${psi_package_version} --pkgsource=${workdir} --maintainer="thetvg@gmail.com" --requires="${requires}"
  cp -f ${workdir}/*.deb ${buildpsi}
}
#Подготовка спек-файла для сборки RPM-пакета
prepare_spec ()
{
  if [ -z "${iswebkit}" ]; then
    extraflags="-DCHAT_TYPE=basic ${spell_flag}"
  fi
  echo "Creating psi.spec file..."
  local specfile="Summary: Client application for the Jabber network
Name: psi-plus
Version: ${psi_package_version}
Release: 1
License: GPL
Group: Applications/Internet
URL: http://psi-plus.com
Source0: %{name}-%{version}.tar.gz


BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root


BuildRequires: openssl-devel, gcc-c++, zlib-devel
%{!?_without_freedesktop:BuildRequires: desktop-file-utils}


%description
Psi is the premiere Instant Messaging application designed for Microsoft Windows, 
Apple Mac OS X and GNU/Linux. Built upon an open protocol named Jabber,           
si is a fast and lightweight messaging client that utilises the best in open      
source technologies. The goal of the Psi project is to create a powerful, yet     
easy-to-use Jabber/XMPP client that tries to strictly adhere to the XMPP drafts.  
and Jabber JEPs. This means that in most cases, Psi will not implement a feature  
unless there is an accepted standard for it in the Jabber community. Doing so     
ensures that Psi will be compatible, stable, and predictable, both from an end-user 
and developer standpoint.
Psi+ - Psi IM Mod by psi-dev@conference.jabber.ru


%prep
%setup


%build
cmake -DCMAKE_INSTALL_PREFIX=\"%{_prefix}\" -DINSTALL_PLUGINS_SDK=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo ${extraflags} .
%{__make} %{?_smp_mflags}


%install
%{__rm} -rf %{buildroot}


%{__make} DESTDIR=\"%{buildroot}\" install


# Install the pixmap for the menu entry
#%{__install} -Dp -m0644 iconsets/system/default/logo_128.png \
#    %{buildroot}%{_datadir}/pixmaps/psi-plus.png ||:               

mkdir -p %{buildroot}%{_datadir}/psi-plus
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/applications

%post
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :


%postun
touch --no-create %{_datadir}/icons/hicolor || :
%{_bindir}/gtk-update-icon-cache --quiet %{_datadir}/icons/hicolor || :


%clean
%{__rm} -rf %{buildroot}


%files
%defattr(-, root, root, 0755)
%doc COPYING README TODO
%{_bindir}/psi-plus

%{_datadir}/psi-plus/
%{_datadir}/pixmaps/psi-plus.png
%{_datadir}/applications/psi-plus.desktop
"
  if [ "${rpm_oscodename}" != "Fedora" ]; then
    specfile="$(echo ${specfile})
%{_bindir}/psi-plus.debug
%{_datadir}/icons/hicolor/*/apps/psi-plus.png
%exclude %{_datadir}/psi-plus/COPYING
%exclude %{_datadir}/psi-plus/README
"
  fi
  local tmp_spec=${buildpsi}/test.spec
  usr_spec=${rpmspec}/psi-plus.spec
  echo "${specfile}" > ${tmp_spec}
  cp -f ${tmp_spec} ${usr_spec}
}
#Сборка RPM-пакета
build_rpm_package ()
{
  prepare_src
  prepare_tar
  local tar_name=psi-plus-${psi_package_version}
  local sources=${rpmsrc}
  if [ -f "${sources}/${tar_name}.tar.gz" ]; then
    prepare_spec
    echo "Building Psi+ RPM package"
    cd ${rpmspec}
    rpmbuild -ba --clean --rmspec --rmsource ${usr_spec}
    local rpm_ready=$(find $HOME/rpmbuild/RPMS | grep psi-plus)
    local rpm_src_ready=$(find $HOME/rpmbuild/SRPMS | grep psi-plus)
    cp -f ${rpm_ready} ${buildpsi}
    cp -f ${rpm_src_ready} ${buildpsi}
  fi
}
#Подготовка исходников к разработке или исправлению патчей
prepare_dev ()
{
  local psidev=${buildpsi}/psidev
  local orig=${psidev}/git.orig
  local new=${psidev}/git
  rm -rf ${orig}
  rm -rf ${new}
  cd ${buildpsi}
  echo ${psidev}
  check_dir ${psidev}
  check_dir ${orig}
  check_dir ${new}
  if [ ! -d ${upstream_src} ]; then
    down_all
  fi
  cd ${upstream_src}
  prepare_psi_src ${orig}
  prepare_psi_src ${new}
  cd ${psidev}
  if [ ! -f psidiff.ignore ]; then
    wget --no-check-certificate "https://raw.github.com/psi-plus/maintenance/master/scripts/posix/psidiff.ignore" || die "Failed to update psidiff.ignore";
  fi
  if [ ! -f "${psidev}/mkpatch" ]; then
    local mkpatch="#!/bin/bash
diff -urpN -X "psidiff.ignore" git.orig git | sed '/\(.*айлы.*различаются\|Binary.*differ\)\|^diff -urpN/d' | sed 's/^\(\(---\|+++\)\s\+\S\+\).*/\1/'
"
    echo "${mkpatch}">${psidev}/mkpatch
    chmod u+x ${psidev}/mkpatch
  fi
  local patchlist=$(ls ${patches}/ | grep diff)
  cd ${orig}
  echo "---------------------
Patching original src
---------------------">${buildpsi}/${orig##*/}_patching.log
  echo -e "${blue}Enter maximum patch number to patch orig src${nocolor}"
  read patchnumber
  if [ ! -z "$patchnumber" ]; then
    patch_psi $patchnumber
  fi
  cd ${new}
  echo "---------------------
Patching work src
---------------------">>${buildpsi}/${new##*/}_patching.log
  echo -e "${blue}Enter maximum patch number to patch work src${nocolor}"
  read patchnumber
  if [ ! -z "$patchnumber" ]; then
    patch_psi $patchnumber
  fi
}
#Подготовка спек-файла для сборки RPM-пакета плагинов
prepare_plugins_spec ()
{
  local specfile="
Summary: ${summary}
Name: ${progname}
Version: ${rpmver}
Release: 1
License: GPL-2
Group: ${group}
URL: ${urlpath}
Source0: ${package_name}
BuildRequires: ${breq}
Requires: psi-plus
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
${desc}

%prep
%setup

%build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%{buildroot}%{_libdir} -DPLUGINS_PATH=/psi-plus/plugins .
%{__make} %{?_smp_mflags} 

%install
[ \"%{buildroot}\" != \"/\"] && rm -rf %{buildroot}
%{__make} install INSTALL_ROOT=\"%{buildroot}\"

if [ \"%{_target_cpu}\" = \"x86_64\" ] && [ -d \"/usr/lib64\" ]; then
  mkdir -p %{buildroot}/usr/lib64
else
  mkdir -p %{buildroot}/usr/lib
fi

%clean
[ \"%{buildroot}\" != \"/\" ] && rm -rf %{buildroot}

%files
%{_libdir}/psi-plus/plugins
"
  echo "${specfile}" > ${rpmspec}/${progname}.spec
}
#Сборка RPM-пакета плагинов
build_rpm_plugins ()
{
  local progname="psi-plus-plugins"
  fetch_all
  get_psi_plus_version
  local plugdir=${buildpsi}/plugins
  check_dir ${plugdir}
  local b_dir=${buildpsi}/build-plugins
  check_dir ${b_dir}
  cd ${b_dir}
  local rpmver=${psi_package_version}
  local allpluginsdir=${b_dir}/${progname}-${rpmver}
  local package_name="${progname}-${rpmver}.tar.gz"
  local summary="Plugins for psi-plus-${rpmver}"
  if [ "${rpm_oscodename}" == "Fedora" ]; then
    extrasuffix=""
  else
    extrasuffix="2"
  fi
  local breq="libotr${extrasuffix}-devel, libtidy-devel, libgcrypt-devel, libgpg-error-devel"
  local urlpath="https://github.com/psi-plus/plugins"
  local group="Applications/Internet"
  local desc="Plugins for jabber-client Psi+"
  check_dir ${allpluginsdir}
  cp -a ${upstream_src}/cmake ${allpluginsdir}/
  cp -a ${upstream_src}/src/plugins/include ${allpluginsdir}/
  cp -rf ${plugdir}/* ${allpluginsdir}/
  cd ${b_dir}
  tar -pczf $package_name ${progname}-${rpmver}
  prepare_plugins_spec
  cp -rf ${package_name} ${rpmsrc}/
  rpmbuild -ba --clean --rmspec --rmsource ${rpmspec}/${progname}.spec
  echo "Cleaning..."
  cd ${buildpsi}
  rm -rf ${allpluginsdir}
}
#Скачиване ресурсов
get_resources ()
{
  fetch_url "https://github.com/psi-plus/resources.git" ${buildpsi}/resources
}
#Установка ресурсов в домашний каталог
install_resources ()
{
  cd ${buildpsi}
  check_dir ${psi_datadir}
  if [ ! -d "resources" ]; then
    get_resources
  fi
  cp -rf ${buildpsi}/resources/* ${psi_datadir}/
}
#Установка иконок из репы ресурсов в домашний каталог
install_iconsets ()
{
  cd ${buildpsi}
  check_dir ${psi_datadir}
  if [ -d "resources" ]; then
    get_resources
  fi  
  cp -rf ${buildpsi}/resources/iconsets ${psi_datadir}/
}
#Установка скинов из репы ресурсов в домашний каталог
install_skins ()
{
  cd ${buildpsi}
  check_dir ${psi_datadir}
  if [ -d "resources" ]; then
    cp -rf ${buildpsi}/resources/skins ${psi_datadir}/
  else
    get_resources
    cp -rf ${buildpsi}/resources/skins ${psi_datadir}/
  fi 
}
#Установка звуков из репы ресурсов в домашний каталог
install_sounds ()
{
  cd ${buildpsi}
  check_dir ${psi_datadir}
  if [ -d "resources" ]; then
    cp -rf ${buildpsi}/resources/sound ${psi_datadir}/
  else
    get_resources
    cp -rf ${buildpsi}/resources/sound ${psi_datadir}/
  fi 
}
#Установка тем из репы ресурсов в домашний каталог
install_themes ()
{
  cd ${buildpsi}
  check_dir ${psi_datadir}
  if [ -d "resources" ]; then
    cp -rf ${buildpsi}/resources/themes ${psi_datadir}/
  else
    get_resources
    cp -rf ${buildpsi}/resources/themes ${psi_datadir}/
  fi 
}
#Обновление ресурсов
update_resources ()
{
  get_resources
}
#Сборка файлов локализации
build_locales ()
{
  local tr_path=${buildpsi}/langs/translations
  fetch_all
  if [ -d "${tr_path}" ]; then
    rm -f ${tr_path}/*.qm
    if [ -f "/usr/bin/qtchooser" ];then
      lrelease -qt=${qt_ver} ${tr_path}/*.ts
    elif [ -f "/usr/bin/lrelease" ] || [ -f "/usr/local/bin/lrelease" ]; then
      lrelease ${tr_path}/*.ts 
    fi
  fi 
}
#Установка файлов локализации в домашний каталог
install_locales ()
{
  local tr_path=${buildpsi}/langs/translations
  build_locales
  check_dir ${psi_datadir}
  cp -rf ${tr_path}/*.qm ${psi_datadir}/
}
#Запуск собранной версии
run_psiplus ()
{
  local psi_binary_path=${builddir}/psi
  if [ -f "${psi_binary_path}/psi-plus" ];then
    cd ${psi_binary_path}
    ./psi-plus
  else
    echo -e "${red}Psi+ binary not found in ${psi_binary_path}. Try to compile it first.${nocolor}"
  fi
}
#Запуск собранной версии в дебаггере
debug_psi ()
{
  local psi_binary_path=${workdir}/cbuild/psi
  if [ -f "${psi_binary_path}/psi-plus" ];then
    cd ${psi_binary_path}
    gdb ./psi-plus
  else
    echo -e "${red}Psi+ binary not found in ${psi_binary_path}. Try to compile it first.${nocolor}"
  fi
}
#Бэкап и установка переменных окружения для МХЕ
#Команды запуска cmake под МХЕ
prepare_mxe()
{
  OLDPATH=${PATH}
  unset $(env | \
  grep -vi '^EDITOR=\|^HOME=\|^LANG=\|MXE\|^PATH=' | \
  grep -vi 'PKG_CONFIG\|PROXY\|^PS1=\|^TERM=' | \
  cut -d '=' -f1 | tr '\n' ' ')
  export PATH="${mxe_root}/usr/bin:$PATH"
}
run_mxe_cmake()
{
  prepare_mxe
  i686-w64-mingw32.shared-cmake $@
}
run_mxe_cmake_64()
{
  prepare_mxe
  x86_64-w64-mingw32.shared-cmake $@
}
#Кросс-компиляция при помощи МХЕ
compile_psi_mxe()
{
  curd=$(pwd)
  flags=""
  if [ ! -d "${workdir}" ]; then
    prepare_src
  fi
  prepare_builddir ${builddir}
  mxe_outd=${tmp_dir}/mxe_builds
  check_dir ${mxe_outd}
  cd ${builddir}
  if [ "$1" == "qt5" ];then
    current_prefix=${i686_mxe_prefix}
    cmakecmd=run_mxe_cmake
  elif [ "$1" == "qt5_64" ];then
    current_prefix=${x86_64_mxe_prefix}
    cmakecmd=run_mxe_cmake_64
  fi
  if [ ${devm} -eq 1 ]; then
    flags="${flags} -DENABLE_PLUGINS=ON -DBUILD_DEV_PLUGINS=ON -DDEV_MODE=ON"
  fi
  if [ ${wbkt} -eq 0 ]; then
    flags="${flags} -DCHAT_TYPE=basic"
  else
    flags="${flags} -DCHAT_TYPE=webkit"
  fi
  flags="${flags} -DUSE_CCACHE=ON -DVERBOSE_PROGRAM_NAME=ON"
  wrkdir=${builddir}
  check_dir ${wrkdir}
  cd ${wrkdir}
  echo "--Starting cmake
  ${cmakecmd} ${flags} ${workdir}"
  ${cmakecmd} ${flags} ${workdir}
  echo 
  #echo "Press Enter to continue..." && read tmpvar
  ${cmakecmd} --build . --target all -- -j${cpu_count}
  ${cmakecmd} --build . --target prepare-bin --
  if [ ${devm} -eq 1 ]; then
    ${cmakecmd} --build . --target prepare-bin-libs --
  fi
  if [ -d "${mxe_outd}/$1" ] && [ ${devm} -ne 0 ]; then
    cd ${mxe_outd} && rm -rf $1
  fi
  check_dir ${mxe_outd}/$1
  cp -rf ${wrkdir}/psi/*  ${mxe_outd}/$1/
  cp -a ${wrkdir}/psi/translations ${mxe_outd}/$1/
  cp -rf ${buildpsi}/mxe_prepare/* ${mxe_outd}/$1/
  cd ${curd}
  if [ ! -z "${OLDPATH}" ]; then
    PATH=${OLDPATH}
  fi
}
#Сборка 32х битных версий при помощи МХЕ
build_i686_mxe()
{
  wbkt=1
  devm=1
  compile_psi_mxe qt5
  devm=0
  wbkt=0
  compile_psi_mxe qt5
  archivate_all qt5
}
#Сборка 64х битных версий при помощи МХЕ
build_x86_64_mxe()
{
  wbkt=1
  devm=1
  compile_psi_mxe qt5_64
  devm=0
  wbkt=0
  compile_psi_mxe qt5_64
  archivate_all qt5_64
}
#Сборка всех возможных вариантов при помощи МХЕ
build_all_mxe()
{
  build_i686_mxe
  build_x86_64_mxe
}
#Архивация МХЕ сборки при помощи 7z
archivate_all()
{
  wbk_suff="all-"
  mxe_outd=${tmp_dir}/mxe_builds
  7z a -mx=9 -m0=LZMA -mmt=on ${mxe_outd}/psi-plus-${wbk_suff}${psi_package_version}-$1.7z ${mxe_outd}/$1/*
  cp -r ${mxe_outd}/psi-plus-${wbk_suff}${psi_package_version}-$1.7z ${buildpsi}/mxe_builds/
}
#Список зависимостей
check_qt_deps()
{
	check_deps "checkinstall qtmultimedia5-dev libqt5multimedia5-plugins qtbase5-dev qttools5-dev qttools5-dev-tools libqca2-dev pkg-config cmake libqt5x11extras5-dev"
}
#Проверка зависимостей в Ubuntu
check_deps()
{
	if [ ! -z "$1" ]; then
		instdep=""
		for dependency in $1; do
			echo "${dependency}"
			local result=$(dpkg --get-selections | grep ${dependency})
			if [ -z "${result}" ]; then
				echo -e "${blue}Package ${dependency} not installed. Trying to install...${nocolor}"
				instdep="${instdep} ${dependency}"
			fi
		done
		if [ ! -z "${instdep}" ]; then
			sudo apt-get install ${instdep}
		fi
	fi
}
#Запись настроек
set_config ()
{
  local use_webkit="n"
  if [ ! -z "$iswebkit" ]; then
    use_webkit="y"
  else
    use_webkit="n"
  fi
  local is_offline="n"
  if [ ${isoffline} -eq 0 ]; then
    is_offline="n"
  else
    is_offline="y"
  fi
  local skip_patches="n"
  if [ ${skip_invalid} -eq 0 ]; then
    skip_patches="n"
  else
    skip_patches="y"
  fi
  local loop=1
  while [ ${loop} = 1 ];  do
    echo -e "${blue}Choose action TODO:${nocolor}
--${pink}[1]${nocolor} - Set WebKit version to use (current: ${use_webkit})
--${pink}[2]${nocolor} - Set offline mode to use (current: ${is_offline})
--${pink}[3]${nocolor} - Skip Invalid patches (current: ${skip_patches})
--${pink}[4]${nocolor} - Set list of plugins needed to build (for all use *)
--${pink}[5]${nocolor} - Set psi+ spellcheck engine (current: ${spellchek_engine})
--${pink}[6]${nocolor} - Set psi+ sources path (current: ${buildpsi})
--${pink}[7]${nocolor} - Print option values
--${pink}[0]${nocolor} - Do nothing"
    read deistvo
    case ${deistvo} in
      "1" ) echo -e "Do you want use WebKit ${pink}[y/n]${nocolor} ?"
            read variable
            if [ "$variable" == "y" ]; then
              iswebkit="--enable-webkit"
              use_webkit="y"
            else
              iswebkit=""
              use_webkit="n"
            fi;;
      "2" ) echo -e "Do you want to use offline mode ${pink}[y/n]${nocolor} ?"
            read variable
            if [ "$variable" == "y" ]; then
              isoffline=1
              is_offline="y"
            else
              isoffline=0
              is_offline="n"
            fi;;
      "3" ) echo -e "Do you want to skip invalid patches when patching ${pink}[y/n]${nocolor} ?"
            read variable
            if [ "$variable" == "y" ]; then
              skip_invalid=1
              skip_patches="y"
            else
              skip_invalid=0
              skip_patches="n"
            fi;;
      "4" ) echo "Please enter plugins needed to build separated by space (* for all)"
            read variable
            if [ ! -z "$variable" ]; then
              use_plugins=${variable}
            else
              use_plugins=""
            fi;;
      "5" ) echo -e "Please set spellcheck engine for psi+. Available values:${pink}
hunspell
enchant
${nocolor} ?"
            read variable
            if [ ! -z "$variable" ]; then
              spellchek_engine=$variable
            fi;;
      "6" ) echo "Please set psi+ sources path (absolute path, or \$HOME/path)"
            read variable
            if [ ! -z "${variable}" ]; then
              if [ "${variable:0:5}" == "\$HOME" ]; then
                buildpsi=${home}/${variable:6}
              else
                buildpsi=${variable}
              fi
            else
              buildpsi=${default_buildpsi}
            fi;;
      "7" ) echo -e "${blue}==Options==${nocolor}
${green}WebKit${nocolor} = ${yellow}${use_webkit}${nocolor}
${green}Offline Mode${nocolor} = ${yellow}${is_offline}${nocolor}
${green}Skip Invalid Patches${nocolor} = ${yellow}${skip_patches}${nocolor}
${green}Plugins${nocolor} = ${yellow}${use_plugins}${nocolor}
${green}Spellcheck engine${nocolor} = ${yellow}${spellchek_engine}${nocolor}
${green}Psi+ sources path${nocolor} = ${yellow}${buildpsi}${nocolor}
${blue}===========${nocolor}";;
      "0" ) clear
            loop=0;;
    esac
  done
  echo "$iswebkit" > ${config_file}
  echo "$isoffline" >> ${config_file}
  echo "$skip_invalid" >> ${config_file}
  if [ "$use_plugins" == "*" ]; then
    echo "all" >> ${config_file}
  else
    echo "$use_plugins" >> ${config_file}
  fi
  echo "$spellchek_engine" >> ${config_file}
  echo "$buildpsi" >> ${config_file}
  update_variables
}
#Вывод меню
print_menu ()
{
  echo -e "${blue}Choose action TODO!${nocolor}
${pink}[1]${nocolor} - Download All needed source files to build psi+
${pink}[2]${nocolor} - Prepare psi+ sources
${pink}[3]${nocolor} - Build psi+ binary
---${pink}[31]${nocolor} - Build psi+ plugins using
---${pink}[32]${nocolor} - Build psi+ with plugins from snapshots
${pink}[4]${nocolor} - Build Debian package with checkinstall
${pink}[5]${nocolor} - Build openSUSE RPM-package
---${pink}[51]${nocolor} - Build plugins openSUSE RPM-package
${pink}[6]${nocolor} - Set libpsibuild options
${pink}[7]${nocolor} - Prepare psi+ sources for development
${pink}[8]${nocolor} - Get help on additional actions
${pink}[9]${nocolor} - Run compiled psi-plus binary
${pink}[0]${nocolor} - Exit"
}
#Справка по командам
get_help ()
{
  echo -e "${red}---------------HELP-----------------------${nocolor}
${pink}[32]${nocolor} - Build psi-plus with plugins from snapshots
${pink}[71]${nocolor} - Copy Psi+ icons to test build in ${buildpsi}/psidev/git
${pink}[ia]${nocolor} - Install all resources to $psi_datadir
${pink}[ii]${nocolor} - Install iconsets to $psi_datadir
${pink}[is]${nocolor} - Install skins to $psi_datadir
${pink}[iz]${nocolor} - Install sounds to to $psi_datadir
${pink}[it]${nocolor} - Install themes to $psi_datadir
${pink}[il]${nocolor} - Install locales to $psi_datadir
${pink}[bl]${nocolor} - Just build locale files without installing
${pink}[ur]${nocolor} - Update resources
${pink}[bs]${nocolor} - Backup ${buildpsi##*/} directory in ${buildpsi%/*}
${pink}[pw]${nocolor} - Prepare psi+ workspace (clean ${buildpsi}/build dir)
${pink}[dp]${nocolor} - Run psi-plus binary under gdb debugger
${pink}[bam]${nocolor} - Build both 32bit and 64bit builds with MXE
${red}-------------------------------------------${nocolor}
${blue}Press Enter to continue...${nocolor}"
  read
}
#Обработка команд
choose_action ()
{
  read vibor
  case ${vibor} in
    "1" ) down_all;;
    "2" ) prepare_src;;
    "3" ) compile_psiplus /usr;;
    "31" ) build_cmake_plugins;;
    "32" ) build_all_psiplus;;
    "4" ) build_deb_package;;
    "5" ) build_rpm_package;;
    "51" ) build_rpm_plugins;;
    "6" ) set_config;;
    "7" ) prepare_dev;;
    "71" ) copy_psiplus_icons ${buildpsi}/psidev/git;;
    "8" ) get_help;;
    "9" ) run_psiplus;;
    "ia" ) install_resources;;
    "ii" ) install_iconsets;;
    "is" ) install_skins;;
    "iz" ) install_sounds;;
    "it" ) install_themes;;
    "ur" ) update_resources;;
    "il" ) install_locales;;
    "bl" ) build_locales;;
    "bs" ) backup_tar;;
    "pw" ) prepare_workspace;;
    "dp" ) debug_psi;;
    "bam" ) build_all_mxe;;
    "0" ) quit;;
  esac
}
#Создание файла настроек
cd ${githubdir}
read_options
if [ ! -f "${config_file}" ]; then
  set_config
fi
clear
#Цикл меню
while true; do
  print_menu
  choose_action
done
exit 0
