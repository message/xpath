xpath
=====

Utility allows to test xpath against xml files

# Installation on Ubuntu
```
sudo apt-get install build-essential cmake libboost-filesystem1.55-dev libboost-system1.55-dev libboost-program-options1.55-dev
git clone git@github.com:message/xpath.git
cd xpath
git submodule init &&  git submodule update
cmake .
make
mv xpath /usr/local/bin/
```