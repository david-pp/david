// $Id: Object.h,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Object.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef OBJECT_H
#define OBJECT_H

#include "Class.h"
#include <vector>

class Value;
class BaseValue;

class Object
{
public:
	Object(ClassDef const * class_);
	virtual ~Object() {}

	virtual Object * clone() const = 0;

	ClassDef const * instanceOf() const;

	virtual Value getValue(size_t idx) const = 0;
	virtual Value getValue(std::string const & name) const;
	virtual void setValue(size_t idx, Value const &) = 0;
	virtual void setValue(std::string const & name, Value const &);

protected:
	Object(const Object & rhs) : myClass(rhs.myClass) {}
	virtual BaseValue * getBaseValue(size_t idx) = 0;
	virtual BaseValue const * getBaseValue(size_t idx) const = 0;
	// the static function is present to avoid getBaseValue to be public
	static BaseValue * getBaseValue(Object *, size_t idx);
	static BaseValue const * getBaseValue(Object const *, size_t idx);

private:
	ClassDef const * const myClass;
};
#endif // OBJECT_H
