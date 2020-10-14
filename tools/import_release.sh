set -e ### Exit immediately if a command fails
LC_CTYPE=ru_RU.cp1251

if [ -z "$1" ]; then echo "Usage: $0 ARCHIVE"; exit; fi

TMPDIR=$(mktemp -d)
unrar-nonfree x -idq -x'var/yuplay.var' $1 Text var $TMPDIR
for F in $(find $TMPDIR -type f); do
	### Change CRLF to LF, append final LF
	sed -e 's,\r$,,' -e '$a\' $F > foo
	### Preserve timestamps
	touch -r $F foo && mv foo $F
done
rsync -a --info=NAME $TMPDIR/* ru
rm -r $TMPDIR
