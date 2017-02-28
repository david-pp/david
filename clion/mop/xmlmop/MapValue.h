// $Id: MapValue.h,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MapValue.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef MAP_VALUE_H_
#define MAP_VALUE_H_

#include "Value.h"

class Object;

class MapValue : public ComputingValue
{
public:
	MapValue(Object * o, size_t targetIndex);

	BaseValue * clone(Object * o) const;

	Value getFinal() const;
	void set(Value const & v);

private:
	size_t trgtIdx;
};

#endif // MAP_VALUE_H_
