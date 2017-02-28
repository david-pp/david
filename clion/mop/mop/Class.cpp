// $Id: Class.cpp,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Class.cpp
// Copyright (c) 2000, Detlef Vollmann

#include <iterator>
#include "Class.h"
#include "Object.h"
#include "Attribute.h"
#include "Value.h"

ClassDef::ClassDef(ClassDef const * base, const std::string & name_, CreateObjFunc objFunc)
 : baseClass(base), name(name_), createObj(objFunc), definitionFix(false),
   defaultObj(0)
{
	baseInit();
}

std::string ClassDef::getName() const
{
	return name;
}

Object * ClassDef::newObject() const
{
	definitionFix = true;
	return (*createObj)(this);
}

Object const * ClassDef::defObject() const
{
	return defaultObj;
}

void ClassDef::setDefault(Object const * def)
{
	defaultObj = def;
}

ClassDef::AttrIterator ClassDef::attribBegin() const
{
	return effectiveAttributes.begin();
}

ClassDef::AttrIterator ClassDef::attribEnd() const
{
	return effectiveAttributes.end();
}

Attribute const & ClassDef::getAttribute(size_t idx) const
{
	return effectiveAttributes[idx];
}

void ClassDef::addAttribute(const Attribute & a)
{
	if (!definitionFix)
	{
		ownAttributes.push_back(a);
		effectiveAttributes.push_back(a);
	}
}

size_t ClassDef::getAttributeCount() const
{
	return effectiveAttributes.size();
}

size_t ClassDef::findAttribute(std::string const & name) const
{ // this does a reverse search to find the latest override
	for (AttributeContainer::const_reverse_iterator i = effectiveAttributes.rbegin();
	     i != effectiveAttributes.rend();
		 ++i)
	{
		if (i->getName() == name)
		{
            // both returns might be somewhat confusing
            // i.base() does not point to the same attribute as i
            // due to reverse iterator characteristics,
            // so the resulting distance must be adjusted by one:
            // return std::distance(effectiveAttributes.begin(), i.base()) - 1;

            // rend() like end() points to one item beyond the last,
            // the the resulting distance must be adjusted as well:
			return std::distance(i, effectiveAttributes.rend()) - 1;
		}
	}
	return getAttributeCount();
}

void ClassDef::baseInit()
{
	if (baseClass)
	{
		baseClass->definitionFix = true;
		std::copy(baseClass->attribBegin(), baseClass->attribEnd(),
                  std::back_inserter<AttributeContainer>(effectiveAttributes));
	}
}

void ClassRegistry::registerClass(ClassDef * cl)
{
	classes.push_back(cl);
}

ClassRegistry::Iterator ClassRegistry::begin()
{
	return classes.begin();
}

ClassRegistry::Iterator ClassRegistry::end()
{
	return classes.end();
}

ClassRegistry::Iterator ClassRegistry::find(std::string const & name)
{
	for (Iterator c = classes.begin(); c != classes.end(); ++c)
	{
		if ((*c)->getName() == name)
		{
			return c;
		}
	}
	return classes.end();
}

ClassRegistry::Iterator ClassRegistry::noClass()
{
	return classes.end();
}

