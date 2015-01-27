#lang slideshow

(require "codify.rkt")

(current-font-size 25)

(slide
 (chapter "4. 类"))

(slide
 #:title "4.1 原则：统一的布局"
 (ht-append 20
            (para #:width 51 "布局原则："
                  "1.static和typedef放在最前面"
                  "2.构造和析构"
                  "3.使用频率较高的通用函数"
                  "4.根据功能分段")
            (goodcode "
class Sept
{
public:
   typedef std::map<int,int> ObjectMap;
   const float PI = 3.14;

   static int getCounter();
   static int counter;

   Sept();
   ~Sept();
  
   // 使用频率较高的通用函数
   int getSeptMaster();
   void setSeptMoney(int money);

   // XXX功能
   void doXXCmd();
   void giveXXAward();

   // YYY功能
   void doYYCmd();
   void giveYYAward();

protected:
   ...
private:
   int privateFunction();
   int private2Function();
  
   int member_;
};
")))

(slide #:title "4.2 原则：保持短小"
       (item "SRP：单一职责"
             (subitem "系统应该由许多短小的类组成")
             (subitem "每个小类封装一个职责")
             (subitem "只有一条修改的理由"))
       (item "高内聚"
             (subitem "类应该只有数量实体变量")
             (subitem "每个方法都应该操作一个或多个这种变量")))

(slide #:title "4.3 原则：为修改做准备（面向对象五大原则）"
       (item "1. SRP：单一职责原则"
             (subitem "Single Responsibility Principle")
             (subitem "一个类应该仅有一个引起它变化的原因"))
       (item "2. OCP：开放封闭原则"
             (subitem "Open Close Principle")
             (subitem "为扩展开放、为修改关闭"))
       (bitmap "img/ocp.jpg"))

(slide #:title "4.3 原则:为修改做准备(面向对象五大原则)"
       (item "3. DIP:依赖倒置原则"
             (subitem "Dependency Inverse Principle")
             (subitem "逆转高层依赖底层"))
       (ht-append 10
                  (bitmap "img/dip-1.png")
                  (bitmap "img/dip-2.png")))

(slide #:title "4.3 原则:为修改做准备(面向对象五大原则)"
       (item "4. LSP:Liskov替换原则"
             (subitem "Liskov Substitution Principle")
             (subitem "子类可以替换父类并且出现在父类能够出现的地方")
             (subitem "面向接口编程"))
       (bitmap "img/lsp.png"))

(slide #:title "4.3 原则:为修改做准备(面向对象五大原则)"
       (item "5. ISP:接口隔离原则"
             (subitem "Interface Isolation Principle")
             (subitem "使用多个专门的接口比使用单个接口要好的多"))
       (ht-append 10 
                  (scale (bitmap "img/isp-1.jpg") 0.8)
                  (scale (bitmap "img/isp-2.jpg") 0.8)))

(slide #:title "我们的原则："
       (item "统一布局")
       (item "保持短小")
       (item "面向对象五大原则："
             (subitem "SRP")
             (subitem "OCP")
             (subitem "DIP")
             (subitem "LSP")
             (subitem "ISP")))

