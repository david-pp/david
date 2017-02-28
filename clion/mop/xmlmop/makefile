# $Id: makefile,v 1.5 2000/06/06 01:52:31 dv Exp $
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

#TARGET=xmlmop
#TARGET=xmlmop.exe

#TARGET_FLAG=-o $(TARGET)
#TARGET_FLAG=-e$(TARGET)

############### Don't change anything below without a really good reason


.SUFFIXES: .cpp .$(OBJ_SUF)

.cpp.$(OBJ_SUF):
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<

MOPDIR=../mop
CPPFLAGS= -I$(MOPDIR)

all: $(TARGET)

	
MOP_OBJS= \
	$(MOPDIR)/Class.$(OBJ_SUF) \
	$(MOPDIR)/DynaObject.$(OBJ_SUF) \
	$(MOPDIR)/Object.$(OBJ_SUF) \
	$(MOPDIR)/SimpleObject.$(OBJ_SUF) \
	$(MOPDIR)/Type.$(OBJ_SUF) \
	$(MOPDIR)/Value.$(OBJ_SUF)

OBJS= \
	main.$(OBJ_SUF) \
	MapObject.$(OBJ_SUF) \
	MapValue.$(OBJ_SUF) \
	address.$(OBJ_SUF) \
	testfunc.$(OBJ_SUF)

$(TARGET) : $(OBJS) $(MOP_OBJS)
	$(CXX) $(TARGET_FLAG) $(OBJS) $(LIBS) $(MOP_OBJS)

# DO NOT DELETE

MapObject.$(OBJ_SUF): MapObject.h
MapValue.$(OBJ_SUF): MapValue.h MapObject.h
main.$(OBJ_SUF): testfunc.h
testfunc.$(OBJ_SUF): testfunc.h address.h MapObject.h XmlHelper.h MTag.h MapValue.h

