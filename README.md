xpath
=====

Utility allows to test xpath against xml files

# Installation on Ubuntu
```
sudo apt-get install -y build-essential cmake libboost-filesystem-dev libboost-system-dev libboost-program-options-dev
git clone https://github.com/message/xpath.git
cd xpath
git submodule init &&  git submodule update
cmake .
make
mv xpath /usr/local/bin/
```
