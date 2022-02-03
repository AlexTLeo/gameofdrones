pwd=$(pwd)
if [ -z "$1" ]; then 
   ARCHIVEVAR=~/archive_drones
else 
   ARCHIVEVAR=$1
fi

unzip -u -d /tmp/archive_drones archive_drones.zip
rm -rf $ARCHIVEVAR
mkdir $ARCHIVEVAR
mv  -v /tmp/archive_drones/archive_drones/* $ARCHIVEVAR
cd $ARCHIVEVAR
make src/master
make src/drone1
cd $pwd