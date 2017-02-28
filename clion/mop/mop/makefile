# $Id: makefile,v 1.1 2000/06/06 01:55:02 dv Exp $
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

############### Don't change anything below without a really good reason

.SUFFIXES: .cpp .$(OBJ_SUF)

.cpp.$(OBJ_SUF):
	$(CXX) $(CXXFLAGS) -c $<

	
MOP_OBJS= \
	Class.$(OBJ_SUF) \
	DynaObject.$(OBJ_SUF) \
	Object.$(OBJ_SUF) \
	SimpleObject.$(OBJ_SUF) \
	Type.$(OBJ_SUF) \
	Value.$(OBJ_SUF)

all: $(MOP_OBJS)

# DO NOT DELETE

Class.$(OBJ_SUF): Class.h Attribute.h Type.h
Class.$(OBJ_SUF): Object.h Value.h
DynaObject.$(OBJ_SUF): DynaObject.h Object.h Class.h
DynaObject.$(OBJ_SUF): Attribute.h Type.h Value.h
Object.$(OBJ_SUF): Object.h Class.h Attribute.h
Object.$(OBJ_SUF): Type.h Value.h
SimpleObject.$(OBJ_SUF): SimpleObject.h DynaObject.h
SimpleObject.$(OBJ_SUF): Object.h Class.h Attribute.h
SimpleObject.$(OBJ_SUF): Type.h Value.h
Type.$(OBJ_SUF): Type.h RealValue.h Value.h
Value.$(OBJ_SUF): Value.h
