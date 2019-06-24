# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

PLOCALES="be bg ca cs de en eo es et fa fi fr he hu it ja kk mk nl pl pt pt_BR ru sk sl sr@latin sv sw uk ur_PK vi zh_CN zh_TW"
PLOCALE_BACKUP="en"

inherit cmake-utils eutils l10n multilib git-r3 xdg-utils qmake-utils

PSI_PLUS_URI="git://github.com/psi-plus"
EGIT_REPO_URI="${PSI_PLUS_URI}/psi-plus-snapshots.git"
PSI_PLUS_LANGS_URI="${PSI_PLUS_URI}/psi-plus-l10n.git"
EGIT_MIN_CLONE_TYPE="single"

DESCRIPTION="Qt5 Jabber client, with Licq-like interface"
HOMEPAGE="http://psi-plus.com/"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS=""
IUSE="crypt debug doc enchant +extras +hunspell jingle iconsets keychain ssl +plugins xscreensaver webengine webkit webp"

REQUIRED_USE="
	?? ( enchant hunspell )
	iconsets? ( extras )
	webengine? ( !webkit )
"

RDEPEND="
	dev-qt/qtconcurrent:5
	dev-qt/qtcore:5
	dev-qt/qtdbus:5
	dev-qt/qtgui:5
	dev-qt/qtmultimedia:5
	dev-qt/qtnetwork:5
	dev-qt/qtsvg:5
	dev-qt/qtsql:5[sqlite]
	dev-qt/qtwidgets:5
	dev-qt/qtx11extras:5
	dev-qt/qtxml:5
	net-dns/libidn:0
	sys-libs/zlib[minizip]
	x11-libs/libX11
	x11-libs/libxcb
	enchant? ( >=app-text/enchant-1.3.0 )
	hunspell? ( app-text/hunspell:= )
	keychain? ( dev-libs/qtkeychain )
	webkit? ( dev-qt/qtwebkit:5	)
	webengine? (
		dev-qt/qtwebchannel:5
		dev-qt/qtwebengine:5[widgets]
		net-libs/http-parser
	)
	xscreensaver? ( x11-libs/libXScrnSaver )
"
DEPEND="${RDEPEND}
	dev-qt/linguist-tools:5
	virtual/pkgconfig
	doc? ( app-doc/doxygen )
"
PDEPEND="
	crypt? ( app-crypt/qca[gpg] )
	jingle? (
		net-im/psimedia[extras?]
		app-crypt/qca:2[ssl]
	)
	ssl? ( app-crypt/qca:2[ssl] )
	webp? ( dev-qt/qtimageformats )
"
RESTRICT="test iconsets? ( bindist )"

src_unpack() {
	git-r3_src_unpack

	# fetch translations
	unset EGIT_BRANCH EGIT_COMMIT
	EGIT_REPO_URI="${PSI_PLUS_LANGS_URI}"
	EGIT_CHECKOUT_DIR="${WORKDIR}/psi-plus-l10n"
	git-r3_src_unpack

	if use iconsets; then
		unset EGIT_BRANCH EGIT_COMMIT
		EGIT_CHECKOUT_DIR="${WORKDIR}/resources" \
		EGIT_REPO_URI="${PSI_PLUS_URI}/resources.git" \
		git-r3_src_unpack
	fi
}

src_prepare() {
	if use iconsets; then
		cp -a "${WORKDIR}/resources/iconsets" "${S}" || die
	fi
	eapply_user
}

src_configure() {
	use debug && EXTRA_FLAGS="$EXTRA_FLAGS -DCMAKE_BUILD_TYPE=Debug"
	use webengine && EXRTA_FLAGS="$EXTRA_FLAGS -DCHAT_TYPE=webengine"
	use webkit && EXRTA_FLAGS="$EXTRA_FLAGS -DCHAT_TYPE=webkit"
	if ! use webengine && ! use webkit; then
		EXRTA_FLAGS="$EXTRA_FLAGS -DCHAT_TYPE=basic"
	fi
	local mycmakeargs=(
		-DUSE_ENCHANT="$(usex enchant)"
		-DUSE_HUNSPELL="$(usex hunspell)"
		-DINSTALL_PLUGINS_SDK="$(usex plugins)"
		-DUSE_KEYCHAIN="$(usex keychain)"
		-DCMAKE_INSTALL_PREFIX="/usr"
		-DINSTALL_EXTRA_FILES="$(usex extras)"
		$(echo ${EXTRA_FLAGS})
	)
	cmake-utils_src_configure
}

src_compile() {
	cmake-utils_src_compile
	use doc && emake -C doc api_public
}

src_install() {
	cmake-utils_src_install

	# this way the docs will be installed in the standard gentoo dir
	rm "${ED}"/usr/share/${PN}/{COPYING,README.html} || die "Installed file set seems to be changed by upstream"
	newdoc iconsets/roster/README README.roster
	newdoc iconsets/system/README README.system
	newdoc certs/README README.certs
	dodoc README.html

	use doc && HTML_DOCS=( doc/api/. )
	einstalldocs

	# install translations
	local mylrelease="$(qt5_get_bindir)"/lrelease
	cd "${WORKDIR}/${PN}-l10n" || die
	insinto /usr/share/${PN}
	install_locale() {
		"${mylrelease}" "translations/psi_${1}.ts" || die "lrelease ${1} failed"
		doins "translations/psi_${1}.qm"
	}
	l10n_for_each_locale_do install_locale
}

pkg_postinst() {
	xdg_icon_cache_update
	xdg_desktop_database_update
}

pkg_postrm() {
	xdg_icon_cache_update
	xdg_desktop_database_update
}
