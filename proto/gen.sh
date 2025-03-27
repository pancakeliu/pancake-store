#!/bin/bash

PROTOC=/usr/bin/protoc

if [ "$1" = "clean" ];then
	rm -f *.pb.cc *.pb.h
else
    ${PROTOC} --cpp_out=. --proto_path=. -I .  *.proto
fi
