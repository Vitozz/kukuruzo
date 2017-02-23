# Copyright 2013-2017 Vitaly Tonkacheyev
# Distributed under the terms of the GNU General Public License v3

EAPI=6

inherit cmake-utils git-r3

IUSE="kde pulseaudio qt4 qt5"

REQUIRED_USE="^^ ( qt4 qt5 )"

DEPEND="
	qt4? (
		dev-qt/qtcore:4
		dev-qt/qtgui:4
		kde? (
			kde-base/kdelibs:4
			dev-qt/qtdbus:4
		)
	)
	qt5? (
		dev-qt/qtcore:5
		dev-qt/qtgui:5
		dev-qt/qtwidgets:5
		kde? (
			kde-frameworks/knotifications
			dev-qt/qtdbus:5
		)
	)
	media-libs/alsa-lib
	pulseaudio? ( media-sound/pulseaudio )
"
RDEPEND="
	${DEPEND}
"
DESCRIPTION="Tray ALSA volume changer written using Qt library"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-2"

src_configure() {
	local mycmakeargs=(
		use -DUSE_PULSE="$(usex pulseaudio)"
		use -DUSE_QT5="$(usex qt5)"
		use -DUSE_KDE="$(usex kde)"
	)
	cmake-utils_src_configure
}

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/qtalsavolume"
}
