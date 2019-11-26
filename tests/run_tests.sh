#!/bin/bash

MEMCHECK=0
if [ "${2}" == "TRUE" ]; then
MEMCHECK=1
fi

FAIL=0
for target in ${1}; do
    echo "${target}"
    if [ ${MEMCHECK} -eq 1 ]; then
        valgrind --error-exitcode=128 --leak-check=full --quiet ./${target}
    else
        ./${target}
    fi
    ret=${?}
    if [ ${ret} -ne 0 ]; then
        FAIL=1
        echo -e "\x1b[31mFAILED\x1b[39m"
    else
        echo -e "\x1b[32mPASSED\x1b[39m"
    fi
done

exit ${FAIL}
