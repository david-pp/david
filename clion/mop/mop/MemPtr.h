// $Id: MemPtr.h,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MemPtr.h - wrapper class for pointer to data members
// Copyright (c) 2000 Detlef Vollmann

#ifndef MEMPTR_H
#define MEMPTR_H
// naming:
// BaseType:		the class to which a pointer-to-member is applied
// TargetType:		the result type to which a pointer-to-member points
// BaseTargetType:	a base class of TargetType

template <typename BaseType, typename BaseTargetType>
class MemPtrBase
{
public:
	virtual BaseTargetType & value(BaseType & obj) const = 0;
	virtual BaseTargetType const & value(BaseType const & obj) const = 0;
	virtual ~MemPtrBase() {};

protected:
	MemPtrBase() {}

private:
	MemPtrBase(MemPtrBase const &);
	MemPtrBase & operator=(MemPtrBase const &);
};

template <typename BaseType, typename BaseTargetType, typename TargetType>
class TypedMemPtr : public MemPtrBase<BaseType, BaseTargetType>
{
public:
	TypedMemPtr(TargetType BaseType::* ptr)
	 : p(ptr)
	{}

	BaseTargetType & value(BaseType & obj) const
	{
		return obj.*p;
	}

	BaseTargetType const & value(BaseType const & obj) const
	{
		return obj.*p;
	}

private:
	TargetType BaseType::* p;
};

template <typename BaseType, typename BaseTargetType>
class MemPtr // this is a handle only
{
public:
    template <typename BaseType2, typename TargetType>
    explicit MemPtr(TargetType BaseType2::* ptr)
     : p(new TypedMemPtr<BaseType, BaseTargetType, TargetType>
           (static_cast<TargetType BaseType::*>(ptr)))
    {}
    
    ~MemPtr()
    {
        delete p;
    }
    
    BaseTargetType & value(BaseType & obj) const
    {
        return p->value(obj);
    }
    
    BaseTargetType const & value(BaseType const & obj) const
    {
        return p->value(obj);
    }
    
private:
    MemPtrBase<BaseType, BaseTargetType> * p;
};
#endif // MEMPTR_H
