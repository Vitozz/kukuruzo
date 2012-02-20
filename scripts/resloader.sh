#!/bin/bash
homedir=$HOME
srcdir=${homedir}/kukuruzorepo
builddir=${srcdir}/build
exitdir=${srcdir}/debians

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
	cd ${homedir}
	git clone "git://github.com/Vitozz/kukuruzo.git"  ${srcdir}
	cd ${srcdir}
	git init
	git pull
}

get_pyav ()
{
	check_dir ${srcdir}/pyalsavolume
	git clone "git://github.com/Vitozz/pyalsavolume.git"  ${srcdir}/pyalsavolume
	cd ${srcdir}/pyalsavolume
	git init
	git pull
	cd ${srcdir}
}

get_pypoff ()
{
	check_dir ${srcdir}/pypoweroff
	git clone "git://github.com/Vitozz/pypoweroff.git"  ${srcdir}/pypoweroff
	cd ${srcdir}/pypoweroff
	git init
	git pull
	cd ${srcdir}
}

get_pyssh ()
{
	check_dir ${srcdir}/pysshclient
	git clone "git://github.com/Vitozz/pysshclient.git"  ${srcdir}/pysshclient
	cd ${srcdir}/pysshclient
	git init
	git pull
	cd ${srcdir}
}

get_html2text ()
{
	check_dir ${srcdir}/htmltotextgui
	git clone "git://github.com/Vitozz/htmltotextgui.git"  ${srcdir}/htmltotextgui
	cd ${srcdir}/htmltotextgui
	git init
	git pull
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

