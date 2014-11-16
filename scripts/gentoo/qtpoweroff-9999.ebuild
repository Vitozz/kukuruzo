# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

inherit cmake-utils git-2

IUSE="qt4 qt5"

REQUIRED_USE="qt4? ( !qt5 )"
REQUIRED_USE="qt5? ( !qt4 )"

DEPEND="
	qt4? (
		dev-qt/qtcore
		dev-qt/qtgui
		dev-qt/qtdbus
	)
	qt5? (
		dev-qt/qtcore:5
		dev-qt/qtgui:5
		dev-qt/qtwidgets:5
		dev-qt/qtdbus:5
	)
"
RDEPEND="
	${DEPEND}
"

DESCRIPTION="Simple program written on Qt to sheduled system shutdown / reboot"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-2"

use qt5 && QT_FLAG="ON"
use qt4 && QT_FLAG="OFF"

src_configure() {
	mycmakeargs="${mycmakeargs}
				-DUSE_QT5='${QT_FLAG}'
				"
	cmake-utils_src_configure
}

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/qtpoweroff"
}
