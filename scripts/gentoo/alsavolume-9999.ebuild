# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

inherit cmake-utils git-2

IUSE="appindicator gtk2 gtk3 kde pulseaudio"

REQUIRED_USE="^^ ( gtk2 gtk3 )"

DEPEND="
	gtk3? (
		>=dev-cpp/gtkmm-3.0
		appindicator? ( dev-libs/libappindicator:3 )
		)
	gtk2? (
		>=dev-cpp/gtkmm-2.4
		appindicator? ( dev-libs/libappindicator:2 )
		)
	media-libs/alsa-lib
	pulseaudio? ( media-sound/pulseaudio )
	dev-util/intltool
	virtual/libintl
	sys-devel/gettext
"
RDEPEND="
	${DEPEND}
"
DESCRIPTION="Tray ALSA volume changer written using gtkmm library"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/cppAlsaVolume.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-2"

src_configure() {
	local mycmakeargs=(
		$(cmake-utils_use_use pulseaudio PULSE)
		$(cmake-utils_use_use appindicator APPINDICATOR)
		$(cmake-utils_use_use kde KDE)
		$(cmake-utils_use_use gtk3 GTK3)
	)
	cmake-utils_src_configure
}
