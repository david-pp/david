// $Id: Attribute.h,v 1.4 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Attribute.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include "Type.h"

class Attribute
{
public:
	Attribute(const std::string & name_, const Type::TypeT typeId)
	: name(name_),
	  type_(typeId)
	{}

	const std::string & getName() const
	{
		return name;
	}

	Type getType() const
	{
		return type_;
	}

private:
	std::string name;
	Type type_;
};

#endif // ATTRIBUTE_H
