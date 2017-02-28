// $Id: MemPtrNew.h,v 1.1.1.1 2000/09/28 03:16:37 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MemPtrNew.h - wrapper class for pointer to data members w/o BaseValue
// Copyright (c) 2000 Detlef Vollmann

#ifndef MEMPTRNEW_H
#define MEMPTRNEW_H
// naming:
// BaseType:		the class to which a pointer-to-member is applied
// TargetType:		the result type to which a pointer-to-member points
// BaseTargetType:	a base class of TargetType

template <typename BaseType>
class MemPtrBase
{
public:
//	virtual BaseTargetType value(BaseType & obj) const = 0;
//	virtual BaseTargetType const & value(BaseType const & obj) const = 0;

protected:
	MemPtrBase() {}
	virtual ~MemPtrBase() {};

private:
	MemPtrBase(MemPtrBase const &);
	MemPtrBase & operator=(MemPtrBase const &);
};

template <typename BaseType, typename TargetType>
class TypedMemPtr : public MemPtrBase<BaseType>
{
public:
	TypedMemPtr(TargetType BaseType::* ptr)
	 : p(ptr)
	{}

	TargetType getValue(BaseType & obj) const
	{
		return obj.*p;
	}

	void setValue(BaseType const & obj, TargetType const & val)
	{
		obj.*p = val;
	}

private:
	TargetType BaseType::* p;
};

template <typename BaseType>
class MemPtr // this is a handle only
{
public:
/*
	template <typename TargetType>
	explicit MemPtr(TargetType BaseType::* ptr)
	 : p(new TypedMemPtr<BaseType, BaseTargetType, TargetType>(ptr))
	{}
*/
	template <typename BaseType2, typename TargetType>
	explicit MemPtr(TargetType BaseType2::* ptr)
	 : p(new TypedMemPtr<BaseType, TargetType>(static_cast<TargetType BaseType::*>(ptr)))
	{}

	template <typename TargetType>
#if defined(_MSC_VER) || defined (__GNUC__)
// version for VC++ and GNU :-{
	TargetType get(BaseType & obj, TargetType * vcppbug) const
#else
// version for standard compiler :-}
	TargetType get(BaseType & obj) const
#endif
	{
		TypedMemPtr<BaseType, TargetType> const & tmp
			= dynamic_cast<TypedMemPtr<BaseType, TargetType> const &>(*p);
		return tmp.getValue(obj);	// uses conversion operator
	}

private:
	MemPtrBase<BaseType> * p;
};
#endif // MEMPTRNEW_H
