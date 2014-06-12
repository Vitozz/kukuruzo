# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=4

inherit cmake-utils git-2
IUSE="pulseaudio"
DEPEND="
	dev-qt/qtgui
	media-libs/alsa-lib
	pulseaudio? ( media-sound/pulseaudio )
"
RDEPEND="
	${DEPEND}
	dev-qt/qtcore
"
DESCRIPTION="Tray ALSA volume changer written using Qt library"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-3"

src_configure() {
	if use pulseaudio; then
		mycmakeargs="${mycmakeargs} -DUSE_PULSE=OK"
		cmake-utils_src_configure
	fi
}

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/qtalsavolume"
}
