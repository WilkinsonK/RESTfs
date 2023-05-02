#!/bin/bash

pushd dist &> /dev/null && make -j; popd &> /dev/null
