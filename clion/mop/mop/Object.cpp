// $Id: Object.cpp,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Object.cpp
// Copyright (c) 2000, Detlef Vollmann
#include "Object.h"
#include "Class.h"
#include "Value.h"

Object::Object(ClassDef const * class_)
: myClass(class_)
{
}

ClassDef const * Object::instanceOf() const
{
	return myClass;
}

Value Object::getValue(std::string const & name) const
{
	size_t idx = instanceOf()->findAttribute(name); // check for not found
	return getValue(idx);
}

void Object::setValue(std::string const & name, Value const & v)
{
	size_t idx = instanceOf()->findAttribute(name); // check for not found
	setValue(idx, v);
}

BaseValue * Object::getBaseValue(Object * o, size_t idx)
{
	return o->getBaseValue(idx);
}

BaseValue const * Object::getBaseValue(Object const * o, size_t idx)
{
	return o->getBaseValue(idx);
}

