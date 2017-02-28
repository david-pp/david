// $Id: RealValue.h,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// RealValue.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef REAL_VALUE_H_
#define REAL_VALUE_H_

#include <string>
#include <sstream>
#include "Value.h"

template <typename PlainT>
class RealValue : public TypedValue<PlainT>
{
public:
	RealValue(PlainT v)
	: val(v)
	{}

	RealValue()	{}

	FinalValue * clone() const
	{
		return new RealValue(*this);
	}

	std::string asString() const
	{
		std::ostringstream os;
		os << val;
		return os.str();
	}

	operator PlainT() const	// conversion to plain type
	{
		return val;
	}

	void set(Value const & v)
	{
#if defined(_MSC_VER) || defined (__GNUC__)
		val = v.get(&val);
#else
		val = v.get<PlainT>();
#endif
	}

private:
	PlainT val;
};

template <>
class RealValue<std::string> : public TypedValue<std::string>, public std::string
{
public:
	RealValue(std::string const & s)
	: std::basic_string<char>(s)
	{}

	RealValue(char const * s)
	: std::basic_string<char>(s)
	{}

	RealValue()	{}

	FinalValue * clone() const
	{
		return new RealValue(*this);
	}

	std::string asString() const
	{
#ifndef __BORLANDC__
		return *this;
#else
		std::string const & ref = *this;
		return ref;
#endif
	}

// this conversion will only be used through the virtual function call
	operator std::string() const	// conversion to plain type
	{
#ifndef __BORLANDC__
		return *this;
#else
		std::string const & ref = *this;
		return ref;
#endif
	}

	void set(Value const & v)
	{
#if defined(_MSC_VER) || defined (__GNUC__)
		*this = v.get((std::string *)0);
#else
		*this = v.get<std::string>();
#endif
	}
};

class Object;
// spezializations for RealValue<Object *>
template <>
RealValue<Object *>::RealValue() : val(0) {}

template <>
std::string RealValue<Object *>::asString() const
{
//	return obj->instanceOf()->className() + " *";
	return "Object *";
}

#endif // REAL_VALUE_H_
