
#!/bin/sh
set -eu
NAME=cryptofile
VERSION=$(cat VERSION)
ARCH=$(dpkg --print-architecture)
ROOT=build/pkg/${NAME}_${VERSION}_${ARCH}

export CFLAGS="$(dpkg-buildflags --get CFLAGS)"
export LDFLAGS="$(dpkg-buildflags --get LDFLAGS)"
export CPPFLAGS="$(dpkg-buildflags --get CPPFLAGS)"

cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TESTING=OFF
cmake --build build/release
rm -rf "$ROOT"
DESTDIR="$PWD/$ROOT" cmake --install build/release --strip

mkdir -p debian && touch debian/control
DEPENDS=$(dpkg-shlibdeps -O -e"$ROOT/usr/bin/$NAME" | sed 's/shlibs:Depends=//')
rm -rf debian

mkdir -p "$ROOT/DEBIAN" "$ROOT/usr/share/doc/$NAME" dist
sed -e "s/@VERSION@/$VERSION/" -e "s/@ARCH@/$ARCH/" -e "s/@DEPENDS@/$DEPENDS/" \
  packaging/DEBIAN/control > "$ROOT/DEBIAN/control"
install -m 0644 packaging/DEBIAN/conffiles "$ROOT/DEBIAN/conffiles"

install -m 0644 packaging/copyright "$ROOT/usr/share/doc/$NAME/copyright"
gzip -9n -c packaging/changelog > "$ROOT/usr/share/doc/$NAME/changelog.gz"

mkdir -p "$ROOT/etc/cryptofile"
install -m 0644 packaging/cryptofile.conf "$ROOT/etc/cryptofile/cryptofile.conf"

find "$ROOT" -type d -exec chmod 0755 {} +

dpkg-deb --root-owner-group --build "$ROOT" dist/
