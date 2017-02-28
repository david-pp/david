# $Id: makefile,v 1.1.1.1 2000/09/28 03:16:37 dv Exp $
# Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
# sell and distribute this software is granted provided this copyright
# notice appears in all copies.

# This software is provided "as is" without express or implied
# warranty, and with no claim as to its suitability for any purpose.

# See http://www.vollmann.ch/ for most recent version.

# This is a quite simple makefile to produce a running program.
# It should run with most platforms/compilers.
# Just define the following variables according to your environment.

#OBJ_SUF=o
#OBJ_SUF=obj

#CXX=g++
#CXX=bcc32.exe

#CXXFLAGS=-g -ansi
#CXXFLAGS=-v

#LIBS=-L/usr/local/STL
#LIBS=

#TARGET=testnmp
#TARGET=testnmp.exe

#TARGET_FLAG=-o $(TARGET)
#TARGET_FLAG=-e$(TARGET)

############### Don't change anything below without a really good reason


.SUFFIXES: .cpp .$(OBJ_SUF)

.cpp.$(OBJ_SUF):
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

all: $(TARGET)

	
OBJS= main.$(OBJ_SUF)

$(TARGET) : $(OBJS)
	$(CXX) $(TARGET_FLAG) $(OBJS) $(LIBS)

