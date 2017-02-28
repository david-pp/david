// $Id: testfunc.cpp,v 1.4 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// testfunc.cpp
// Copyright (c) 2000, Detlef Vollmann
#include "testfunc.h"
#include "address.h"
#include "CppObject.h"
#include "MapObject.h"
#include "SimpleObject.h"
#include "XmlHelper.h"
#include "FuncValue.h"
#include "MapValue.h"

using namespace std;

namespace
{
std::string getDTDStreet(Object * o)
{
	Object * addr = dynamic_cast<DynaObject *>(o)->privObject();
	if (addr)
	{
#if defined(_MSC_VER) || defined (__GNUC__)
		std::string str(addr->getValue("Street").get((std::string *)0));
		std::string no(addr->getValue("HouseNo").get((std::string *)0));
#else
		std::string str(addr->getValue("Street").get<std::string>());
		std::string no(addr->getValue("HouseNo").get<std::string>());
#endif

		return str + " " + no;
	}
	else
	{
		return "No Street";
	}

}

std::string phoneCountry(Object * o)
{
	Object * addr = dynamic_cast<DynaObject *>(o)->privObject();
	if (addr)
	{
#if defined(_MSC_VER) || defined (__GNUC__)
		std::string co(addr->getValue("Country").get((std::string *)0));
#else
		std::string co(addr->getValue("Country").get<std::string>());
#endif

		if (co == "Helvetia")
		{
			return "41";
		}
		else
		{
			return co;
		}
	}

	return "++";
}

std::string phoneRegion(Object * o)
{
	Object * addr = dynamic_cast<DynaObject *>(o)->privObject();
	if (addr)
	{
		std::string::size_type idx;
#if defined(_MSC_VER) || defined (__GNUC__)
		std::string phoneNo(addr->getValue("Phone").get((std::string *)0));
#else
		std::string phoneNo(addr->getValue("Phone").get<std::string>());
#endif

		idx = phoneNo.find_first_of(" -/");

		if (idx != std::string::npos)
		{
			return phoneNo.substr(0, idx);
		}
		else
		{
			return "---";
		}
	}

	return "---";
}

std::string phoneNumber(Object * o)
{
	Object * addr = dynamic_cast<DynaObject *>(o)->privObject();
	if (addr)
	{
		std::string::size_type idx;

#if defined(_MSC_VER) || defined (__GNUC__)
		std::string phoneNo(addr->getValue("Phone").get((std::string *)0));
#else
		std::string phoneNo(addr->getValue("Phone").get<std::string>());
#endif

		idx = phoneNo.find_first_of(" -/");

		if (idx != std::string::npos)
		{
			return phoneNo.substr(idx+1);
		}
		else
		{
			return "00000";
		}
	}

	return "00000";
}

SimpleObject * phoneObj = 0;
ClassDef const * classPhoneNumber = 0;

Object * makePhone(Object * o)
{
	static SimpleObject * phone = 0;	// should be auto_ptr

	delete phone;

	Object * addr = dynamic_cast<DynaObject *>(o)->privObject();

	phone = new SimpleObject(classPhoneNumber);
	phone->setPrivate(addr);

	phoneObj->setValue("PhoneNumber", RealValue<Object *>(phone));

	return phoneObj;
}
} // anonymous namespace

