# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /var/cvsroot/gentoo-x86/net-im/psi/psi-plugins-9999.ebuild,v 1.12 2011/06/30 09:23:16 pva Exp $

EAPI=5

PSI_URI="git://github.com/psi-im"
EGIT_REPO_URI="${PSI_URI}/psi.git"
PSI_PLUGINS_URI="${PSI_URI}/plugins.git"
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

IUSE="${PSI_PLUGINS} webengine qt4 qt5"

REQUIRED_USE="^^ ( qt4 qt5 )"

RDEPEND="
	qt4? (
		dev-qt/qtcore:4
		dev-qt/qtgui:4
		ripperccplugin? (
			dev-qt/qtgui:4
			dev-qt/qtnetwork:4
			dev-libs/qjson
		)
		httpuploadplugin? ( dev-qt/qtnetwork:4 )
		contentdownloaderplugin? ( dev-qt/qtnetwork:4 )
		videostatusplugin? (
			dev-qt/qtx11extras
			dev-qt/qtdbus:4
		)
		screenshotplugin? (
			dev-qt/qtnetwork:4
			dev-qt/qtx11extras
		)
		juickplugin? (
			dev-qt/qtnetwork:4
			dev-qt/qtwebkit:4
		)
		imagepreviewplugin? (
			dev-qt/qtnetwork:4
			dev-qt/qtwebkit:4
		)
	)
	qt5? (
		dev-qt/qtwidgets:5
		dev-qt/qtxml:5
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
			webengine? ( >=dev-qt/qtwebengine-5.7:5 )
			!webengine? ( dev-qt/qtwebkit:5 )
		)
		imagepreviewplugin? (
			dev-qt/qtcore:5
			dev-qt/qtnetwork:5
			webengine? ( >=dev-qt/qtwebengine-5.7:5 )
			!webengine? ( dev-qt/qtwebkit:5 )
		)
		otrplugin? ( dev-qt/qtconcurrent )
		ripperccplugin? (
			dev-qt/qtgui:5
			dev-qt/qtnetwork:5
		)
	)
	otrplugin? (
		net-libs/libotr
		app-text/htmltidy
		dev-libs/libgpg-error
		dev-libs/libgcrypt
	)
"
DEPEND="${RDEPEND}
net-im/psi[plugins]
"
RESTRICT="test"

pkg_setup() {
	MY_PN=psi-plugins
}

src_unpack() {
	git-r3_src_unpack
	unset EGIT_BRANCH EGIT_COMMIT
	EGIT_CHECKOUT_DIR="${WORKDIR}/${P}/src/plugins" \
	EGIT_REPO_URI="${PSI_PLUGINS_URI}" \
	git-r3_src_unpack
}

src_configure() {
	local plugins=""
	local EXTRA_FLAGS="-DONLY_PLUGINS=ON"
	if use ripperccplugin || use battleshipgameplugin; then
		EXTRA_FLAGS="${EXTRA_FLAGS} -BUILD_DEV_PLUGINS=ON"
	fi
	for p in ${PSI_PLUGINS}; do
		use "${p}" && plugins="${plugins};${p}"
	done
	local mycmakeargs=(
		$(cmake-utils_use_use qt5 QT5)
		$(echo -DCMAKE_INSTALL_PREFIX=/usr)
		$(echo ${EXTRA_FLAGS} -DBUILD_PLUGINS=${plugins})
	)
	cmake-utils_src_configure
}

src_compile() {
	cmake-utils_src_compile
}

src_install() {
	#emake INSTALL_ROOT="${D}" install
	cmake-utils_src_install
}

