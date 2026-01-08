#!/bin/bash

rm -rf libdata_sender.so libdata_processor.so libdata_view.so main

export LD_LIBRARY_PATH="$(pwd)"

gcc -fPIC -shared data_sender.c -o libdata_sender.so -Wl,--allow-shlib-undefined

gcc -fPIC -shared data_processor.c -o libdata_processor.so -Wl,--allow-shlib-undefined

gcc -fPIC -shared data_view.c -o libdata_view.so -Wl,--allow-shlib-undefined

gcc ./cjson/cJSON.c main.c -o main -Wl,--allow-shlib-undefined

./main