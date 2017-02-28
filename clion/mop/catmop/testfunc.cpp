// $Id: testfunc.cpp,v 1.5 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// testfunc.cpp
// Copyright (c) 2000, Detlef Vollmann

#include "testfunc.h"

#include <iostream>
#include <list>
#include <stdexcept>
#include <typeinfo>
#include "Class.h"
#include "SimpleObject.h"
#include "CppObject.h"
#include "RealValue.h"
#include "FuncValue.h"
#include "Product.h"
#include "Book.h"
#include "Cd.h"


using std::cout;
using std::endl;

ClassRegistry classReg;

void
testProductMaintenance()
{

    classReg.registerClass(new ClassDef(0, "Product", 0,
                                        Product::ownAttribBegin(),
                                        Product::ownAttribEnd()));

	ClassRegistry::Iterator base;
	base = classReg.find("Product");
	ClassDef::AttrIterator a;
	cout << "Product:" << endl;
	for (a = (*base)->attribBegin(); a != (*base)->attribEnd(); ++a)
	{
		cout << a->getName() << endl;
	}

#if defined(_MSC_VER)
	classReg.registerClass(new ClassDef(makeClass<Book>(*base, "Book", (Book*)0)));
	classReg.registerClass(new ClassDef(makeClass<Cd>(*base, "Cd", (Cd*)0)));
#elif defined(__BORLANDC__)
	classReg.registerClass(new ClassDef(makeClass<Book>(*base, std::string("Book"))));
	classReg.registerClass(new ClassDef(makeClass<Cd>(*base, std::string("Cd"))));
#else
	classReg.registerClass(new ClassDef(makeClass<Book>(*base, "Book")));
	classReg.registerClass(new ClassDef(makeClass<Cd>(*base, "Cd")));
#endif

	ClassRegistry::Iterator c;
	cout << "Classes:" << endl;
	for (c = classReg.begin(); c != classReg.end(); ++c)
	{
		cout << (*c)->getName() << endl;
	}

	ClassDef * tShirt = new ClassDef(*base, "T-Shirt", SimpleObject::newObject);
	tShirt->addAttribute(Attribute("Size", Type::stringT));
	tShirt->addAttribute(Attribute("Color", Type::stringT));
	tShirt->addAttribute(Attribute("Name", Type::stringT));
	tShirt->addAttribute(Attribute("Price", Type::doubleT));
	classReg.registerClass(tShirt);

	std::list<Attribute> attrL;
	attrL.push_back(Attribute("Make", Type::stringT));

	ClassDef * software = new ClassDef(*base, "Software", SimpleObject::newObject, attrL.begin(), attrL.end());
	software->addAttribute(Attribute("Title", Type::stringT));
	classReg.registerClass(software);

	cout << "Software:" << endl;
	for (a = software->attribBegin();
         a != software->attribBegin();
         ++a)
	{
		cout << a->getName() << endl;
	}

	ClassRegistry::Iterator book = classReg.find("Book");
	std::auto_ptr<Object> ecpp((*book)->newObject());

	cout << "ecpp attributes:" << endl;
	for (a = (*book)->attribBegin(); a != (*book)->attribEnd(); ++a)
	{
		cout << a->getName() << endl;
	}

	ecpp->setValue(5, RealValue<double>(22.50));
	try
	{
		ecpp->setValue(6, RealValue<std::string>("Scott Meyers"));
	}
	catch (std::bad_cast & e)
	{
#if defined(__BORLANDC__)
		// Borland's bad_cast isn't derived from std::exception!
		cout << "bad_cast for 6,string" << endl;
#else
		cout << "bad_cast for 6,string: " << e.what() << endl;
#endif
	}
	catch (std::exception & e)
	{ // VC++6 throws bad_cast instead of std::bad_cast.  But I don't know where it is declared
		cout << typeid(e).name() <<  " for 6,string: " << e.what() << endl;
	}
	try
	{
		ecpp->setValue(4, RealValue<double>(280));
	}
	catch (std::bad_cast & e)
	{
#if defined(__BORLANDC__)
		// Borland's bad_cast isn't derived from std::exception!
		cout << "bad_cast for 4,double" << endl;
#else
		cout << "bad_cast for 4,double: " << e.what() << endl;
#endif
	}
	catch (std::exception & e)
	{ // VC++6 throws bad_cast instead of std::bad_cast.  But I don't know where it is declared
		cout << typeid(e).name() <<  " for 4,double: " << e.what() << endl;
	}
	ecpp->setValue(0, RealValue<int>(23456));
	ecpp->setValue(2, RealValue<std::string>("Scott Meyers"));
	ecpp->setValue("Title", RealValue<std::string>("Effective C++"));
	ecpp->setValue(6, RealValue<double>(280));
	size_t idx;

	cout << "ecpp:" << endl;
	for (a = (*book)->attribBegin(); a != (*book)->attribEnd(); ++a)
	{
		cout << a->getName() << endl;
	}
	for (a = (*book)->attribBegin(), idx = 0;
         a != (*book)->attribEnd();
         ++a, ++idx)
	{
		cout << a->getName() << ": " << ecpp->getValue(idx).asString() << endl;
	}

	cout << ecpp->getValue("Author").asString() << endl;

	Book b("Bjarne Stroustrup", "The C++ Programming Language", "Addison-Wesley", 27.50, 370);
    CppObject<Book> mb(*book, b);
    Object * ob = &mb;
    cout << "C++ object through MOP" << endl;
	for (a = ob->instanceOf()->attribBegin(), idx = 0;
         a != ob->instanceOf()->attribEnd();
         ++a, ++idx)
	{
		cout << a->getName() << ": " << ob->getValue(idx).asString() << endl;
	}

}

