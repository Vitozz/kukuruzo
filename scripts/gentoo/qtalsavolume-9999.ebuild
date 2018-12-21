# Copyright 2013-2017 Vitaly Tonkacheyev
# Distributed under the terms of the GNU General Public License v3

EAPI=6

inherit cmake-utils git-r3

IUSE="kde pulseaudio"

DEPEND="
	dev-qt/qtcore:5
	dev-qt/qtgui:5
	dev-qt/qtwidgets:5
	kde? (
		kde-frameworks/knotifications
		dev-qt/qtdbus:5
	)
	media-libs/alsa-lib
	pulseaudio? ( media-sound/pulseaudio )
"
RDEPEND="
	${DEPEND}
"
DESCRIPTION="Tray ALSA volume changer written using Qt library"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="https://github.com/Vitozz/kukuruzo.git"
EGIT_MIN_CLONE_TYPE="single"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-2"

src_configure() {
	local mycmakeargs=(
		-DUSE_PULSE="$(usex pulseaudio)"
		-DUSE_KDE="$(usex kde)"
	)
	cmake-utils_src_configure
}

src_prepare() {
	S="${WORKDIR}/${P}/qt/qtalsavolume"
	eapply_user
}
