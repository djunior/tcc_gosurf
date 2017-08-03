#!/bin/bash

INPUT=$1
FILE=`basename $INPUT`
OUTPUT_DIR=output_images/analysed_waves/${FILE%.*}

echo INPUT: $INPUT
echo FILE: $FILE
echo OUTPUT_DIR: $OUTPUT_DIR

mkdir $OUTPUT_DIR

echo Starting pre processor

./deploy/simpleTimestack $INPUT

mv output_images/timestack.jpg $OUTPUT_DIR

echo Starting image filter

mkdir output_images/process
DEBUG=n ./deploy/filterImage $OUTPUT_DIR/timestack.jpg

mv output_images/process $OUTPUT_DIR

echo Starting wave viewer

mkdir output_images/wave_viewer
DEBUG=n ./deploy/waveViewer $INPUT

mv waves.txt $OUTPUT_DIR

mv output_images/wave_viewer $OUTPUT_DIR

echo Starting wave inspector

mkdir output_images/wave_inspector
./deploy/waveInspector $INPUT

mv output_images/wave_inspector $OUTPUT_DIR
mv manual_waves.txt $OUTPUT_DIR

