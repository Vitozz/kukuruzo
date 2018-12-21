# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/net-im/psi/psi-plugins-9999.ebuild,v 1.12 2011/06/30 09:23:16 pva Exp $

EAPI=5

PSI_PLUS_URI="git://github.com/psi-im"
EGIT_REPO_URI="${PSI_PLUS_URI}/plugins.git"
EGIT_MIN_CLONE_TYPE="single"
EGIT_SUBMODULES=()

inherit cmake-utils eutils git-r3

DESCRIPTION="Psi plugins"
HOMEPAGE="http://psi-im.org/"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS=""

PSI_PLUGINS="
attentionplugin
autoreplyplugin
battleshipgameplugin
birthdayreminderplugin
chessplugin
cleanerplugin
conferenceloggerplugin
contentdownloaderplugin
enummessagesplugin
extendedmenuplugin
extendedoptionsplugin
gnupgplugin
gomokugameplugin
historykeeperplugin
httpuploadplugin
icqdieplugin
imageplugin
imagepreviewplugin
jabberdiskplugin
juickplugin
messagefilterplugin
omemoplugin
otrplugin
pepchangenotifyplugin
qipxstatusesplugin
ripperccplugin
screenshotplugin
skinsplugin
stopspamplugin
storagenotesplugin
translateplugin
videostatusplugin
watcherplugin
"

IUSE="${PSI_PLUGINS}"

RDEPEND="
	dev-qt/qtwidgets:5
	dev-qt/qtxml:5
	net-im/psi-plus
	httpuploadplugin? ( dev-qt/qtnetwork:5 )
	contentdownloaderplugin? ( dev-qt/qtnetwork:5 )
	videostatusplugin? (
		dev-qt/qtx11extras
		dev-qt/qtdbus:5
	)
	screenshotplugin? (
		dev-qt/qtnetwork:5
		dev-qt/qtprintsupport:5
		dev-qt/qtx11extras
	)
	juickplugin? (
		dev-qt/qtcore:5
		dev-qt/qtnetwork:5
	)
	imagepreviewplugin? (
		dev-qt/qtcore:5
		dev-qt/qtnetwork:5
	)
	otrplugin? ( dev-qt/qtconcurrent )
	omemoplugin? (
		app-crypt/libsignal-protocol-c
		dev-libs/openssl
	)
	ripperccplugin? (
		dev-qt/qtgui:5
		dev-qt/qtnetwork:5
	)
	otrplugin? (
		net-libs/libotr
		app-text/htmltidy
		dev-libs/libgpg-error
		dev-libs/libgcrypt
	)
"

RESTRICT="test"

pkg_setup() {
	MY_PN=psi-plugins
}

src_unpack() {
	git-r3_src_unpack
}

src_configure() {
	local plugins=""
#	local EXTRA_FLAGS="-DONLY_PLUGINS=ON"
	if use ripperccplugin || use battleshipgameplugin; then
		EXTRA_FLAGS="${EXTRA_FLAGS} -BUILD_DEV_PLUGINS=ON"
	fi
	for p in ${PSI_PLUGINS}; do
		use "${p}" && plugins="${plugins};${p}"
	done
	local mycmakeargs=(
		$(echo -DCMAKE_INSTALL_PREFIX=/usr)
		$(echo ${EXTRA_FLAGS} -DBUILD_PLUGINS=${plugins})
	)
	cmake-utils_src_configure
}

src_compile() {
	cmake-utils_src_compile
}

src_install() {
	cmake-utils_src_install
}

