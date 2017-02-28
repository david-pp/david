// $Id: SimpleObject.cpp,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// SimpleObject.cpp
// Copyright (c) 2000, Detlef Vollmann
#include "SimpleObject.h"

SimpleObject::SimpleObject(ClassDef const * class_)
: DynaObject(class_)
{
	init(this);
}

SimpleObject::SimpleObject(SimpleObject const & rhs)
: DynaObject(rhs)
{
	copy(this, rhs);
}

Object * SimpleObject::newObject(ClassDef const * myClass)
{
		return new SimpleObject(myClass);
}

Object * SimpleObject::clone() const
{
	return new SimpleObject(*this);
}

