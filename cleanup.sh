pwd=$(pwd)
cd $ARCHIVEVAR
make clean
rm -f logs/*.*
touch logs/errors.log
touch logs/info.log
rm -f description_all.txt
cd $pwd