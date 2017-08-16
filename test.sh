#!/usr/bin/bash

rm ./watch/*
i=0

echo "File count:"


printf '%s ' {1..1391803} | xargs touch
