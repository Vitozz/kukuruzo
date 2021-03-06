# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=5

PYTHON_DEPEND="2:2.7"
SUPPORT_PYTHON_ABIS="1"
RESTRICT_PYTHON_ABIS="3.*"

inherit distutils git-2
DEPEND="
	>=dev-python/pygobject-3.0.0
	dev-python/chardet
"
RDEPEND="${DEPEND}"
DESCRIPTION="HTML To TXT File Converter"
HOMEPAGE="http://sites.google.com/site/thesomeprojects/"
EGIT_REPO_URI="git://github.com/Vitozz/kukuruzo.git"

SLOT="0"
KEYWORDS="~amd64 ~x86 ~amd64-linux ~x86-linux"
LICENSE="GPL-3"

src_compile() {
	distutils_src_compile
}

src_install() {
	distutils_src_install
}

src_prepare() {
	S=${EGIT_SOURCEDIR}/python/htmltotextgui
}