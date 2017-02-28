// $Id: MTag.h,v 1.3 2000/06/05 15:01:23 dv Exp $
// Copyright (c) 2000, Detlef Vollmann.  Permission to copy, use, modify,
// sell and distribute this software is granted provided this copyright
// notice appears in all copies.

// This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

// See http://www.vollmann.ch/ for most recent version.

// MTag.h
// Copyright (c) 2000, Detlef Vollmann
#ifndef MTAG_H_
#define MTAG_H_
#include <string>
#include <iostream>

class MTag
{
public:
	MTag(std::ostream &o, std::string const & tag, std::string const & contents = "")
	 : os(o), tagName(tag)
	{
		os << '<' << tag << '>' << contents;
	}
	~MTag()
	{
		os << "</" << tagName << '>' << std::endl;
	}

private:
	std::ostream &os;
	const std::string tagName;
};
#endif // MTAG_H_
