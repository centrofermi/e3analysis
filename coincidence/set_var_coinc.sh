#!/usr/local/bin bash

export COINC=$PWD

#=======================================
#CORRECTLY SET PATH AND LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${COINC}/lib:${LD_LIBRARY_PATH}