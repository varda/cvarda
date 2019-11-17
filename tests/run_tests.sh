#!/bin/bash

FAIL=0

for target in ${1}; do
    valgrind --error-exitcode=128 --leak-check=full ./${target}
    ret=${?}
    if [ ${ret} -ne 0 ]; then
        FAIL=1
    fi
done

exit ${FAIL}