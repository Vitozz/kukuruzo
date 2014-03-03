# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=4

inherit cmake-utils git-2

DEPEND="
	dev-qt/qtgui
"
RDEPEND="
	${DEPEND}
	dev-qt/qtcore
"

DESCRIPTION="Simple tool written on Qt to test regular expressions"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-3"

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/regexptest"
}
