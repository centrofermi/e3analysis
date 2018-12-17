export TIMING_DIR=$PWD/jplephem/

echo $PATH

if [ -z "${PATH}" ]; then
   echo "Setting new PATH ..."
   export PATH=/opt/rh/python27/root/usr/bin/
else
   export PATH=/opt/rh/python27/root/usr/bin/:$PATH; export PATH
fi

echo $PATH

if [ -z "${LD_LIBRARY_PATH}" ]; then
   export LD_LIBRARY_PATH=/home/mazziot/ExtSoft/CLHEP_2.2.0.5/CLHEP/lib:/opt/rh/python27/root/usr/lib64/:/opt/rh/python27/root/usr/lib64/python2.7/:/opt/rh/python27/root/usr/lib/python2.7/
else
   export LD_LIBRARY_PATH=/home/mazziot/ExtSoft/CLHEP_2.2.0.5/CLHEP/lib:/opt/rh/python27/root/usr/lib64/:/opt/rh/python27/root/usr/lib64/python2.7/:/opt/rh/python27/root/usr/lib/python2.7/:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

if [ -z "${PYTHONPATH}" ]; then
   export PYTHONPATH=/lib:/opt/rh/python27/root/usr/lib64/:/opt/rh/python27/root/usr/lib64/python2.7/:/opt/rh/python27/root/usr/lib/python2.7/
else
   export PYTHONPATH=/opt/rh/python27/root/usr/lib64/:/opt/rh/python27/root/usr/lib64/python2.7/:/opt/rh/python27/root/usr/lib/python2.7/:$PYTHONPATH
fi



echo "$LD_LIBRARY_PATH " $LD_LIBRARY_PATH
echo "$PYTHONPATH " $PYTHONPATH
