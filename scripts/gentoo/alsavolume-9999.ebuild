# Copyright 2013-2017 Vitaly Tonkacheyev
# Distributed under the terms of the GNU General Public License v3

EAPI=6

inherit cmake-utils git-r3

IUSE="appindicator gtk2 gtk3 kde pulseaudio"

REQUIRED_USE="
	^^ ( gtk2 gtk3 )
	appindicator? ( !kde )
	kde? ( !appindicator )
	"

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
		use -DUSE_PULSE="$(usex pulseaudio)"
		use -DUSE_APPINDICATOR="$(usex appindicator)"
		use -DUSE_KDE="$(usex kde)"
		use -DUSE_GTK3="$(usex gtk3)"
	)
	cmake-utils_src_configure
}
