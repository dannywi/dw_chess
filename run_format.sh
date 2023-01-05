#!/bin/bash
find . -name BUILD -exec buildifier {} \;
find . -name *.[hc]pp -exec clang-format -i {} \;
