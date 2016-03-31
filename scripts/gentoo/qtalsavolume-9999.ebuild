# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

inherit cmake-utils git-2

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
	KDE_FLAG="OFF"
	if use qt5 && use kde; then
			KDE_FLAG="-DUSE_KDE5=ON"
	fi
	if use qt4 && use kde; then
			KDE_FLAG="-DUSE_KDE=ON"
	fi
	local mycmakeargs=(
		$(cmake-utils_use_use pulseaudio PULSE)
		$(cmake-utils_use_use qt5 QT5)
		$(echo ${KDE_FLAG})
	)
	cmake-utils_src_configure
}

src_prepare() {
	S="${EGIT_SOURCEDIR}/qt/qtalsavolume"
}
