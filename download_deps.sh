#! /bin/bash

echo 'Downloading tensorflow source...'
wget https://github.com/tensorflow/tensorflow/archive/v2.0.0.zip -P ./deps/ > /dev/null
unzip deps/v2.0.0.zip -d ./deps/ > /dev/null
rm deps/v2.0.0.zip
echo 'Downloading tensorflow dependencies...'
./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/download_dependencies.sh > /dev/null
echo 'Builing tensorflow for current CPU arch'
./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/build_lib.sh > /dev/null
echo 'Builing tensorflow for ARMHF'
./deps/tensorflow-2.0.0/tensorflow/lite/tools/make/build_rpi_lib.sh > /dev/null