// $Id: MapValue.cpp,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MapValue.cpp
// Copyright (c) 2000, Detlef Vollmann

#include "MapValue.h"
#include "MapObject.h"


MapValue::MapValue(Object * o, size_t targetIndex)
: ComputingValue(o), trgtIdx(targetIndex)
{}

BaseValue * MapValue::clone(Object * o) const
{
	return new MapValue(o, trgtIdx);
}

Value MapValue::getFinal() const
{
	Object * trgt = dynamic_cast<MapObject *>(obj)->getTarget();
	return trgt->getValue(trgtIdx);
}

void MapValue::set(Value const & v)
{
	Object * trgt = dynamic_cast<MapObject *>(obj)->getTarget();
	trgt->setValue(trgtIdx, v);
}
