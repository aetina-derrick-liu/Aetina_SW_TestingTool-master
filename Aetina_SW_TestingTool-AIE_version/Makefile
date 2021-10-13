#---------------------------------------------------------------------------------------------
# Environment Variable
INCLUDE_DIRS = -Iinclude/
BINARY_DIRS = $(PWD)/bin
LIBRARY_DIRS = $(PWD)/bin/lib
LOGFILE := TestingTool_Report.txt
#---------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------
# Compile Variable
#CC = gcc
CC = /opt/linaro/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
AR = ar

CFLAGS := -fPIC -Wall -Wextra -O3 -g -static
#CFLAGS += -D_DEBUG_
LDFLAGS := -lpthread

libTestOfBoard_OBJS = libTestOfBoard.o spidev_test.o serialport_test.o dictionary.o iniparser.o \
											lm75_tmp100.o

libTestOfBoard_SRC = src/utils
libTestOfBoard_LIBRARIES = libTestOfBoard.a

TestingTool_EXE = Aetina
TestingTool_SRC = src/doTests.c src/common.c src/main.c
#TestingTool_CFLAGS += -D_DEBUG_
#---------------------------------------------------------------------------------------------

#---------------------------------------------------------------------------------------------
# Install Variable
INSTALL_DIRECTORY=..
ROOT_DIRECTORY=../../..
#---------------------------------------------------------------------------------------------

all : ${libTestOfBoard_LIBRARIES} ${TestingTool_EXE}

libTestOfBoard.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/libTestOfBoard.c -o $@

spidev_test.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/spidev_test.c -o $@

serialport_test.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/serialport_test.c -o $@	

dictionary.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/dictionary.c -o $@

iniparser.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/iniparser.c -o $@

lm75_tmp100.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c ${libTestOfBoard_SRC}/lm75_tmp100.c -o $@

#libezxml : 
#	cd 3rdparty/ezxml;
#	${MAKE} 

${libTestOfBoard_LIBRARIES} : ${libTestOfBoard_OBJS}
	rm -f $@
	${AR} -crs ${LIBRARY_DIRS}/$@ ${libTestOfBoard_OBJS}
	rm -f ${libTestOfBoard_OBJS}

${TestingTool_EXE}:
	${CC} ${TestingTool_CFLAGS} ${INCLUDE_DIRS} ${TestingTool_SRC} \
	${LIBRARY_DIRS}/${libTestOfBoard_LIBRARIES} -o ${BINARY_DIRS}/${TestingTool_EXE} ${LDFLAGS} 

## Clean
clean:
	@rm -f ${LIBRARY_DIRS}/${libTestOfBoard_LIBRARIES} ${BINARY_DIRS}/${TestingTool_EXE} $(BINARY_DIRS)/${LOGFILE}
.PHONY: clean

## Install 
install:
	cp -r ./bin ${INSTALL_DIRECTORY}/
	ln -sr ${ROOT_DIRECTORY}/opt/aetina/bin/Aetina ${ROOT_DIRECTORY}/usr/bin/Aetina
.PHONY: install

## Uninstall
uninstall:
	rm ${ROOT_DIRECTORY}/usr/bin/Aetina
	rm -r ${INSTALL_DIRECTORY}/bin
.PHONY: uninstall
