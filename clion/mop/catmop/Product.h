// $Id: Product.h,v 1.5 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Product.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef PRODUCT_H
#define PRODUCT_H
#include <string>
#include "MemPtr.h"
#include "RealValue.h"
#include "Object.h"

class Product
{
	typedef MemPtr<Product, FinalValue> MemberPtr;
public:
	Product()
	 : productNo(0), weight(0)
	{
		// create a unique product number
	}

	virtual ~Product() {}

	virtual std::string getName() const = 0;
	virtual double getPrice() const = 0;
	virtual double getWeight() const = 0;

	static size_t ownAttribCount()
	{
		return 2;
	}
	static Attribute * ownAttribBegin()
	{
		static Attribute a[] = {Attribute("Product No", Type::intT),
								Attribute("Weight", Type::doubleT)};
		return a;
	}
	static Attribute * ownAttribEnd()
	{
        return ownAttribBegin() + ownAttribCount();
    }
	static MemberPtr * memberBegin()
	{
		static MemberPtr m[] = {MemberPtr(&Product::productNo),
								MemberPtr(&Product::weight)};

		return m;
	}
	static MemberPtr * memberEnd()
	{
        return memberBegin() + ownAttribCount();
    }

protected:
	Product(const Product & rhs)
     : productNo(rhs.productNo),
       weight(rhs.weight)
    {}
	RealValue<int> productNo;
	RealValue<double> weight;
};

// DynaProduct acts as interface for ShoppingCart for dynamic
// MOP classes derived from Product
class DynaProduct : public Product
{
public:
    DynaProduct(Object const * o) : obj(o) {}

    virtual std::string getName() const
    {
        Value v = obj->getValue("Name");
#if defined(_MSC_VER) || defined (__GNUC__)
        return v.get((std::string *)0);
#else
        return v.get<std::string>();
#endif
    }

    virtual double getPrice() const
    {
        Value v = obj->getValue("Price");
#if defined(_MSC_VER) || defined (__GNUC__)
        return v.get((double *)0);
#else
        return v.get<double>();
#endif
    }
    
    virtual double getWeight() const
    {
        Value v = obj->getValue("Weight");
#if defined(_MSC_VER) || defined (__GNUC__)
        return v.get((double *)0);
#else
        return v.get<double>();
#endif
    }

private:
    Object const * const obj;
};

#endif // PRODUCT_H
