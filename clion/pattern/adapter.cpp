#include  <iostream>

using namespace std;

typedef int Coordinate;
typedef int Dimension;

// Desired interface
class Rectangle {
public:
    virtual void draw() = 0;
};

// Legacy component
class LegacyRectangle {
public:
    LegacyRectangle(Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2) {
        x1_ = x1;
        y1_ = y1;
        x2_ = x2;
        y2_ = y2;
        cout << "LegacyRectangle:  create.  (" << x1_ << "," << y1_ << ") => ("
             << x2_ << "," << y2_ << ")" << endl;
    }

    void oldDraw() {
        cout << "LegacyRectangle:  oldDraw.  (" << x1_ << "," << y1_ <<
             ") => (" << x2_ << "," << y2_ << ")" << endl;
    }

private:
    Coordinate x1_;
    Coordinate y1_;
    Coordinate x2_;
    Coordinate y2_;
};

// Class Adapter wrapper
class RectangleAdapter : public Rectangle, private LegacyRectangle {
public:
    RectangleAdapter(Coordinate x, Coordinate y, Dimension w, Dimension h) :
            LegacyRectangle(x, y, x + w, y + h) {
        cout << "RectangleAdapter: create.  (" << x << "," << y <<
             "), width = " << w << ", height = " << h << endl;
    }

    virtual void draw() {
        cout << "RectangleAdapter: draw." << endl;
        oldDraw();
    }
};

// Object Adpater wrappter
class RectangleAdapter2 : public Rectangle {
public:
    RectangleAdapter2(Coordinate x, Coordinate y, Dimension w, Dimension h)
            : adaptee_(x, y, x + w, y + h) {
        cout << "RectangleAdapter2: create.  (" << x << "," << y <<
             "), width = " << w << ", height = " << h << endl;
    }

    virtual void draw() {
        cout << "RectangleAdapter2: draw." << endl;
        adaptee_.oldDraw();
    }

private:
    LegacyRectangle adaptee_;
};

int main() {
    // class mode
    {
        Rectangle *r = new RectangleAdapter(120, 200, 60, 40);
        r->draw();
    }

    // object
    {
        Rectangle *r = new RectangleAdapter2(120, 200, 60, 40);
        r->draw();
    }
}