// $Id: main.cpp,v 1.1.1.1 2000/09/28 03:16:37 dv Exp $

#include <string>
#include "MemPtrNew.h"

class Product
{
public:
	double price;
};

class Book : public Product
{
public:
	std::string author;
};

int main()
{
	MemPtr<Book> mp1(&Book::author);
	MemPtr<Book> mp2(&Product::price);
	return 1;
}