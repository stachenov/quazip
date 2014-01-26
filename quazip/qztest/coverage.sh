#!/bin/sh
lcov --capture -b ../quazip -d ../quazip/.obj --output-file cov.info
genhtml cov.info --output-directory cov
