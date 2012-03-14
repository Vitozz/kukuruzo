#!/bin/bash
#homedir=$HOME
#srcdir=${homedir}/kukuruzorepo
#builddir=${srcdir}/build
#exitdir=${srcdir}/debians

check_dir ()
{
	if [ ! -z "$1" ]
	then
		if [ ! -d "$1" ]
		then
			tmpdir=$1			
			echo "making directory $tmpdir..."
			cd ${builddir}
			mkdir "$tmpdir"
		fi
	fi
}

set_workdir ()
{
	if [ ! -z "$1" ]
	then
		echo "Workdir = $1"
		workdir="$1"
	fi
}

get_src ()
{
	if [ -d "${srcdir}/.git" ]
	then
		update_git ${srcdir}
	else
		cd ${homedir}
		git clone "git://github.com/Vitozz/kukuruzo.git"  ${srcdir}
		cd ${srcdir}
		git init
		git pull
	fi
}

get_pyav ()
{
	tmp_dir="${srcdir}/pyalsavolume"
	check_dir ${tmp_dir}
	if [ -d "${tmp_dir}/.git" ]
	then
		update_git ${tmp_dir}
	else
		git clone "git://github.com/Vitozz/pyalsavolume.git"  ${tmp_dir}
		cd ${tmp_dir}
		git init
		git pull
		cd ${srcdir}
	fi
}

get_pypoff ()
{
	tmp_dir="${srcdir}/pypoweroff"
	check_dir ${tmp_dir}
	if [ -d "${tmp_dir}/.git" ]
	then
		update_git ${tmp_dir}
	else
		git clone "git://github.com/Vitozz/pypoweroff.git"  ${tmp_dir}
		cd ${tmp_dir}
		git init
		git pull
		cd ${srcdir}
	fi
}

get_pyssh ()
{
	tmp_dir="${srcdir}/pysshclient"
	check_dir ${tmp_dir}
	if [ -d "${tmp_dir}/.git" ]
	then
		update_git ${tmp_dir}
	else
		git clone "git://github.com/Vitozz/pysshclient.git"  ${tmp_dir}
		cd ${tmp_dir}
		git init
		git pull
		cd ${srcdir}
	fi
}

get_html2text ()
{
	tmp_dir="${srcdir}/htmltotextgui"
	check_dir ${tmp_dir}
	if [ -d "${tmp_dir}/.git" ]
	then
		update_git ${tmp_dir}
	else
		git clone "git://github.com/Vitozz/htmltotextgui.git"  ${tmp_dir}
		cd ${tmp_dir}
		git init
		git pull
		cd ${srcdir}
	fi
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
	if [ ! -z "$1" ]
	then
		if [ -d "$1" ]
		then
			cd "$1"
			git reset --hard
			git init
			git pull
		fi
	fi
}

