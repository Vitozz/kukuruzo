# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

inherit cmake-utils git-r3

DEPEND="
		dev-qt/qtcore:5
		dev-qt/qtgui:5
		dev-qt/qtwidgets:5
"
RDEPEND="
	${DEPEND}
"

DESCRIPTION="Simple tool written on Qt to test regular expressions"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-3"

src_configure() {
	cmake-utils_src_configure
}

src_prepare() {
	S=${WORKDIR}/${P}/qt/regexptest
	eapply_user
}
