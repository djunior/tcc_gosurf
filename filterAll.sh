#!/bin/bash

ORIGIN_DIR="$1"
OUTPUT_DIR="output_images/filtered/"
#OUTPUT_DIR=$ORIGIN_DIR

rm -rf $OUTPUT_DIR

mkdir $OUTPUT_DIR

for f in `ls $ORIGIN_DIR`
do
	echo Filtering image $f
	FILE_PREFIX=${f%.*}
	mkdir $OUTPUT_DIR/$FILE_PREFIX
	DEBUG=n ./deploy/filterImage $ORIGIN_DIR/$f
	mv output_images/process/* $OUTPUT_DIR/$FILE_PREFIX/
	mv waves.txt $OUTPUT_DIR/$FILE_PREFIX
done