std::string tShirtName(Object * ts)
{
	std::string name("T-Shirt ");
#if defined(_MSC_VER) || defined (__GNUC__)
	name += ts->getValue("Size").get(&name);
#else
	name += ts->getValue("Size").get<std::string>();
#endif
	name += " ";
#if defined(_MSC_VER) || defined (__GNUC__)
	name += ts->getValue("Color").get(&name);
#else
	name += ts->getValue("Color").get<std::string>();
#endif

	return name;
}

void
testShopCart()
{

	Book b("Bjarne Stroustrup", "The C++ Programming Language", "Addison-Wesley", 27.50, 370);
	Cd cd("George Gershwin", "Porgy and Bess", "London Philharmonic Orchestra", 21.90);

	Product *p = &b;

	cout << "Product Name: " << p->getName() << endl;
	cout << "Product Price: " << p->getPrice() << endl;
	cout << "Product Weight: " << p->getWeight() << endl;

	p = &cd;

	cout << "Product Name: " << p->getName() << endl;
	cout << "Product Price: " << p->getPrice() << endl;
	cout << "Product Weight: " << p->getWeight() << endl;

	ClassRegistry::Iterator tsT = classReg.find("T-Shirt");
	SimpleObject defTS(*tsT);
	defTS.replaceValue("Name", FuncValue<std::string>(&defTS, tShirtName));
	defTS.setValue(2, RealValue<std::string>("no Size"));
	(*tsT)->setDefault(&defTS);

	std::auto_ptr<Object> ts((*tsT)->newObject());
	ts->setValue(0, RealValue<int>(87654));
	ts->setValue(2, RealValue<std::string>("XXL"));
	ts->setValue("Color", RealValue<std::string>("red"));
	ts->setValue("Price", RealValue<double>(25.95));
	ts->setValue("Weight", RealValue<double>(387));

	for (size_t idx = 0; idx != 4; ++idx)
	{
		cout << ts->getValue(idx).asString() << endl;
	}

	DynaProduct dts(ts.get());
	p = &dts;

	cout << "Product Name: " << p->getName() << endl;
	cout << "Product Price: " << p->getPrice() << endl;
	cout << "Product Weight: " << p->getWeight() << endl;
}
