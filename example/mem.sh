
# !/bin/bash

(make clean; make) >/dev/null

all="misc scalar vec table struct section nsection vec-section user-mat"

for i in $all; do
    # run
    echo "***** testing program: " $i
    info=$(valgrind ./$i 2>&1 | grep "total heap usage")

    # output info
    echo $info

    #stat
    total=$(echo $info | gawk '{print $5}')
    echo "total alloc: " $total

    #stat
    rel=$(echo $info | gawk '{print $7}')
    echo "total freed: " $rel

    if ((rel < total)); then
        echo "memory leak found in \"$i\""
    else
        echo "no memory leak found in \"$i\""
    fi

    echo

done
