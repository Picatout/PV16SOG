#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=FAT/byteordering.c FAT/fat.c FAT/partition.c FAT/sd_raw.c pv16sog.c graphics.c Hardware/hardware.c Hardware/TVout.S Hardware/systicks.S text.c Hardware/spi_ram.c Hardware/ps2_kbd.c Hardware/QWERTY.c filesys.c Hardware/spi.c shell.c errors.c font6x8.c reader.c editor.c stackvm.S vm_graphics.S Hardware/tones.S

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/FAT/byteordering.o ${OBJECTDIR}/FAT/fat.o ${OBJECTDIR}/FAT/partition.o ${OBJECTDIR}/FAT/sd_raw.o ${OBJECTDIR}/pv16sog.o ${OBJECTDIR}/graphics.o ${OBJECTDIR}/Hardware/hardware.o ${OBJECTDIR}/Hardware/TVout.o ${OBJECTDIR}/Hardware/systicks.o ${OBJECTDIR}/text.o ${OBJECTDIR}/Hardware/spi_ram.o ${OBJECTDIR}/Hardware/ps2_kbd.o ${OBJECTDIR}/Hardware/QWERTY.o ${OBJECTDIR}/filesys.o ${OBJECTDIR}/Hardware/spi.o ${OBJECTDIR}/shell.o ${OBJECTDIR}/errors.o ${OBJECTDIR}/font6x8.o ${OBJECTDIR}/reader.o ${OBJECTDIR}/editor.o ${OBJECTDIR}/stackvm.o ${OBJECTDIR}/vm_graphics.o ${OBJECTDIR}/Hardware/tones.o
POSSIBLE_DEPFILES=${OBJECTDIR}/FAT/byteordering.o.d ${OBJECTDIR}/FAT/fat.o.d ${OBJECTDIR}/FAT/partition.o.d ${OBJECTDIR}/FAT/sd_raw.o.d ${OBJECTDIR}/pv16sog.o.d ${OBJECTDIR}/graphics.o.d ${OBJECTDIR}/Hardware/hardware.o.d ${OBJECTDIR}/Hardware/TVout.o.d ${OBJECTDIR}/Hardware/systicks.o.d ${OBJECTDIR}/text.o.d ${OBJECTDIR}/Hardware/spi_ram.o.d ${OBJECTDIR}/Hardware/ps2_kbd.o.d ${OBJECTDIR}/Hardware/QWERTY.o.d ${OBJECTDIR}/filesys.o.d ${OBJECTDIR}/Hardware/spi.o.d ${OBJECTDIR}/shell.o.d ${OBJECTDIR}/errors.o.d ${OBJECTDIR}/font6x8.o.d ${OBJECTDIR}/reader.o.d ${OBJECTDIR}/editor.o.d ${OBJECTDIR}/stackvm.o.d ${OBJECTDIR}/vm_graphics.o.d ${OBJECTDIR}/Hardware/tones.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/FAT/byteordering.o ${OBJECTDIR}/FAT/fat.o ${OBJECTDIR}/FAT/partition.o ${OBJECTDIR}/FAT/sd_raw.o ${OBJECTDIR}/pv16sog.o ${OBJECTDIR}/graphics.o ${OBJECTDIR}/Hardware/hardware.o ${OBJECTDIR}/Hardware/TVout.o ${OBJECTDIR}/Hardware/systicks.o ${OBJECTDIR}/text.o ${OBJECTDIR}/Hardware/spi_ram.o ${OBJECTDIR}/Hardware/ps2_kbd.o ${OBJECTDIR}/Hardware/QWERTY.o ${OBJECTDIR}/filesys.o ${OBJECTDIR}/Hardware/spi.o ${OBJECTDIR}/shell.o ${OBJECTDIR}/errors.o ${OBJECTDIR}/font6x8.o ${OBJECTDIR}/reader.o ${OBJECTDIR}/editor.o ${OBJECTDIR}/stackvm.o ${OBJECTDIR}/vm_graphics.o ${OBJECTDIR}/Hardware/tones.o

