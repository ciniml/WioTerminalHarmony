.PHONY: all

MHC_ROOT ?= $(HOME)/Harmony3/mhc

all: config/default/initialization.c

config/default/initialization.c: config/default/default.xml config/default/harmony.prj
	cd $(MHC_ROOT) && java -jar mhc.jar -fw=../ -mode=gen -c=$(abspath config/default/harmony.prj) -loadstate=default.xml -log=ALL
