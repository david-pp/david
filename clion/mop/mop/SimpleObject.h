// $Id: SimpleObject.h,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// SimpleObject.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef SIMPLE_OBJECT_H_
#define SIMPLE_OBJECT_H_

#include "DynaObject.h"

class SimpleObject : public DynaObject
{
public:
	explicit SimpleObject(ClassDef const *);
	SimpleObject(SimpleObject const &);

	static Object * newObject(ClassDef const *);
	virtual Object * clone() const;
};
#endif // SIMPLE_OBJECT_H_
