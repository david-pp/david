// $Id: Cd.h,v 1.5 2000/09/28 03:22:35 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Cd.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef CD_H
#define CD_H

#include <string>
#include "Product.h"

class Cd : public Product
{
	typedef MemPtr<Cd, FinalValue> MemberPtr;
//	typedef BaseValue Cd::* MemberPtr;
public:
	Cd(const std::string & composer_,
	   const std::string & title_,
	   const std::string & musician_,
	   double price_)
	: composer(composer_),
	  title(title_),
	  musician(musician_),
	  price(price_)
	{}
	Cd() {}

	std::string getName() const
	{
		std::string name;

		name = composer + ": " + title;

		return name.substr(0, 40);
	}

	double getPrice() const
	{
		return price;
	}

	double getWeight() const
	{
		return 60;
	}

	static size_t ownAttribCount()
	{
		return 4;
	}
	static Attribute * ownAttribBegin()
	{
		static Attribute a[] = {Attribute("Composer", Type::stringT),
								Attribute("Title", Type::stringT),
								Attribute("Musician", Type::stringT),
								Attribute("Price", Type::doubleT)
								};
		return a;
	}
	static Attribute * ownAttribEnd()
	{
        return ownAttribBegin() + ownAttribCount();
    }
	static MemberPtr * memberBegin()
	{
		static MemberPtr m[] = {MemberPtr(&Cd::productNo),
								MemberPtr(&Product::weight),
								MemberPtr(&Cd::composer),
								MemberPtr(&Cd::title),
								MemberPtr(&Cd::musician),
								MemberPtr(&Cd::price)
								};

		return m;
	}
	static MemberPtr * memberEnd()
	{
        return memberBegin() + 6;
    }

private:
	RealValue<std::string> composer, title, musician;
	RealValue<double> price;
};
#endif // CD_H
