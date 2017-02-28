// $Id: FuncValue.h,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// FuncValue.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef FUNC_VALUE_H_
#define FUNC_VALUE_H_

#include <string>
#include "Value.h"

class Object;

template <typename ReturnPlainT>
class FuncValue : public ComputingValue
{
	typedef ReturnPlainT (*FunctionT)(Object *);
public:
	FuncValue(Object * o, FunctionT getFunction)
	: ComputingValue(o), getF(getFunction)
	{}

	BaseValue * clone(Object * o) const
	{
		return new FuncValue(o, getF);
	}

	Value getFinal() const
	{
		return RealValue<ReturnPlainT>(getF(obj));
	}

	void set(Value const & v)
	{
		throw "not supported";
	}

private:
	FunctionT getF;
};

#endif // FUNC_VALUE_H_
