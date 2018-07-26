ENV := /opt/poky/2.2.3/environment-setup-aarch64-poky-linux
C_FILES := ./*.c

all: 9z2_simple 

9z2_simple: 9z2_simple.c
	export CXX=gcc; [ -f $(abspath $(ENV)) ] && source $(abspath $(ENV)); $$CXX -o $@ 9z2_simple.c

clean:
	-rm 9z2_simple
