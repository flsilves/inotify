#!/usr/bin/bash

NUMBER_OF_FILES=500000
SCRIPT=$(realpath $0)
SCRIPT_PATH=$(dirname ${SCRIPT})
WATCH_FOLDER=${SCRIPT_PATH}/watch

echo "creating ${NUMBER_OF_FILES} test files on ${WATCH_FOLDER}"


rm ${WATCH_FOLDER}/*

#dd if=/dev/zero of=${WATCH_FOLDER}/masterfile bs=1 count=${NUMBER_OF_FILES}
#split -b 1 ${WATCH_FOLDER}/masterfile ${WATCH_FOLDER}/split_

echo "unpacking"
tar -zxf ${SCRIPT_PATH}/test.tar.gz -C ${WATCH_FOLDER}
echo "done"

#rm ${WATCH_FOLDER}/masterfile
