// $Id: Value.h,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Value.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef VALUE_H
#define VALUE_H

#include <string>

class Object;
class Value;

class BaseValue
{
public:
	virtual ~BaseValue() {}
	virtual BaseValue * clone(Object *) const = 0;

	virtual Value getFinal() const = 0;
	virtual void set(Value const & v) = 0;

protected:
	BaseValue() {}
	BaseValue(BaseValue const &) {}
	BaseValue & operator=(BaseValue const &) { return *this; }

private:
	// Type info
};

class ComputingValue : public BaseValue
{
public:
	ComputingValue(Object * o);

protected:
	ComputingValue(ComputingValue const &);

	Object * const obj;
};

class FinalValue : public BaseValue
{
public:
	virtual BaseValue * clone(Object *) const;
	virtual FinalValue * clone() const = 0;

	virtual std::string asString() const = 0;
	virtual Value getFinal() const;

protected:
	FinalValue();
	FinalValue(FinalValue const &);
	FinalValue & operator=(FinalValue const &);
};

template <typename PlainT>
class TypedValue : public FinalValue
{
public:

	virtual operator PlainT() const = 0;

protected:
	TypedValue() {}
	TypedValue(TypedValue const & rhs) : FinalValue(rhs) {}
	TypedValue & operator=(TypedValue const & rhs)
	{
		FinalValue::operator =(rhs);

		return *this;
	}
};

class Value	// Value handle
{
public:
	Value(FinalValue const &);
	Value(Value const &);
    explicit Value(FinalValue * = 0);

	~Value();

	Value & operator=(const Value &);

	template <typename PlainT>
#if defined(_MSC_VER) || defined (__GNUC__)
// version for VC++ and GNU :-{
	PlainT get(PlainT * vcppbug) const
#else
// version for standard compiler :-}
	PlainT get() const
#endif
	{
        if (v)
        {
		    TypedValue<PlainT> const & tv
                = dynamic_cast<TypedValue<PlainT> const &>(*v);
		    return tv;	// uses conversion operator
        }
        else
        {
            return PlainT();
        }
	}

	std::string asString() const;

private:
	FinalValue * v;
};


#endif // VALUE_H
