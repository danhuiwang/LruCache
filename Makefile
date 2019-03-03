#--------------------------------------------------------------------
ifeq ($(GCOV_ON),1)
GCOV= -ftest-coverage -fprofile-arcs -lgcov
endif

#c++11
C11=/opt/rh/devtoolset-3/root/usr/bin/
PATH:=$(C11):$(PATH)


CC = gcc
CXX = g++
AR = ar cru
CFLAGS = -std=c++11 -o2 -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC $(GCOV)
SOFLAGS = -shared

INC = inc
SYSINC = /home/homework/include
SRC = src
LIB = lib
BIN = bin
TEST = test
TESTR = testr

INCFLAGS = -I$(INC) -I$(SYSINC)

CFLAGS += $(INCFLAGS)

LDFLAGS = -L/home/homework/lib64/ -lpthread -lcrypto -lgtest_main -lgtest $(GCOV)

LIBLRUCACHE = libLruCache
LIBLRUCACHE_A = $(LIB)/$(LIBLRUCACHE).a
LIBLRUCACHE_SO = $(LIB)/$(LIBLRUCACHE).so
TESTEXE = $(BIN)/LruCache_test

#--------------------------------------------------------------------

LIBOBJS =  			         \
	$(SRC)/LruCache.o               \
	$(SRC)/HashLruCache.o                \
	$(SRC)/LRU_Lock.o             	 \
	$(SRC)/LRU_Time.o     

TESTOBJS = 				 \
	$(TEST)/LruCacheTest.o		 \
	$(TEST)/HashLruCacheTest.o

TARGET =                                 \
	$(LIBLRUCACHE_A)                  \
	$(LIBLRUCACHE_SO)                 \
	$(TESTEXE)

#--------------------------------------------------------------------
.PHONY: all clean $(TARGET) $(LIBLRUCACHE_A) $(LIBLRUCACHE_SO) $(TESTEXE) install $(TESTR)

all: $(TARGET) $(TESTEXE) $(TESTR)

$(LIBLRUCACHE_SO): $(LIBOBJS)
	$(CXX) $(SOFLAGS) $^ -o $@

$(LIBLRUCACHE_A): $(LIBOBJS)
	$(AR) $@ $^

$(TESTEXE): $(TESTOBJS) $(LIBOBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

#$(TESTR):
#	exec $(TESTEXE)  --gtest_output=xml:test-output/LruCache_test.xml

install:
	cp -rf $(LIB)/* /home/homework/lib64/
	mkdir -p /home/homework/include/LruCache/
	cp -rf $(INC)/* /home/homework/include/LruCache/

clean:
	@rm -rf  $(LIBOBJS) $(TARGET) $(TESTOBJS) test-output/*

#--------------------------------------------------------------------

# make rule

%.o : %.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@
	
%.o : %.cc
	$(CXX) $(CFLAGS) -c $^ -o $@	
