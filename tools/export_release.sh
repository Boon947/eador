set -e ### Exit immediately if a command fails
LC_CTYPE=ru_RU.cp1251

if [ -z "$1" ]; then echo "Usage: $0 ARCHIVE"; exit; fi

VERSION=$(grep -o 'New Horizons[^#]*' en/Text/message.txt | tr ' ' '_')
DEST=/tmp/$VERSION

rm -rf $DEST
7z x tools/base.7z -o$DEST >/dev/null
cp -ipr en/* $DEST
unrar-nonfree x -xText -xvar $1 $DEST
7z x tools/gui.7z -o$DEST/dat >/dev/null

cd $DEST/dat
IFS_BAK=$IFS; IFS=.
for F in *.pcx *.bmp; do
	set $F
	[ -e $1.timestamp ] || touch -r $1.dat $1.timestamp
	echo dat $1.dat $2 ORIG="$F"
	dat $1.dat $2 ORIG="$F" >/dev/null
done

for F in *.timestamp; do
	set $F
	dat -u $1.dat | grep ^Updating
	touch -r $1.timestamp $1.dat
	rm "$F"
done
rm *.pcx *.bmp
IFS=$IFS_BAK

cd $DEST/..
zip -rv $VERSION.zip $VERSION
