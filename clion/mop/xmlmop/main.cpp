// $Id: main.cpp,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// main.cpp
// Copyright (c) 2000, Detlef Vollmann
#include "testfunc.h"
#include "Type.h"

void init()
{
	Type::init();
}

int main()
{
	init();

	testfunc();

	return 0;
}
