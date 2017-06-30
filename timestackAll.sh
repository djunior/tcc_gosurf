#!/bin/bash

ORIGIN_DIR="$1"
FOLDER_NAME="${ORIGIN_DIR#*/}"
OUTPUT_DIR="output_images/timestack/$FOLDER_NAME"

echo "ORIGIN DIR: $ORIGIN_DIR"
echo "OUTPUT DIR: $OUTPUT_DIR"

rm -rf $OUTPUT_DIR

mkdir $OUTPUT_DIR

mkdir $OUTPUT_DIR/timestack0
mkdir $OUTPUT_DIR/timestack25
mkdir $OUTPUT_DIR/timestack50
mkdir $OUTPUT_DIR/timestack75
mkdir $OUTPUT_DIR/timestack100

for f in `ls $ORIGIN_DIR | grep m4v`
do
	echo Filtering image $f
	./deploy/timestack $ORIGIN_DIR/$f $OUTPUT_DIR
done
