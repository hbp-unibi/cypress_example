# Single Neuron Introduction

## Installation and Usage

To set up the application, run the following from this folder:
```bash
mkdir build && cd build
cmake ..
make -j4
```

Afterwards you can execute the program by
```bash
./simple <platform>
```
Here, the <platform> can be substituted by all platforms currently supported by Cypress. This list contains SpiNNaker, Spikey, BrainScaleS (and its executable system specification) and nest/pynn.nest. __Note__ that the respective backend has to be installed on your system. 

Neuron Parameters and weights are defined in the config, see ```config/parameters.json```. This is currently hard-coded -- sorry for that.

To install a backend (nest) into a virtual environment, execute ```./virtualenvs.sh``` in the source directory. To activate the virtual environment with the nest installation, type ```source ~/venvs/nest/bin/activate```.


## Optional bash alias
On the long term, it is easier to define an alias in your ~/.bashrc. Here is my configuration (replace USER by your username): 
```bash
if [ -e /home/USER/venvs/nest/bin/activate ]; then
        alias nest="\
        (export \
                VIRTUAL_ENV_DISABLE_PROMPT=1\
                PS1='\[\e[0;1;38;5;106m\]\[\e[48;5;236m\] NEST \[\e[0m\] \w> '; \
         source /home/USER/venvs/nest/bin/activate; history -a; bash --norc --noprofile)"
fi
```
From now on you can enter the virtual environment by just typing ```nest``` and exit it again with ```exit```. 




