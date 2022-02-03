#!/bin/sh 
pwd=$(pwd)
more README_drone1.txt

cd $ARCHIVEVAR

cat descriptions/description_master.txt descriptions/description_drone1.txt > description_all.txt
more description_all.txt
rm -f description_all.txt
cd $pwd