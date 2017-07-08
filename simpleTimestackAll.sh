#!/bin/bash

SOURCE_DIR=$1
OUTPUT_DIR=output_images/${SOURCE_DIR##*/}

for f in `ls $1`
do
	echo Processing $f
	./deploy/simpleTimestack $SOURCE_DIR/$f
	FILE_OUTPUT=${f%.*}.jpg
	echo Saving $FILE_OUTPUT
	mkdir $OUTPUT_DIR
	mv output_images/timestack.jpg $OUTPUT_DIR/$FILE_OUTPUT
done
