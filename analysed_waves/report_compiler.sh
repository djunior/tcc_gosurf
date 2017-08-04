#!/bin/bash

ALG_SUM=`cat */report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`
MAN_SUM=`cat */manual_report.txt | awk 'BEGIN{sum=0}/Waves/{sum+=$3}END{print sum}'`

PERCENT=$((100 - (ALG_SUM - MAN_SUM)*100 / MAN_SUM ))

echo Percentual total de ondas encontradas: $PERCENT

for f in `ls . | grep IMG`
do
	echo "Analisando video $f"
	awk -f report_compiler.awk $f/manual_report.txt $f/report.txt
done

