#!/bin/bash
FILE="./supercrack.exe"
if [[ -f "$FILE" ]]; then
    echo "we found supercrack.exe binary so use that" #wsl
else
    FILE="./supercrack"
    if [[ -f "$FILE" ]]; then
        echo "we found ./supercrack binary so use that" #linux
    else
        echo "could not find supercrack binary, compile it first using make"
        exit
    fi
fi

$FILE -a0 -m17040 tools/gpg_tests/refdata/m17040/ref1-cast5.hash tools/gpg_tests/refdata/m17040/ref1-cast5.txt --potfile-disable
$FILE -a0 -m17040 tools/gpg_tests/refdata/m17040/ref2-cast5.hash tools/gpg_tests/refdata/m17040/ref2-cast5.txt --potfile-disable
