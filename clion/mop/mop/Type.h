// $Id: Type.h,v 1.4 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Type.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef TYPE_H
#define TYPE_H

#include <vector>

class FinalValue;
class Object;

class Type
{
public:
	enum TypeT {stringT, intT, doubleT, objectT, unknownT};

	explicit Type(TypeT typeId);

	FinalValue * newValue() const;

	TypeT getType() const;

	static void init();

private:
	TypeT typeId;
	static std::vector<FinalValue *> prototypes;
};

#endif // TYPE_H
