set -e
LC_CTYPE=ru_RU.cp1251
GLOBIGNORE="campaign_dialog.var:dialog.var:credits.var"

# Start from scratch
rm -rf translate/{source,target}
mkdir -pv translate/{source,target}/{Text,var}

# Export Text
cp ru/Text/*.txt translate/source/Text

# Export var
DEST=translate/source/var
VAR_NAMES=$(cd ru/var && ls *.var)

for F in $VAR_NAMES; do
  grep "^Name:\|^Text:\|^Land:" ru/var/$F > $DEST/$F.txt || rm $DEST/$F.txt
done

for F in campaign_dialog.var dialog.var; do
  sed "/Text:/,/#####/!d" ru/var/$F > $DEST/$F.txt
done
