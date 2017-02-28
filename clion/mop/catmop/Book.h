// $Id: Book.h,v 1.5 2000/09/28 03:22:35 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Book.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef BOOK_H
#define BOOK_H

#include <string>
#include "Product.h"

class Book : public Product
{
	typedef MemPtr<Book, FinalValue> MemberPtr;
public:
	Book(const std::string & author_,
		 const std::string & title_,
		 const std::string & publisher_,
		 double price_,
		 double weight_)
	: author(author_),
	  title(title_),
	  publisher(publisher_),
	  price(price_),
	  weight(weight_)
	{}
	Book() {}

	std::string getName() const
	{
		std::string name;

		name = author + ": " + title;

		return name.substr(0, 40);
	}

	double getPrice() const
	{
		return price;
	}

	double getWeight() const
	{
		return weight;
	}

	static size_t ownAttribCount()
	{
		return 5;
	}
	static Attribute * ownAttribBegin()
	{
		static Attribute a[] = {Attribute("Author", Type::stringT),
								Attribute("Title", Type::stringT),
								Attribute("Publisher", Type::stringT),
								Attribute("Price", Type::doubleT),
								Attribute("Weight", Type::doubleT)
								};
		return a;
	}
	static Attribute * ownAttribEnd()
	{
        return ownAttribBegin() + ownAttribCount();
    }
	static MemberPtr * memberBegin()
	{
		static MemberPtr m[] = {MemberPtr(&Book::productNo),
								MemberPtr(&Product::weight),
								MemberPtr(&Book::author),
								MemberPtr(&Book::title),
								MemberPtr(&Book::publisher),
								MemberPtr(&Book::price),
								MemberPtr(&Book::weight)
								};

		return m;
	}
	static MemberPtr * memberEnd()
	{
        return memberBegin() + 7;
    }

private:
	RealValue<std::string> author, title, publisher;
	RealValue<double> price, weight;
};
#endif // BOOK_H
