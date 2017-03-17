#include <iostream>
#include <cstring>

//
// Syntax vs. Semantic
//

// Lifecycle of Object:
//  - Construct
//  - Copy/Move (Construct, Assignment)
//  - Cleanup

// STL Support Move Semantic


// A default constructor: X()
// A copy constructor: X(const X&)
// A copy assignment: X& operator=(const X&)
// A move constructor: X(X&&)
// A move assignment: X& operator=(X&&)
// A destructor: ~X()

class Y;

class X {
    X(Y); // ‘‘ordinar y constructor’’: create an object
    X(); // default constructor
    X(const X&); // copy constr uctor
    X(X&&); // move constructor
    X& operator=(const X&); // copy assignment: clean up target and copy
    X& operator=(X&&); // move assignment: clean up target and move
    ~X(); // destructor: clean up
};

int main() {
}
