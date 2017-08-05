#!/bin/bash

INPUT=$1
FILE=`basename $INPUT`
OUTPUT_DIR=output_images/analysed_waves/${FILE%.*}

echo INPUT: $INPUT
echo FILE: $FILE
echo OUTPUT_DIR: $OUTPUT_DIR

rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

echo Starting pre processor

./deploy/simpleTimestack $INPUT

mv output_images/timestack.jpg $OUTPUT_DIR

echo Starting image filter

mkdir -p output_images/process
DEBUG=n ./deploy/filterImage $OUTPUT_DIR/timestack.jpg

mv output_images/process $OUTPUT_DIR

echo Starting wave viewer

mkdir -p output_images/wave_viewer
DEBUG=n ./deploy/waveViewer $INPUT

mv output_images/wave_viewer $OUTPUT_DIR

echo Starting wave inspector
echo Press a key when ready

read

mkdir -p output_images/wave_inspector
./deploy/waveInspector $INPUT

./deploy/generateReport manual_waves.txt

mv output_images/wave_inspector $OUTPUT_DIR
mv manual_waves.txt $OUTPUT_DIR
mv report.txt $OUTPUT_DIR/manual_report.txt


./deploy/generateReport waves.txt

mv waves.txt $OUTPUT_DIR
mv report.txt $OUTPUT_DIR

