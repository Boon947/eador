set -e
LC_CTYPE=ru_RU.cp1251

sed -i 's,tuv lang="EN-US"[^>]*,tuv lang="EN-US",' translate/omegat/project_save.tmx

# Import Text
cp translate/target/Text/*.txt en/Text

# Clean up empty lines from deleted flavor text
for F in item.txt medal.txt Ritual.txt Spell.txt; do
  # Copy entire file to hold-space for multi-line regex
  sed -i -z 'H;1h;$!d;x;s,[\n ]*#\n,#\n,g' en/Text/$F
done

# Import var
GLOBIGNORE=campaign_dialog.var:dialog.var
VAR_FILES=$(cd ru/var && ls *.var)

for F in $VAR_FILES; do
  sed "/^Name:\|^Text:\|^Land:/ {
    R translate/target/var/$F.txt
    d
  }" ru/var/$F > en/var/$F
done

for F in campaign_dialog.var dialog.var; do
  sed "/Text:/,/#####/ {
    R translate/target/var/$F.txt
    d
  }" ru/var/$F > en/var/$F
done

SRC_FILES=$(cd ru && find -type f)
for F in $SRC_FILES; do touch -r ru/$F en/$F; done
