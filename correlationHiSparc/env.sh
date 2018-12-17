#scl enable devtoolset-6 python27 bash
export VGM_DIR=/home/eeesoft/vgm/install
source /home/eeesoft/geant4_10_03_p01/install/bin/geant4.sh
source /home/eeesoft/rootgit/install/bin/thisroot.sh
export COAST_DIR=/home/eeesoft/rootgit/coast-v4r5/installation
export LD_LIBRARY_PATH=/home/eeesoft/geant4_vmc/EEE_Analyzer/eeeroot/lib:/home/eeesoft/geant4_vmc/install/lib:$COAST_DIR/lib:$LD_LIBRARY_PATH
export PATH=/home/eeesoft/geant4_vmc/EEE_Analyzer/eeeroot/bin:/home/eeesoft/geant4_vmc/EEE_Analyzer/eeeroot/:$PATH
export CPLUS_INCLUDE_PATH=/home/eeesoft/geant4_vmc/EEE_Analyzer/eeeroot/
