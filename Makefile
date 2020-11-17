cc = gcc
target =  build/main.out

source = src
include = include

main_source = ${source}/*.c
generate_main_debug: 
	${cc} -O0 -g ${main_source} -o ${target} -lm -I ${include}


debug: generate_main_debug
.phony: debug


help:
	@echo "this is a make file"
.phony: help

