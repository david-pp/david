// $Id: MapObject.cpp,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MapObject.cpp
// Copyright (c) 2000, Detlef Vollmann

#include "MapObject.h"


MapObject::MapObject(ClassDef const * class_)
: DynaObject(class_), target(0)
{
	init(this);
}

MapObject::MapObject(MapObject const & rhs)
: DynaObject(rhs), target(rhs.target)
{
	copy(this, rhs);
}

MapObject::MapObject(ClassDef const * class_, Object * trgt)
: DynaObject(class_), target(trgt)
{
	init(this);
}

Object * MapObject::newObject(ClassDef const * myClass)
{
		return new MapObject(myClass);
}

Object * MapObject::clone() const
{
	return new MapObject(*this);
}

void MapObject::attachObject(Object * trgt)
{
	target = trgt;
}

Object * MapObject::getTarget()
{
	return target;
}

