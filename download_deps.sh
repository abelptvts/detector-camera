#! /bin/bash

START=$(date +%s.%N)

mkdir deps
if [ ! -d "./deps/tensorflow-2.0.0/" ]; then
  echo 'Downloading tensorflow source...'
  wget https://github.com/tensorflow/tensorflow/archive/v2.0.0.zip -P ./deps/ >/dev/null
  unzip deps/v2.0.0.zip -d ./deps/ >/dev/null
  rm deps/v2.0.0.zip
  echo 'Downloading tensorflow dependencies...'
  ./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/download_dependencies.sh >/dev/null
  echo 'Builing tensorflow for current CPU arch'
  ./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/build_lib.sh >/dev/null
  echo 'Builing tensorflow for ARMHF'
  ./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/build_rpi_lib.sh >/dev/null
fi

echo 'Downloading boost for socket io'
cd ./deps/
if [ ! -d "./boost_1_69_0" ]; then
  wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz >/dev/null
  tar -xvf boost_1_69_0.tar.gz
  rm boost_1_69_0.tar.gz
else
  rm -rf boost-1.69.0
fi
cd boost_1_69_0/
./bootstrap.sh
if [ $DOCKER = 1 ]; then
  echo 'using gcc : arm : arm-linux-gnueabihf-g++ ;' >> project-config.jam
  ./b2 install toolset=gcc-arm --prefix="../boost-1.69.0" --with-system --with-date_time --with-random link=static runtime-link=shared threading=multi
else
  ./b2 install --prefix="../boost-1.69.0" --with-system --with-date_time --with-random link=static runtime-link=shared threading=multi
fi
cd ../

echo 'Downloading socket io'
if [ ! -d "./socket.io-client-cpp/" ]; then
#  git clone --recurse-submodules https://github.com/socketio/socket.io-client-cpp.git
  git clone --recurse-submodules https://github.com/ErisExchange/socket.io-client-cpp.git
fi
cd socket.io-client-cpp
git checkout eris
git pull
cd ./lib/websocketpp/
git checkout master
git pull
cd ../../
if [ ! -d "./cmake/" ]; then
  mkdir cmake
else
  rm -rf ./cmake/*
fi
cd cmake
if [ $DOCKER = 1 ]; then
  cmake -DCMAKE_CXX_COMPILER=/usr/bin/arm-linux-gnueabihf-g++ -DCMAKE_C_COMPILER=/usr/bin/arm-linux-gnueabihf-gcc -DBOOST_ROOT:STRING=../boost-1.69.0 -DBOOST_VER:STRING=1.69.0 -DOPENSSL_ROOT_DIR=/usr/lib/arm-linux-gnueabihf ../
else
  cmake -DBOOST_ROOT:STRING=../boost-1.69.0 -DBOOST_VER:STRING=1.69.0 ../
fi
make install -j 4

END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)

echo $DIFF
