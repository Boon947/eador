set -e ### Exit immediately if a command fails
LC_CTYPE=ru_RU.cp1251

TMPDIR=$(mktemp -d)

case $1 in
*.zip) unzip $1 'Text/*' 'var/*' -d $TMPDIR ;;
*.rar) unrar-nonfree x $1 'Text' 'var' $TMPDIR ;;
*) echo "Usage: $0 ARCHIVE" ; rmdir $TMPDIR ; exit
esac

rm $TMPDIR/var/yuplay.var

for F in $(find $TMPDIR -type f); do
	### Change CRLF to LF, append final LF
	sed -e 's,\r$,,' -e '$a\' $F > foo
	### Preserve timestamps
	touch -r $F foo && mv foo $F
done
rsync -a --info=NAME $TMPDIR/* ru
rm -r $TMPDIR
