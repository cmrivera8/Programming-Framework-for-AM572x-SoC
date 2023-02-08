#!/bin/bash

start=2
end=2600
steps=200
previous_number=0

for ((i=0; i<=steps; i++))
do
    number=$(echo $start $end $steps $i | awk '{ print int((exp(log($1) + $4 * (log($2) - log($1)) / $3))); print number }')
    if [ $number -eq $previous_number ]; then
        continue
    fi
    previous_number=$number

    # echo $i
    # echo $number

    #Force ARM computation
    export TI_CBLAS_OFFLOAD=0
    ./cblas $number

    #Force DSP computation
    export TI_CBLAS_OFFLOAD=1
    ./cblas $number
done

#To send to target:
#scp -r -o HostKeyAlgorithms=ssh-rsa ./debug/test.sh root@${IP}:/home/root/cblas