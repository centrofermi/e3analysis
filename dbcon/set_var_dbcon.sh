#!/usr/local/bin bash

export DBCON=$PWD

#=======================================
#CORRECTLY SET PATH AND LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./:${LD_LIBRARY_PATH}