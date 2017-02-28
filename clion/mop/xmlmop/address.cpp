// $Id: address.cpp,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// address.cpp
// Copyright (c) 2000, Detlef Vollmann

// the Borland Compiler couldn't compile Address::getClassInfo() inline!

#include "address.h"

size_t Address::ownAttribCount()
{
	return 8;
}

Attribute * Address::ownAttribBegin()
{
	static Attribute a[] = {Attribute("Street", Type::stringT),
							Attribute("HouseNo", Type::stringT),
							Attribute("City", Type::stringT),
							Attribute("Region", Type::stringT),
							Attribute("PostCode", Type::stringT),
							Attribute("Country", Type::stringT),
							Attribute("Phone", Type::stringT),
							Attribute("Internet", Type::stringT)
							};
	return a;
}

Attribute * Address::ownAttribEnd()
{
    return ownAttribBegin() + ownAttribCount();
}

Address::MemberPtr * Address::memberBegin()
{
	static MemberPtr m[] = {MemberPtr(&Address::street),
							MemberPtr(&Address::no),
							MemberPtr(&Address::city),
							MemberPtr(&Address::region),
							MemberPtr(&Address::postCode),
							MemberPtr(&Address::country),
							MemberPtr(&Address::phone),
							MemberPtr(&Address::eMail)
							};

	return m;
}

Address::MemberPtr * Address::memberEnd()
{
    return memberBegin() + 8;
}

