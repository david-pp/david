// $Id: Type.cpp,v 1.4 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Type.cpp
// Copyright (c) 2000, Detlef Vollmann
#include "Type.h"
#include "RealValue.h"

class Object;

Type::Type(TypeT typeId_)
: typeId(typeId_)
{}

FinalValue * Type::newValue() const
{
	return prototypes[typeId]->clone();
}

Type::TypeT Type::getType() const
{
	return typeId;
}

void Type::init()
{
	prototypes[stringT] = new RealValue<std::string>("");
	prototypes[intT] = new RealValue<int>(0);
	prototypes[doubleT] = new RealValue<double>(0);
	prototypes[objectT] = new RealValue<Object *>(0);
}

std::vector<FinalValue *> Type::prototypes(Type::unknownT);

