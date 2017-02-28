// $Id: MapObject.h,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MapObject.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef MAPOBJECT_H_
#define MAPOBJECT_H_

#include "DynaObject.h"
#include <vector>

class Value;

class MapObject : public DynaObject
{
public:
	explicit MapObject(ClassDef const *);
	MapObject(MapObject const & rhs);
	MapObject(ClassDef const *, Object *);

	static Object * newObject(ClassDef const *);
	virtual Object * clone() const;

	void attachObject(Object *);
	Object * getTarget();


private:
	Object * target;
};

#endif // MAPOBJECT_H_
