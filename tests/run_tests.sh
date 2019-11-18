#!/bin/bash

FAIL=0

for target in ${1}; do
    echo "${target}"
    valgrind --error-exitcode=128 --leak-check=full --quiet ./${target}
    ret=${?}
    if [ ${ret} -ne 0 ]; then
        FAIL=1
        echo -e "\e[91mFAILED\e[39m"
    else
        echo -e "\e[32mPASSED\e[39m"
    fi
done

exit ${FAIL}
