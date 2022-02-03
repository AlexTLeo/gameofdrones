pwd=$(pwd)
cd $ARCHIVEVAR/bin
gnome-terminal -- sh -c "./master; bash"
gnome-terminal -- sh -c "./drone1 0;"
gnome-terminal -- sh -c "./drone1 1;"
gnome-terminal -- sh -c "./drone1 2;"
gnome-terminal -- sh -c "./drone1 3;"
gnome-terminal -- sh -c "./drone1 4;"
cd $pwd