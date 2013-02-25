
rc=0
for i in {0..47};
do
if [ $rc == 0 ]
    then python2 hid-monitor.py $i; 
    rc=$?;
fi; 
done;
