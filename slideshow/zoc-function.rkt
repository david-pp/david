#lang slideshow

(require "codify.rkt")

(current-font-size 25)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; 2.函数
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(slide
 (chapter "2. 函数:过程的抽象"))

(slide
 #:title "2.1 原则:取个描述性的名字"
 (item "取个一眼就看出函数本意的名字很重要")
 (item "长而具有描述性的名称，要比短而让人费解的好")
 (item "使用动词或动词+名词短语"))

(slide
 #:title "2.2 原则:保持参数列表的简洁"
 (item "无参数最好，其次一元，再次二元，三元尽量避免")
 (item "尽量避免标识参数")
 (item "使用参数对象")
 (item "参数列表")
 (item "避免输出和输入混用，无法避免则输出在左，输入在右")
  (goodcode "
bool isBossNpc();
void summonNpc(int id);
void summonNpc(int id, int type);
void summonNpc(int id, int state, int type); // 还能记得参数顺序吗？

void showCurrentEffect(int state, bool show); // Bad!!!
void showCurrentEffect(int state); // Good!!
void hideCurrentEffect(int state); // 新加个函数也没多难吧？

bool needWeapon(DWORD skillid, BYTE& failtype); // Bad!!!
"))

(slide
 #:title "2.3 原则:保持函数短小"
 (item "现状：有些函数我们得按米来度量")
 (item "第一规则：要短小")
 (item "第二规则：还要更短小")
 (item "一屏之地，一览无余"))

(slide
 #:title "2.4 原则:只做一件事"
 (item "函数应该只做一件事，做好这件事")
 (item "只做这一件事"))

(slide
 #:title "2.5 原则:每个函数位于同一抽象层级"
 (item "要确保函数只做一件事，函数中的语句都要在同一个抽象层级上")
 (item "自顶下下读代码"))

(slide
 #:title "2.6 原则:无副作用"
 (item "谎言，往往名不副实"))

(slide
 #:title "2.7 原则:操作和检查要分离"
 (item "要么是做点什么，要么回答点什么，但二者不可兼得")
 (item "混合使用---副作用的肇事者"))

(slide
 #:title "2.8 原则:使用异常来代替返回错误码"
 (item "操作函数返回错误码轻微违法了操作与检查的隔离原则")
 (item "用异常在某些情况下会更好点")
 (item "抽离try-cacth")
 (item "错误处理也是一件事情，也应该封装为函数")
 (goodcode "
bool RedisClient::connect(const std::string& host, uint16_t port)
{
	this->host = host;
	this->port = port;
	this->close();
	
	try 
	{
		redis_cli = new redis::client(host, port);
		return true;
	}
	catch (redis::redis_error& e) 
	{
		redis_cli = NULL;
		std::cerr << \"error:\" << e.what() << std::endl;
		return false;
	}

	return false;
}
"))

(slide
 #:title "2.9 原则:减少重复代码"
 (colorize (t "重复是一些邪恶的根源！！！") "red"))

(slide
 #:title "2.10 原则:避免丑陋不堪的switch-case"
 (item "天生要做N件事情的货色")
 (item "多次出现就要考虑用多态进行重构")
 (ht-append 
  10
  (badcode "
bool saveBinary(type, data) {
   switch (type) {
     case TYPE_OBJECT:
           ....
          break;
     case TYPE_SKILL:
           ...
          break;
     ....
   }
}
bool needSaveBinary(type) {
   switch (type) {
     case TYPE_OBJECT:
          return true;
     case TYPE_SKILL:
           ...
          break;
     ....
   }
}
")
  (goodcode "

class BinaryMember
{
  BinaryMember* createByType(type){
   switch (type) {
     case TYPE_OBJECT:
          return new ObjectBinaryMember;
     case TYPE_SKILL:
          return new SkillBinaryMember;
     ....
  }

  virtual bool save(data);
  virtual bool needSave(data);
};

class ObjectBinaryMember : public BinaryMember
{
   bool save(data){
       ....
   }
   bool needSave(data){
       ....
   }
};")))


(slide
 #:title "我们的原则:"
 (item "使用描述性的名字")
 (item "保持参数列表的整洁和清晰")
 (item "短小")
 (item "只做一件事，并把它做好")
 (item "每个函数位于同一抽象层")
 (item "无副作用")
 (item "操作和检查要分离")
 (item "使用异常来代替返回错误码")
 (item "避免丑陋不堪的switch-case")
 (item "避免重复"))

(slide
 #:title "我们的规范:"
 (item "命名：动词/动词词组，首字母小写")
 (item "参数：无特殊原因3个以内,输出在左，输入在右")
 (item "函数体：一屏之地，一览无余"))


(slide
 #:title "我们应该这样做:"
 (item "添加新函数"
       (subitem "刚下手时违反规范和原则没关系")
       (subitem "开发过程中逐步打磨")
       (subitem "保证提交后的代码是整洁的即可"))
 (item "重构已有函数，见到一个消灭一个"
       (subitem "冗长而复杂")
       (subitem "有太多缩进和嵌套循环")
       (subitem "参数列表过长")
       (subitem "名字随意取")
       (subitem "重复了三次以上")))

