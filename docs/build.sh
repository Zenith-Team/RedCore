sudo apt-get update && sudo apt-get install -y graphviz
wget https://github.com/doxygen/doxygen/releases/download/Release_1_14_0/doxygen-1.14.0.linux.bin.tar.gz
tar -xzf doxygen-1.14.0.linux.bin.tar.gz
wget https://nsmbu.github.io/sead/sead.tag -O sead.tag
wget https://nsmbu.github.io/Sys/Sys.tag -O Sys.tag
wget https://nsmbu.github.io/Eft/Eft.tag -O Eft.tag
wget https://nsmbu.github.io/FFL/FFL.tag -O FFL.tag
wget https://nsmbu.github.io/G3d/G3d.tag -O G3d.tag
wget https://nsmbu.github.io/Lyt/Lyt.tag -O Lyt.tag
wget https://nsmbu.github.io/Snd/Snd.tag -O Snd.tag
wget https://nsmbu.github.io/headers/headers.tag -O headers.tag
./doxygen-1.14.0/bin/doxygen ./docs/Doxyfile
