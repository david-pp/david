// $Id: CppObject.h,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// CppObject.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef CPP_OBJECT_H
#define CPP_OBJECT_H

#include "MemPtr.h"
#include "Object.h"

template <typename OrigClass>
class CppObject : public Object
{
	typedef MemPtr<OrigClass, FinalValue> MemberPtr;
public:
	CppObject(ClassDef const * myClass)
	: Object(myClass), myObject(), members(OrigClass::memberBegin())
	{}

	CppObject(ClassDef const * myClass, OrigClass const & obj)
	: Object(myClass), myObject(obj), members(OrigClass::memberBegin())
	{}

	static Object * newObject(ClassDef const * myClass)
	{
		return new CppObject(myClass);
	}
	virtual Object * clone() const
	{
		return new CppObject(*this);
	}

#if defined(__BORLANDC__)
	virtual Value getValue(std::string const & name) const { return Object::getValue(name); }
	virtual void setValue(std::string const & name, Value const & v) { Object::setValue(name, v); }
#else
	using Object::getValue;
	using Object::setValue;
#endif
	virtual Value getValue(size_t idx) const
	{
		return members[idx].value(myObject);
	}

	virtual void setValue(size_t idx, Value const & v)
	{
		BaseValue * p = &(members[idx].value(myObject));
		p->set(v);
	}

protected:
	virtual BaseValue * getBaseValue(size_t idx)
	{
		return &members[idx].value(myObject);
	}

	virtual BaseValue const * getBaseValue(size_t idx) const
	{
		return &members[idx].value(myObject);
	}

private:
	MemberPtr * members;
	OrigClass myObject;
};

template <typename CppClass>
inline
#if defined(_MSC_VER)
ClassDef makeClass(ClassDef const * base, std::string const & name, CppClass * vcppBug)
#else
ClassDef makeClass(ClassDef const * base, std::string const & name)
#endif
{
	return ClassDef(base, name,
                    CppObject<CppClass>::newObject,
                    CppClass::ownAttribBegin(), CppClass::ownAttribEnd());
}

template <typename CppClass>
inline
CppObject<CppClass> makeObject(ClassDef const * cl, CppClass const & obj)
{
	return CppObject<CppClass>(cl, obj);
}
#endif // CPP_OBJECT_H