# Source Files
SOURCEFILES=FAT/byteordering.c FAT/fat.c FAT/partition.c FAT/sd_raw.c pv16sog.c graphics.c Hardware/hardware.c Hardware/TVout.S Hardware/systicks.S text.c Hardware/spi_ram.c Hardware/ps2_kbd.c Hardware/QWERTY.c filesys.c Hardware/spi.c shell.c errors.c font6x8.c reader.c editor.c stackvm.S vm_graphics.S Hardware/tones.S


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24EP512MC202
MP_LINKER_FILE_OPTION=,--script=p24EP512MC202.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/FAT/byteordering.o: FAT/byteordering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/byteordering.o.d 
	@${RM} ${OBJECTDIR}/FAT/byteordering.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/byteordering.c  -o ${OBJECTDIR}/FAT/byteordering.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/byteordering.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/byteordering.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/fat.o: FAT/fat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/fat.o.d 
	@${RM} ${OBJECTDIR}/FAT/fat.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/fat.c  -o ${OBJECTDIR}/FAT/fat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/fat.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/fat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/partition.o: FAT/partition.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/partition.o.d 
	@${RM} ${OBJECTDIR}/FAT/partition.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/partition.c  -o ${OBJECTDIR}/FAT/partition.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/partition.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/partition.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/sd_raw.o: FAT/sd_raw.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/sd_raw.o.d 
	@${RM} ${OBJECTDIR}/FAT/sd_raw.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/sd_raw.c  -o ${OBJECTDIR}/FAT/sd_raw.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/sd_raw.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/sd_raw.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/pv16sog.o: pv16sog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pv16sog.o.d 
	@${RM} ${OBJECTDIR}/pv16sog.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pv16sog.c  -o ${OBJECTDIR}/pv16sog.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/pv16sog.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/pv16sog.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/graphics.o: graphics.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/graphics.o.d 
	@${RM} ${OBJECTDIR}/graphics.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  graphics.c  -o ${OBJECTDIR}/graphics.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/graphics.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/graphics.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/hardware.o: Hardware/hardware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/hardware.o.d 
	@${RM} ${OBJECTDIR}/Hardware/hardware.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/hardware.c  -o ${OBJECTDIR}/Hardware/hardware.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/hardware.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/hardware.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/text.o: text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/text.o.d 
	@${RM} ${OBJECTDIR}/text.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  text.c  -o ${OBJECTDIR}/text.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/text.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/text.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/spi_ram.o: Hardware/spi_ram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/spi_ram.o.d 
	@${RM} ${OBJECTDIR}/Hardware/spi_ram.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/spi_ram.c  -o ${OBJECTDIR}/Hardware/spi_ram.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/spi_ram.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/spi_ram.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/ps2_kbd.o: Hardware/ps2_kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/ps2_kbd.o.d 
	@${RM} ${OBJECTDIR}/Hardware/ps2_kbd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/ps2_kbd.c  -o ${OBJECTDIR}/Hardware/ps2_kbd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/ps2_kbd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/ps2_kbd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/QWERTY.o: Hardware/QWERTY.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/QWERTY.o.d 
	@${RM} ${OBJECTDIR}/Hardware/QWERTY.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/QWERTY.c  -o ${OBJECTDIR}/Hardware/QWERTY.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/QWERTY.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/QWERTY.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/filesys.o: filesys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/filesys.o.d 
	@${RM} ${OBJECTDIR}/filesys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  filesys.c  -o ${OBJECTDIR}/filesys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/filesys.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/filesys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/spi.o: Hardware/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/spi.o.d 
	@${RM} ${OBJECTDIR}/Hardware/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/spi.c  -o ${OBJECTDIR}/Hardware/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/spi.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/shell.o: shell.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/shell.o.d 
	@${RM} ${OBJECTDIR}/shell.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  shell.c  -o ${OBJECTDIR}/shell.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/shell.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/shell.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/errors.o: errors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/errors.o.d 
	@${RM} ${OBJECTDIR}/errors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  errors.c  -o ${OBJECTDIR}/errors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/errors.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/errors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/font6x8.o: font6x8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/font6x8.o.d 
	@${RM} ${OBJECTDIR}/font6x8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  font6x8.c  -o ${OBJECTDIR}/font6x8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/font6x8.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/font6x8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/reader.o: reader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/reader.o.d 
	@${RM} ${OBJECTDIR}/reader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  reader.c  -o ${OBJECTDIR}/reader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/reader.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/reader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/editor.o: editor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/editor.o.d 
	@${RM} ${OBJECTDIR}/editor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  editor.c  -o ${OBJECTDIR}/editor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/editor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/editor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/FAT/byteordering.o: FAT/byteordering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/byteordering.o.d 
	@${RM} ${OBJECTDIR}/FAT/byteordering.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/byteordering.c  -o ${OBJECTDIR}/FAT/byteordering.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/byteordering.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/byteordering.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/fat.o: FAT/fat.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/fat.o.d 
	@${RM} ${OBJECTDIR}/FAT/fat.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/fat.c  -o ${OBJECTDIR}/FAT/fat.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/fat.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/fat.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/partition.o: FAT/partition.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/partition.o.d 
	@${RM} ${OBJECTDIR}/FAT/partition.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/partition.c  -o ${OBJECTDIR}/FAT/partition.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/partition.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/partition.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/FAT/sd_raw.o: FAT/sd_raw.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/FAT" 
	@${RM} ${OBJECTDIR}/FAT/sd_raw.o.d 
	@${RM} ${OBJECTDIR}/FAT/sd_raw.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  FAT/sd_raw.c  -o ${OBJECTDIR}/FAT/sd_raw.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/FAT/sd_raw.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/FAT/sd_raw.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/pv16sog.o: pv16sog.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pv16sog.o.d 
	@${RM} ${OBJECTDIR}/pv16sog.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  pv16sog.c  -o ${OBJECTDIR}/pv16sog.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/pv16sog.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/pv16sog.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/graphics.o: graphics.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/graphics.o.d 
	@${RM} ${OBJECTDIR}/graphics.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  graphics.c  -o ${OBJECTDIR}/graphics.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/graphics.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/graphics.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/hardware.o: Hardware/hardware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/hardware.o.d 
	@${RM} ${OBJECTDIR}/Hardware/hardware.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/hardware.c  -o ${OBJECTDIR}/Hardware/hardware.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/hardware.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/hardware.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/text.o: text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/text.o.d 
	@${RM} ${OBJECTDIR}/text.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  text.c  -o ${OBJECTDIR}/text.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/text.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/text.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/spi_ram.o: Hardware/spi_ram.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/spi_ram.o.d 
	@${RM} ${OBJECTDIR}/Hardware/spi_ram.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/spi_ram.c  -o ${OBJECTDIR}/Hardware/spi_ram.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/spi_ram.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/spi_ram.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/ps2_kbd.o: Hardware/ps2_kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/ps2_kbd.o.d 
	@${RM} ${OBJECTDIR}/Hardware/ps2_kbd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/ps2_kbd.c  -o ${OBJECTDIR}/Hardware/ps2_kbd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/ps2_kbd.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/ps2_kbd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/QWERTY.o: Hardware/QWERTY.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/QWERTY.o.d 
	@${RM} ${OBJECTDIR}/Hardware/QWERTY.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/QWERTY.c  -o ${OBJECTDIR}/Hardware/QWERTY.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/QWERTY.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/QWERTY.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/filesys.o: filesys.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/filesys.o.d 
	@${RM} ${OBJECTDIR}/filesys.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  filesys.c  -o ${OBJECTDIR}/filesys.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/filesys.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/filesys.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/Hardware/spi.o: Hardware/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/spi.o.d 
	@${RM} ${OBJECTDIR}/Hardware/spi.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  Hardware/spi.c  -o ${OBJECTDIR}/Hardware/spi.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/spi.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/Hardware/spi.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/shell.o: shell.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/shell.o.d 
	@${RM} ${OBJECTDIR}/shell.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  shell.c  -o ${OBJECTDIR}/shell.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/shell.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/shell.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/errors.o: errors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/errors.o.d 
	@${RM} ${OBJECTDIR}/errors.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  errors.c  -o ${OBJECTDIR}/errors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/errors.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/errors.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/font6x8.o: font6x8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/font6x8.o.d 
	@${RM} ${OBJECTDIR}/font6x8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  font6x8.c  -o ${OBJECTDIR}/font6x8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/font6x8.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/font6x8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/reader.o: reader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/reader.o.d 
	@${RM} ${OBJECTDIR}/reader.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  reader.c  -o ${OBJECTDIR}/reader.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/reader.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/reader.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/editor.o: editor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/editor.o.d 
	@${RM} ${OBJECTDIR}/editor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  editor.c  -o ${OBJECTDIR}/editor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/editor.o.d"      -mno-eds-warn  -g -omf=elf -no-legacy-libc  -mconst-in-data -O1 -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/editor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/Hardware/TVout.o: Hardware/TVout.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/TVout.o.d 
	@${RM} ${OBJECTDIR}/Hardware/TVout.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/TVout.S  -o ${OBJECTDIR}/Hardware/TVout.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/TVout.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/TVout.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/TVout.o.d" "${OBJECTDIR}/Hardware/TVout.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Hardware/systicks.o: Hardware/systicks.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/systicks.o.d 
	@${RM} ${OBJECTDIR}/Hardware/systicks.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/systicks.S  -o ${OBJECTDIR}/Hardware/systicks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/systicks.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/systicks.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/systicks.o.d" "${OBJECTDIR}/Hardware/systicks.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/stackvm.o: stackvm.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/stackvm.o.d 
	@${RM} ${OBJECTDIR}/stackvm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  stackvm.S  -o ${OBJECTDIR}/stackvm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/stackvm.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/stackvm.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/stackvm.o.d" "${OBJECTDIR}/stackvm.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/vm_graphics.o: vm_graphics.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/vm_graphics.o.d 
	@${RM} ${OBJECTDIR}/vm_graphics.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  vm_graphics.S  -o ${OBJECTDIR}/vm_graphics.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/vm_graphics.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/vm_graphics.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/vm_graphics.o.d" "${OBJECTDIR}/vm_graphics.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Hardware/tones.o: Hardware/tones.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/tones.o.d 
	@${RM} ${OBJECTDIR}/Hardware/tones.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/tones.S  -o ${OBJECTDIR}/Hardware/tones.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/tones.o.d"  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/tones.o.asm.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/tones.o.d" "${OBJECTDIR}/Hardware/tones.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/Hardware/TVout.o: Hardware/TVout.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/TVout.o.d 
	@${RM} ${OBJECTDIR}/Hardware/TVout.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/TVout.S  -o ${OBJECTDIR}/Hardware/TVout.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/TVout.o.d"  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/TVout.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/TVout.o.d" "${OBJECTDIR}/Hardware/TVout.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Hardware/systicks.o: Hardware/systicks.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/systicks.o.d 
	@${RM} ${OBJECTDIR}/Hardware/systicks.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/systicks.S  -o ${OBJECTDIR}/Hardware/systicks.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/systicks.o.d"  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/systicks.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/systicks.o.d" "${OBJECTDIR}/Hardware/systicks.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/stackvm.o: stackvm.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/stackvm.o.d 
	@${RM} ${OBJECTDIR}/stackvm.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  stackvm.S  -o ${OBJECTDIR}/stackvm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/stackvm.o.d"  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/stackvm.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/stackvm.o.d" "${OBJECTDIR}/stackvm.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/vm_graphics.o: vm_graphics.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/vm_graphics.o.d 
	@${RM} ${OBJECTDIR}/vm_graphics.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  vm_graphics.S  -o ${OBJECTDIR}/vm_graphics.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/vm_graphics.o.d"  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/vm_graphics.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/vm_graphics.o.d" "${OBJECTDIR}/vm_graphics.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/Hardware/tones.o: Hardware/tones.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/Hardware" 
	@${RM} ${OBJECTDIR}/Hardware/tones.o.d 
	@${RM} ${OBJECTDIR}/Hardware/tones.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  Hardware/tones.S  -o ${OBJECTDIR}/Hardware/tones.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Hardware/tones.o.d"  -omf=elf -no-legacy-libc  -Wa,-MD,"${OBJECTDIR}/Hardware/tones.o.asm.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/Hardware/tones.o.d" "${OBJECTDIR}/Hardware/tones.o.asm.d"  -t $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -no-legacy-libc   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--heap=0,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -no-legacy-libc  -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=0,--stack=1024,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/pv16sog.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
