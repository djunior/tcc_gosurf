#!/bin/bash

rm -rf deploy/

cd build

rm -rf deploy

mkdir deploy

make

mv deploy ../
