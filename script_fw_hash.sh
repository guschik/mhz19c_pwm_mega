#!/bin/sh
FW_HASH=


#FW_HASH=$(git log --oneline | head -n 1 | awk {'print $1'})
#echo $FW_HASH
#echo "const char * sketchVersion = \"$(git describe --tags --always --dirty) $FW_HASH\";" > gitTagVersion.h
#echo "const char * sketchVersion = \"$(git describe --tags --always --dirty) $FW_HASH\";"

#~/arduino-1.8.5/arduino --verify test.ino
#rm gitTagVersion.h

FW_HASH=$(git log --oneline | head -n 1 | awk '{print $1}')
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
echo "const char * sketchVersion = \"$(git describe --tags --always --dirty) branch:$BRANCH_NAME commit:$FW_HASH\";" > gitTagVersion.h