void
testfunc()
{
#if defined(_MSC_VER)
	ClassDef addrClass(makeClass(0, "Address", (Address*)0));
#elif defined(__BORLANDC__)
	ClassDef addrClass(makeClass<Address>(0, std::string("Address")));
#else
	ClassDef addrClass(makeClass<Address>(0, "Address"));
#endif

	Address ad("Berner Strasse", "123",
			   "Luzern",
			   "", // region
			   "6001",
			   "Helvetia",
			   "041-4100000",
			   "nobody@irgendwo.ch");
	CppObject<Address> ado(&addrClass, ad);
	printXML(cout, ado);
	cout << endl;

	ClassDef addr2Class(0, "Address2", MapObject::newObject);
	addr2Class.addAttribute(Attribute("HouseName", Type::stringT));
	addr2Class.addAttribute(Attribute("City", Type::stringT));
	addr2Class.addAttribute(Attribute("Street", Type::stringT));
	addr2Class.addAttribute(Attribute("PCode", Type::stringT));
	addr2Class.addAttribute(Attribute("Country", Type::stringT));
	addr2Class.addAttribute(Attribute("Phone", Type::stringT));
	addr2Class.addAttribute(Attribute("EMail", Type::stringT));

	SimpleObject defAddr2(&addr2Class);
	defAddr2.setValue(0, RealValue<std::string>(""));	// HouseName, defaulted
	defAddr2.replaceValue(1, MapValue(&defAddr2, 2));	// City
	defAddr2.replaceValue(2, FuncValue<std::string>(&defAddr2, getDTDStreet));	// Street
	defAddr2.replaceValue(3, MapValue(&defAddr2, 4));	// PCode
	defAddr2.replaceValue(4, MapValue(&defAddr2, 5));	// Country
	defAddr2.replaceValue(5, MapValue(&defAddr2, 6));	// Voice
	defAddr2.replaceValue(6, MapValue(&defAddr2, 7));	// EMail

	addr2Class.setDefault(&defAddr2);

	MapObject mad(&addr2Class, &ado);
	mad.setPrivate(&ado);

	printXML(cout, mad);
	cout << endl;

	ClassDef phoneNoClass(0, "PhoneNumber", SimpleObject::newObject);
	phoneNoClass.addAttribute(Attribute("CountryCode", Type::stringT));
	phoneNoClass.addAttribute(Attribute("RegionCode", Type::stringT));
	phoneNoClass.addAttribute(Attribute("Number", Type::stringT));
	classPhoneNumber = &phoneNoClass;

	SimpleObject defPhone(&phoneNoClass);
	defPhone.replaceValue(0, FuncValue<std::string>(&defPhone, phoneCountry));
	defPhone.replaceValue(1, FuncValue<std::string>(&defPhone, phoneRegion));
	defPhone.replaceValue(2, FuncValue<std::string>(&defPhone, phoneNumber));
	phoneNoClass.setDefault(&defPhone);

	SimpleObject mphone(&phoneNoClass);

	ClassDef phoneClass(0, "Phone", SimpleObject::newObject);
	phoneClass.addAttribute(Attribute("PhoneNumber", Type::objectT));
	phoneObj = new SimpleObject(&phoneClass);

	phoneObj->setValue("PhoneNumber", RealValue<Object *>(&mphone));

	printXML(cout, *phoneObj);
	cout << endl;

	ClassDef addr3Class(0, "Address3", MapObject::newObject);
	addr3Class.addAttribute(Attribute("HouseName", Type::stringT));
	addr3Class.addAttribute(Attribute("City", Type::stringT));
	addr3Class.addAttribute(Attribute("Street", Type::stringT));
	addr3Class.addAttribute(Attribute("PCode", Type::stringT));
	addr3Class.addAttribute(Attribute("Country", Type::stringT));
	addr3Class.addAttribute(Attribute("Phone", Type::objectT));
	addr3Class.addAttribute(Attribute("Internet", Type::stringT));

	SimpleObject defAddr3(&addr3Class);
	defAddr3.setValue(0, RealValue<std::string>(""));	// HouseName, defaulted
	defAddr3.replaceValue(1, MapValue(&defAddr2, 2));	// City
	defAddr3.replaceValue(2, MapValue(&defAddr2, 0));	// Street
	defAddr3.replaceValue(3, MapValue(&defAddr2, 4));	// PCode
	defAddr3.replaceValue(4, MapValue(&defAddr2, 5));	// Country
	defAddr3.replaceValue(5, FuncValue<Object *>(&defAddr3, makePhone));	// Phone
	defAddr3.replaceValue(6, MapValue(&defAddr2, 7));	// EMail
	addr3Class.setDefault(&defAddr3);

	MapObject mad3(&addr3Class, &ado);
	mad3.setPrivate(&ado);

	printXML(cout, mad3);
	cout << endl;
}
