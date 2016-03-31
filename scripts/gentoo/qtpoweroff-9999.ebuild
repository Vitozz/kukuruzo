# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

inherit cmake-utils git-2

IUSE="qt4 qt5"

REQUIRED_USE="^^ ( qt4 qt5 )"

DEPEND="
	qt4? (
		dev-qt/qtcore:4
		dev-qt/qtgui:4
		dev-qt/qtdbus:4
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

src_configure() {
	local mycmakeargs=(
		$(cmake-utils_use_use qt5 QT5)
	)
	cmake-utils_src_configure
}

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/qtpoweroff"
}
