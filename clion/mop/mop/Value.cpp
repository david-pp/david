// $Id: Value.cpp,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Value.cpp
// Copyright (c) 2000, Detlef Vollmann

#include "Value.h"

ComputingValue::ComputingValue(Object * o)
 : obj(o)
{}

ComputingValue::ComputingValue(ComputingValue const & rhs)
 : BaseValue(rhs), obj(rhs.obj)
{}

BaseValue * FinalValue::clone(Object *) const
{
	return clone();
}

Value FinalValue::getFinal() const
{
	return *this;
}

FinalValue::FinalValue()
{}

FinalValue::FinalValue(FinalValue const & rhs)
 : BaseValue(rhs)
{}

FinalValue & FinalValue::operator=(FinalValue const & rhs)
{
	BaseValue::operator =(rhs);

	return *this;
}

Value::Value(FinalValue const & fv)
 : v(fv.clone())
{}

Value::Value(Value const & rhs)
 : v(rhs.v ? rhs.v->clone() : 0)
{}

Value::Value(FinalValue *fv)
 : v(fv)
{}

Value::~Value()
{
	delete v;
}

Value & Value::operator=(const Value & rhs)
{
    // this is not a typical pimpl assignment, but a set()
    if (v)
    {
        if (rhs.v)
        { // fine, all v's exist
            v->set(rhs);
        }
        else
        { // the other v doesn't exist, so we must delete our own :-(
            FinalValue * old = v;
            v = 0;
            delete old;
        }
    }
    else
    { // we don't have a v, so just copy the other
	    v = (rhs.v ? rhs.v->clone() : 0);
    }

	return *this;
}

std::string Value::asString() const
{
	if (v)
    {
        return v->asString();
    }
    else
    {
        return std::string();
    }
}
