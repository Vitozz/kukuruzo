# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

inherit cmake-utils git-r3

DEPEND="
	dev-qt/qtcore:5
	dev-qt/qtgui:5
	dev-qt/qtwidgets:5
	dev-qt/qtdbus:5
"
RDEPEND="
	${DEPEND}
"

DESCRIPTION="Simple program written on Qt to sheduled system shutdown / reboot"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"
EGIT_MIN_CLONE_TYPE="single"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-2"

src_configure() {
	cmake-utils_src_configure
}

src_prepare() {
	echo "WORKDIR ${WORKDIR}"
	S="${WORKDIR}/${PN}-${PV}/qt/qtpoweroff"
	eapply_user
}
