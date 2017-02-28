// $Id: DynaObject.h,v 1.3 2000/06/05 15:01:22 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// DynaObject.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef DYNAOBJECT_H
#define DYNAOBJECT_H

#include "Object.h"
#include "Value.h"
#include <vector>

class DynaObject : public Object
{
public:
	DynaObject(ClassDef const *);
	DynaObject(DynaObject const &);
	~DynaObject();

#if defined(__BORLANDC__)
	virtual Value getValue(std::string const & name) const { return Object::getValue(name); }
	virtual void setValue(std::string const & name, Value const & v) { Object::setValue(name, v); }
#else
	using Object::getValue;
	using Object::setValue;
#endif
	virtual Value getValue(size_t idx) const;
	virtual void setValue(size_t idx, Value const &);
	virtual void replaceValue(size_t idx, BaseValue const &);
	void replaceValue(std::string const & name, BaseValue const &);

	Object * privObject() const;
	void setPrivate(Object *);

protected:
	virtual BaseValue * getBaseValue(size_t idx);
	virtual BaseValue const * getBaseValue(size_t idx) const;
	void init(DynaObject * self);
	void copy(DynaObject * self, DynaObject const & rhs);

private:
	typedef std::vector<BaseValue *> ValueContainer;

	DynaObject & operator=(DynaObject const &); // forbidden
	ValueContainer values;
	Object * privateObj;
};
#endif // DYNAOBJECT_H
