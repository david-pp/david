# $Id: g++env.sh,v 1.1 2000/06/06 01:44:34 dv Exp $
# Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
# sell and distribute this software is granted provided this copyright
# notice appears in all copies.

# This software is provided "as is" without express or implied
# warranty, and with no claim as to its suitability for any purpose.

# See http://www.vollmann.ch/ for most recent version.

# This shell scripts sets environment variables to compile the MOP
# programs on two

# run with ". g++env.sh"

export LIBS CXX CXXFLAGS

CXX=/usr/local/pre/bin/g++

CC_FLAGS="-B/usr/local/pre/lib/gcc-lib/ -g"
STL_INCL="-nostdinc++ -I/usr/local/install/SGISTL/sl"
STL_LIBPATH=-L/usr/local/install/SGISTL/sl

#SGI_ALLOCATOR=-D'__STL_DEFAULT_ALLOCATOR(T)=__allocator< T , malloc_alloc >'

CXXFLAGS="$CC_FLAGS $SGI_ALLOCATOR $STL_INCL -ansi"
LIBS="-lm $STL_LIBPATH" #$STLLIB

