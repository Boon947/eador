set -e ### Exit immediately if a command fails
LC_CTYPE=ru_RU.cp1251

TMPDIR=$(mktemp -d)
VERSION=$(grep -o 'New Horizons[^#]*' en/Text/message.txt | tr ' ' '_')

7z x tools/base.7z -o$TMPDIR >/dev/null
7z x tools/gui.7z -o$TMPDIR/dat >/dev/null
cp -ipr en/* $TMPDIR

case $1 in
*.zip) unzip $1 -x 'Text/*' 'var/*' -d $TMPDIR ;;
*.rar) unrar-nonfree x -xText -xvar $1 $TMPDIR ;;
*) echo "Usage: $0 ARCHIVE" ; rm -r $TMPDIR ; exit
esac

cd $TMPDIR/dat
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

cd /tmp
mv $TMPDIR $VERSION
zip -rv $VERSION.zip $VERSION
