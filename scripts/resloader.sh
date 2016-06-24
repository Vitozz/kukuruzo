#!/bin/bash
#homedir=$HOME
#srcdir=${homedir}/kukuruzorepo
#builddir=${srcdir}/build
#exitdir=${srcdir}/debians
#COLORS
red="\e[0;31m"
green="\e[0;32m"
nocolor="\x1B[0m"
pink="\x1B[01;91m"
yellow="\x1B[01;93m"
blue="\x1B[01;94m"
#
kukruzo_url="git://github.com/Vitozz/kukuruzo.git"
pyalsavolume_url="git://github.com/Vitozz/pyalsavolume.git"
pypoweroff_url="git://github.com/Vitozz/pypoweroff.git"
cppAlsaVolume_url="git://github.com/Vitozz/cppAlsaVolume.git"

check_dir ()
{
	if [ ! -z "$1" ]
	then
		if [ ! -d "$1" ]
		then
			tmpdir=$1			
			echo -e "${blue}creating directory${nocolor} ${pink}$tmpdir...${nocolor}"
			if [ -d "${builddir}" ]; then
				cd ${builddir}
			else
				mkdir -pv ${builddir}
			fi
			mkdir -pv "$tmpdir"
		fi
	fi
}

set_workdir ()
{
	if [ ! -z "$1" ]
	then
		echo -e "${blue}Workdir =${nocolor} ${pink}$1${nocolor}"
		workdir="$1"
	fi
}

get_src ()
{
	check_dir ${srcdir}
	update_git ${kukruzo_url} ${srcdir}
}

get_pyav ()
{
	tmp_dir="${srcdir}/pyalsavolume"
	check_dir ${tmp_dir}
	update_git ${pyalsavolume_url} ${tmp_dir}
	cd ${srcdir}
}

get_pypoff ()
{
	tmp_dir="${srcdir}/pypoweroff"
	check_dir ${tmp_dir}
	update_git ${pypoweroff_url} ${tmp_dir}
	cd ${srcdir}
}

get_avolume ()
{
	tmp_dir="${srcdir}/cppAlsaVolume"
	check_dir ${tmp_dir}
	update_git ${cppAlsaVolume_url} ${tmp_dir}
	cd ${srcdir}
}

prepare_src ()
{
	check_dir ${builddir}
	rm -rf ${builddir}/*
	check_dir ${exitdir}
	if [ ! -z "$1" ]
	then
		dirname="$1"
		check_dir ${builddir}/${workdir}
		cp -rf ${srcdir}/${dirname}/* ${builddir}/${workdir}/
	fi
}

update_git ()
{
	curd=$(pwd)
	if [ ! -z "$2" ]
	then
		if [ ! -d "$2/.git" ]
		then
			git clone $1 $2
			cd "$2"
			git init
		else
			cd "$2"
			git reset --hard
			git init
			git pull
		fi
	fi
	cd ${curd}
}

