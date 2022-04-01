#!/bin/bash

# Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0


#=====================================================
# usage:
#   ./list_stl_dependencies.sh
#=====================================================

SCOPE=${1:-list}
COMPONENTS=(iceoryx_hoofs iceoryx_posh)
SOURCE_DIR=(source include)
WORKSPACE=$(git rev-parse --show-toplevel)
QNX_PLATFORM_DIR=$WORKSPACE/iceoryx_hoofs/platform/qnx/
ALLOWLIST=$WORKSPACE/tools/scripts/header-allowlist.txt
TEMP_FILE=$(mktemp)
GET_HEADER_NAME="\<\K[^<>]+(?=>)" # Matches the content between angle brackets

for COMPONENT in ${COMPONENTS[@]}; do
    for DIR in ${SOURCE_DIR[@]}; do
        GREP_PATH="${GREP_PATH} ${WORKSPACE}/${COMPONENT}/$DIR"
    done
done

echo "# QNX platform/ libc headers" >> $TEMP_FILE

echo "# usage of headers for QNX"
gcc -w -fpreprocessed -dD -E $(find $QNX_PLATFORM_DIR -type f -iname *.cpp -o -iname *.hpp) \
 | grep -e "#include <" \
 | grep -oP $GET_HEADER_NAME \
 | sort \
 | uniq \
 | tee -a $TEMP_FILE \
 | cat


echo "# iceoryx_posh / iceoryx_hoofs headers" >> $TEMP_FILE
# GCC can't preprocess .inl so we grep them in plain text
echo
echo "# usage of <..> header includes"
{ gcc -w -fpreprocessed -dD -E $(find $GREP_PATH -type f -iname *.cpp -o -iname *.hpp); cat $(find $GREP_PATH -type f -iname *.inl); } \
 | grep -e "#include <" \
 | grep -oP $GET_HEADER_NAME \
 | sort \
 | uniq \
 | tee -a $TEMP_FILE \
 | cat

if [[ "$SCOPE" == "check" ]]; then
    echo
    echo "Comparing system headers against allowed ones.."
    diff $TEMP_FILE $ALLOWLIST
    if [ $? -eq 1 ]; then
        echo "One or more header is not on the allow list, please remove the header usage or extend the allow list!"
        exit 1
    fi
    echo "No header divergence found!"
fi

echo
echo "# usage of std components"
{ gcc -w -fpreprocessed -dD -E $(find $GREP_PATH -type f -iname *.cpp -o -iname *.hpp); cat $(find $GREP_PATH -type f -iname *.inl); } \
 | grep -HIne "std::" \
 | sed -n  "s/\([^:]*\:[0-9]*\)\:.*\(std::[a-zA-Z_]*\).*/\ \  \2/p" \
 | sort \
 | uniq

echo
echo "# files with stl dependency"
grep -RIne "std::" $GREP_PATH | sed -n  "s/\([^:]*\)\:[0-9]*\:.*std::[a-zA-Z_]*.*/\1/p" | xargs -I{} basename {} | sort | uniq

echo
echo "# using namespace with std component"
grep -RIne ".*using[ ]*namespace[ ]*std" $GREP_PATH | sed -n "s/\(.*\)/\ \ \1/p"

echo
echo "# usage of std components"
grep -RIne "std::" $GREP_PATH | sed -n  "s/.*\(std::[a-zA-Z_]*\).*/\ \ \1/p" | sort | uniq

exit 0
