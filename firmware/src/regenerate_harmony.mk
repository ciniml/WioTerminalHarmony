.PHONY: all

HARMONY_ROOT ?= $(HOME)/Harmony3
MHC_ROOT ?= $(HOME)/Harmony3/mhc
CMSIS_ROOT ?= $(HARMONY_ROOT)/dev_packs/arm/CMSIS/5.6.0/CMSIS

all: config/default/initialization.c packs/CMSIS/CMSIS/Core/Include

config/default/initialization.c: config/default/default.xml config/default/harmony.prj
	cd $(MHC_ROOT) && java -jar mhc.jar -fw=../ -mode=gen -c=$(abspath config/default/harmony.prj) -loadstate=default.xml -log=ALL

packs/CMSIS/CMSIS/Core/Include:
	mkdir -p packs/CMSIS/CMSIS/Core/Include
	cp $(CMSIS_ROOT)/Core/Include/cmsis_armcc.h	       packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_armclang_ltm.h packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_armclang.h     packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_compiler.h     packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_gcc.h          packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_iccarm.h       packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/cmsis_version.h      packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/core_cm4.h           packs/CMSIS/CMSIS/Core/Include/
	cp $(CMSIS_ROOT)/Core/Include/mpu_armv7.h          packs/CMSIS/CMSIS/Core/Include/
