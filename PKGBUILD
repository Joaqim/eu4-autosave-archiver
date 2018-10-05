# Maintainer: Your Name <joaqimpla@gmail.com>
#_target=mingw-w64
_target=x86_64-pc-linux
#_target=x86_64-mingw-w64
#_sysroot=/usr/local/${_target}

_sysroot=/usr
_name=plan


#pkgname=${_target}-${_name}
pkgname=${_name}
#pkgver=$(git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')
pkgver=0.1
pkgrel=2
epoch=
pkgdesc="TODO: Package Description"
arch=("x86_64")
url=""
license=('GPL')
groups=()
depends=("opencv" "glfw" "glew" "tesseract-ocr-git" "poppler")
#depends=("${_target}-opencv" "${_target}-glfw" "${_target}-glew" "${_target}-tesseract-ocr-git" "${_target}-poppler")
makedepends=()
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
#source=("$pkgname-$pkgver.pkg.tar")
        #"$pkgname-$pkgver.patch")
#source=("ftp://ftp.gnu.org/gnu/sed/$pkgname-$pkgver.tar.xz")
#source=("https://github.com/Zenoobia/plan/releases/download/${pkgver}-${pkgrel}/${pkgname}-${pkgver}-${pkgrel}-${arch}.pkg.tar")
noextract=()

validpgpkeys=()


#_target=x86_64-
prepare() {
	cd ..
	./autogen.sh
}


build() {

	../configure --prefix=${_sysroot} --bindir=${_sysroot}/bin \
			     --with-sysroot=${_sysroot} \
			     --build=$CHOST --host=$CHOST --target=${_target} \
			     --disable-debug
	make
	}

package() {

	make DESTDIR="${pkgdir}"/ install

	# clean-up cross compiler root
	#rm -r ${pkgdir}/${_sysroot}/{bin,man}
	#rm -r ${pkgdir}/${_sysroot}/bin
}

install() {
	make DESTDIR="${_sysroot}" install
}
