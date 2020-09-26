##===================================================================##
## File Name: makefile
##
## Create Author: Tom Hui
##
## Create Date: Web Jan 24 2207 2018
##
## Description:
##		
##===================================================================##

include ../inc.mk

TARGET :=
TARGET_ME = ${LIB_PATH}/libhuihttp.a
ifneq "$(OS_NAME)" "Linux"
TARGET += ${TARGET_ME}.a
else 
TARGET += ${TARGET_ME}
endif

SRC = $(wildcard *.cpp)
OBJS = ${patsubst %.cpp, %.o, ${SRC}}

CXX_INCLUDE += -I${HUIBASE_INCLUDE_PATH} 
CXX_INCLUDE += -I${HUI_DEPS_PATH}/libconfig/lib
CXX_INCLUDE += -I${HUI_DEPS_PATH}/libevent/include
CXX_INCLUDE += -I${HUI_DEPS_PATH}/hiredis

LINK_CXX_LIBS += -L${HUIBASE_LIB_PATH} 
LINK_CXX_LIBS += ${DEPS_PATH}/libconfig/lib/.libs/libconfig++.a
LINK_CXX_LIBS += -lhuibase -lhuicrypto
LINK_CXX_LIBS += -L${HUI_DEPS_PATH}/libevent/.libs
LINK_CXX_LIBS += -levent -levent_pthreads -lpthread

CXX_CFLAGS += -fstack-protector-all
LINK_CXX_CFLAGS += -fstack-protector-all 

ifeq "$(OS_NAME)" "Darwin"
LINK_CC_LIBS += -liconv
endif

LINK_CXX_LIBS += -ldl

TTYPE = AFILE

all: def

def: ${TARGET}

test_me:
	@echo ${OBJS}


include ../route.mk

