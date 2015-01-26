#lang slideshow

(require "codify.rkt")

(current-font-size 25)

(slide
 (chapter "3.数据的抽象:结构体VS.对象"))

(slide
 #:title "3.1 数据抽象"
 (item "结构体(struct实例):"
       (subitem "暴露数据实现"))
 (item "对象(class实例):"
       (subitem "隐藏数据实现")
       (subitem "暴露行为接口"))
 (ht-append 10
            (goodcode "
struct Point {
   int x;
   int y;
};
")
            (goodcode "

class Point {

public:
     int getX();
     int getY();
     void setByCartesion(int x, int y);

     int getR();
     int getTheta();     
     void setByPolor(int r, int theta);
// God knows the following.. who cares?    
private:
     int x_; 
     int y_;
};

")))

(slide
 #:title "3.2 结构体 VS. 对象"
 (item "反对称性"
       (subitem "结构体:容易添加新函数,缺难于修改数据")
       (subitem "对象:容易添加数据,缺难于添加新函数"))
  (item "思考:"
        (subitem "Shape增加一个计算周长的函数?")
        (subitem "Shape增加一种新类型?"))
 (ht-append 10
            (goodcode "
struct Shape
{
   int type;
   union {
      Point  rect;
      double radius;
   };
};

double caclArea(Shape* shape)
{
  switch (shape->type)
  {
      case RECT:   
        return shape->rect.width*shape->rect.height;
      case CIRCLE: 
        return shape->radius*shape->radius*3.14;
  }
}

")
            (goodcode "

class Shape {
public:
   virtual double caclArea();
};

class Squre : public Shape {
public:
   double caclArea(){ return width_ * height_;}
private:
   double width_;
   double height_:
};

class Circle : public Shape {
public:
   double caclArea(){ return 3.14*radius_* radius_;}
private:
   double radius_;
};

")))
 
(slide
 #:title "3.3 原则:结构体和对象的选择"
 (item "结构体"
       (subitem "结构相对固定")
       (subitem "行为变化较大")
       (subitem "如:Point, Time, ..."))
 (item "对象" 
       (subitem "行为相对固定")
       (subitem "经常会添加新类型"))
 (item "避免混合使用"))

(slide
 #:title "3.4 原则:不要和陌生人说话"
 (item "德墨忒尔律:模块不应了解它所操作的内部情形")
 (item "类C的方法f只应该调用以下对象的方法:"
       (subitem "C")
       (subitem "由f创建的对象")
       (subitem "作为参数传递给f的对象")
       (subitem "由C的实体变量持有的对象"))
 (item "目的"
       (subitem "信息隐藏")
       (subitem "避免修改时引起多处修改")))

(slide
 #:title "3.5 原则:数据传输对象/POD"
 (item "POD:Plain Of Data")
 (item "用于消息传递")
 (item "用于存档"))

(slide
 #:title "我们的原则:"
 (item "结构体和对象的选择遵循数据和行为的变化")
 (item "不要和陌生人说话")
 (item "POD的使用"))
