#!/bin/bash
mkdir ~/venvs
cd ~/venvs/

# Nest and SpiNNaker installation
virtualenv --python=python3 --system-site-packages nest
source nest/bin/activate
pip install cython -U
cd nest
wget https://github.com/nest/nest-simulator/archive/v2.18.0.tar.gz
tar -xzvf v2.18.0.tar.gz
cd nest-simulator-2.18.0/ && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=~/venvs/nest/
make -j && make install
pip install pyNN==0.9.5 neo==0.6.1 lazyarray
deactivate

cd ~/venvs/
virtualenv --python=python3 --system-site-packages spinnaker
source spinnaker/bin/activate
pip install sPyNNaker8=='1!5.1.0'
python -m spynnaker8.setup_pynn
python -c "import pyNN.spiNNaker"
deactivate

# Spikey installation
:"
cd ~/venvs/
git clone https://github.com/costrau/spikey_demo spikey
cd spikey
python3 waf setup --project=deb-pynn@0.6 --repo-db-url=https://github.com/costrau/projects
python3 waf configure
python3 waf install --targets=*
"

