#!/bin/bash

ORIGIN_DIR="$1"
OUTPUT_DIR="output_images/filtered/"
#OUTPUT_DIR=$ORIGIN_DIR

rm -rf $OUTPUT_DIR

mkdir $OUTPUT_DIR

for f in `ls $ORIGIN_DIR | grep stable`
do
	echo Filtering image $f
	FILE_PREFIX=${f%_*}
	mkdir $OUTPUT_DIR/$FILE_PREFIX
	./deploy/filterImage $ORIGIN_DIR/$f $OUTPUT_DIR/$FILE_PREFIX/
	cp -rf $ORIGIN_DIR/$f $OUTPUT_DIR/$FILE_PREFIX/
done
