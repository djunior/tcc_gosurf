#!/bin/bash

abs () { echo -E "${1#-}" ;}

ALG_SUM=`cat wave_data/*/report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`
MAN_SUM=`cat wave_data/*/manual_report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`

DIFF=$((ALG_SUM-MAN_SUM))
ABS_DIFF=abs $DIFF
PERCENT=$((100 - ( ABS_DIFF*100 / MAN_SUM) ))

echo Numero de ondas identificadas manualmente: $MAN_SUM
echo Numero de ondas identificadas pelo algoritmo: $ALG_SUM
echo Percentual total de ondas encontradas: $PERCENT

echo $OSTYPE

for f in `ls wave_data/`
do
	echo "Analisando video wave_data/$f"
	awk -f report_compiler.awk wave_data/$f/manual_report.txt wave_data/$f/report.txt | awk -f latexTableGenerator.awk > wave_data/$f/report_table.tex
	if [[ "$OSTYPE" == "linux-gnu" ]]; then
		iconv -t ISO-8859-1 wave_data/$f/report_table.tex -o wave_data/$f/report_table.tex
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		echo "Mac: converting to ISO"
		iconv -t ISO_8859-1 wave_data/$f/report_table.tex > report_table.tex
		mv report_table.tex wave_data/$f/report_table.tex
	fi
done

