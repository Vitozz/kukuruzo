# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

inherit cmake-utils git-2

IUSE="pulseaudio gtk2 gtk3"

REQUIRED_USE="^^ ( gtk2 gtk3 )"

DEPEND="
	gtk3? ( >=dev-cpp/gtkmm-3.0 )
	gtk2? ( >=dev-cpp/gtkmm-2.4 )
	media-libs/alsa-lib
	pulseaudio? ( media-sound/pulseaudio )
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
	PULSE_FLAG="OFF"
	GTK3_FLAG="ON"
	use pulseaudio && PULSE_FLAG="ON"
	use gtk2 && GTK3_FLAG="OFF"
	mycmakeargs="${mycmakeargs}
				-DUSE_PULSE='${PULSE_FLAG}'
				-DUSE_GTK3='${GTK3_FLAG}'
				"
	cmake-utils_src_configure
}
