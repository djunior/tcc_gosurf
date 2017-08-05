#!/bin/bash

ALG_SUM=`cat */report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`
MAN_SUM=`cat */manual_report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`

PERCENT=$((100 - (ALG_SUM - MAN_SUM)*100 / MAN_SUM ))

echo Numero de ondas identificadas manualmente: $MAN_SUM
echo Numero de ondas identificadas pelo algoritmo: $ALG_SUM
echo Percentual total de ondas encontradas: $PERCENT

echo $OSTYPE

for f in `ls . | grep IMG`
do
	echo "Analisando video $f"
	awk -f report_compiler.awk $f/manual_report.txt $f/report.txt | awk -f latexTableGenerator.awk > $f/report_table.tex
	if [[ "$OSTYPE" == "linux-gnu" ]]; then
		iconv -t ISO-8859-1 $f/report_table.tex -o $f/report_table.tex
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		echo "Mac: converting to ISO"
		iconv -t ISO_8859-1 $f/report_table.tex > report_table.tex
		mv report_table.tex $f/report_table.tex
	fi
done

