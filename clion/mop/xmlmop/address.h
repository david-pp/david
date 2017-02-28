// $Id: address.h,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// address.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef ADDRESS_H_
#define ADDRESS_H_
#include <string>
#include <iosfwd>
#include "Attribute.h"
#include "RealValue.h"
#include "MemPtr.h"

class Address
{
	typedef MemPtr<Address, FinalValue> MemberPtr;
public:
	Address(std::string const & street_, std::string const & houseNo_, std::string const & city_,
			std::string const & region_, std::string const & postCode_, std::string const & country_,
			std::string const & phone_, std::string const & eMail_)
	 : street(street_), no(houseNo_), city(city_),
	   region(region_), postCode(postCode_), country(country_),
	   phone(phone_), eMail(eMail_)
	{}
	Address()
	{}

	static size_t ownAttribCount();
	static Attribute * ownAttribBegin();
	static Attribute * ownAttribEnd();
	static MemberPtr * memberBegin();
	static MemberPtr * memberEnd();

private:
	RealValue<std::string> street, no, city, region, postCode, country, phone, eMail;
};

#endif // ADDRESS_H_
