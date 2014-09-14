# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=4

inherit cmake-utils git-2
IUSE="pulseaudio"

DEPEND="
	>=dev-cpp/gtkmm-3.0
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

PULSE_FLAG="OFF"
use pulseaudio && PULSE_FLAG="ON"
src_configure() {
	mycmakeargs="${mycmakeargs}
				-DUSE_PULSE='${PULSE_FLAG}'
				"
	cmake-utils_src_configure
}
