// $Id: Class.h,v 1.4 2000/09/28 03:22:36 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// Class.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef CLASS_H
#define CLASS_H

#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include "Attribute.h"

class Object;

class ClassDef
{
	typedef std::vector<Attribute> AttributeContainer;
public:
	typedef AttributeContainer::const_iterator AttrIterator;
	typedef Object * (*CreateObjFunc)(ClassDef const *);

	template <typename Iterator>
	ClassDef(ClassDef const * base, const std::string & name_, CreateObjFunc objFunc,
			 Iterator attribBegin, Iterator attribEnd)
	 : baseClass(base), name(name_),
#ifndef _MSC_VER
// MS VC++ doesn't ship with a vector constructor that takes arbitrary iterators
	   ownAttributes(attribBegin, attribEnd),
#endif // _MSC_VER
	   createObj(objFunc), definitionFix(false),
	   defaultObj(0)
	{
#ifdef _MSC_VER
		std::copy(attribBegin, attribEnd, std::back_inserter<AttributeContainer>(ownAttributes));
#endif // _MSC_VER
		baseInit();
		effectiveAttributes.insert(effectiveAttributes.end(), ownAttributes.begin(), ownAttributes.end());
	}

	ClassDef(ClassDef const * base, const std::string & name_, CreateObjFunc objFunc);

	std::string getName() const;
	Object * newObject() const;
	Object const * defObject() const;
	void setDefault(Object const *);
	AttrIterator attribBegin() const;
	AttrIterator attribEnd() const;
	Attribute const & getAttribute(size_t idx) const;
	void addAttribute(const Attribute &);
	size_t getAttributeCount() const;
	size_t findAttribute(std::string const & name) const;

private:
	void baseInit();
	ClassDef const * const baseClass;
	const std::string name;
	AttributeContainer ownAttributes, effectiveAttributes;
	const CreateObjFunc createObj;
	mutable bool definitionFix;
	Object const * defaultObj;
};

class ClassRegistry
{
	typedef std::list<ClassDef *> Container;
public:
	typedef Container::iterator Iterator;

	void registerClass(ClassDef * cl);
	Iterator begin();
    Iterator end();
	Iterator find(std::string const & name);
	Iterator noClass();

private:
	Container classes;
};

#endif // CLASS_H
