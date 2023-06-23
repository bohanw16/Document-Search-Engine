# Copyright ©2022 Dylan Hartono.  All rights reserved.  Permission is
# hereby granted to students registered for University of Washington
# CSE 333 for use solely during Summer Quarter 2022 for purposes of
# the course.  No other use, copying, distribution, or modification
# is permitted without prior written consent. Copyrights for
# third-party components of this work must be honored.  Instructors
# interested in reusing these course materials should contact the
# author.

# define the commands we will use for compilation and library building
AR = ar
ARFLAGS = rcs
CC = gcc
CXX = g++

# define useful flags to cc/ld/etc.
MESHDIRS = -I. -I.. -I./libpart1 -I./libpart2 -I./libpart3/readers -I./libpart3 -I./public -I./private -I./tests
CFLAGS = -g -Wall -O0 -std=c++17 $(MESHDIRS)
LDFLAGS = -L./public -L./libpart1 -L./libpart2 -L./libpart3 -lhw4 -lhw3 -lhw2 -lhw1 -lpthread
CPPUNITFLAGS = -L../gtest -lgtest

# MESHDIRS = -I. -I.. -I./libhw1 -I./libhw2 -I./libhw3/readers -I./libhw3 -I./public -I./private -I./tests
# CFLAGS = -g -Wall -O0 -std=c++17 $(MESHDIRS)
# LDFLAGS = -L./public -L./libhw1 -L./libhw2 -L./libhw3 -lhw4 -lhw3 -lhw2 -lhw1 -lpthread
# CPPUNITFLAGS = -L../gtest -lgtest

# define common dependencies
OBJS_COMMON_BASE = ThreadPool.o ServerSocket.o HttpServer.o HttpConnection.o FileReader.o
OBJS_COMMON = $(addprefix objs/,$(OBJS_COMMON_BASE))

OBJS_GOOD = $(OBJS_COMMON) objs/HttpUtils.o

HEADERS = public/HttpConnection.h \
	  public/HttpServer.h \
	  public/ServerSocket.h \
	  public/ThreadPool.h \
	  public/HttpUtils.h \
	  public/HttpRequest.h public/HttpResponse.h \
	  public/FileReader.h

TESTOBJS_BASE = test_serversocket.o test_threadpool.o test_filereader.o \
	   test_httpconnection.o test_httputils.o test_suite.o
TESTOBJS = $(addprefix objs/,$(TESTOBJS_BASE))

all: objs http333d tests/test_suite

http333d: http333d.o public/libhw4.a $(HEADERS)
	$(CXX) $(CFLAGS) -o $@ http333d.o public/libhw4.a $(LDFLAGS)

public/libhw4.a: $(OBJS_GOOD) $(HEADERS)
	$(AR) $(ARFLAGS) $@ $(OBJS_GOOD)

tests/test_suite: $(TESTOBJS) public/libhw4.a $(HEADERS)
	$(CXX) $(CFLAGS) -o $@ $(TESTOBJS) \
	$(CPPUNITFLAGS) $(LDFLAGS) -lpthread

objs/%.o: private/%.cc $(HEADERS)
	$(CXX) $(CFLAGS) -c -o $@ $<

objs/%.o: tests/%.cc $(HEADERS)
	$(CXX) $(CFLAGS) -c -o $@ $<

%.o: %.cc $(HEADERS)
	$(CXX) $(CFLAGS) -c -o $@ $<

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -std=c17 -o $@ $<

objs:
	mkdir objs

clean:
	/bin/rm -rf objs/ *.o *~ tests/test_suite http333d public/libhw4.a
