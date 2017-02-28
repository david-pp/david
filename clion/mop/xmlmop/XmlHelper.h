// $Id: XmlHelper.h,v 1.5 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// XmlHelper.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef XMLHELPER_H_
#define XMLHELPER_H_

#include <iostream>
#include "MTag.h"
#include "Object.h"
#include "Class.h"
#include "Value.h"

inline void printXML(std::ostream & o, Object const & obj)
{
	ClassDef const * cl = obj.instanceOf();
	MTag addr(o, cl->getName(), "\n");

    ClassDef::AttrIterator a;
	int idx;

	for (a = cl->attribBegin(), idx = 0;
         a != cl->attribEnd(); ++a, ++idx)
	{
		if (a->getType().getType() == Type::objectT)
		{
			Value const & v = obj.getValue(idx);
#if defined(_MSC_VER) || defined (__GNUC__)
			Object * subObj = v.get((Object **)0);
#else
			Object * subObj = v.get<Object *>();
#endif

/*
			BaseValue * bv = v.get();
this version doesn't work on VC++6 :-(
probably due to a prematurely called destructor on the temporary Value
			BaseValue * bv = obj.getValue(idx).get();
			TypedValue<Object *> const & objVal = dynamic_cast<TypedValue<Object *> const &>(*bv);
*/
			printXML(o, *subObj);
		}
		else
		{
			MTag(o, a->getName(), obj.getValue(idx).asString());
		}
	}

}

#endif // XMLHELPER_H_
