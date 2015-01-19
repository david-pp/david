#lang slideshow

(require "codify.rkt")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; 1.命名
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(slide
 (bt "1. 取个好名字"))

(slide
 #:title "1.1 原则:名副其实"
 (item "选名字是件严肃的事情,选个好名字很重要")
 (item "如果名字需要注释来补充,那就不是个好名字")
 (vl-append
  10
  (badcode "
int t = currentTime.elapse(e); // 消逝的时间,以毫秒计
...
if (t > timeout_value)
{
   Zebra::logger->debug(\"---一次循环用时 %u 毫秒-----\", t);
}
")
  (goodcode "
int elapsed_ms = currentTime.elapse(e);
...
if (elapsed_ms > timeout_value)
{
   Zebra::logger->debug(\"-----一次循环用时 %u 毫秒---\", elapsed_ms);
}
")))

(slide
 #:title "1.2 原则:避免误导"
 (item "必须避免留下掩藏代码本意的错误线索")
 (item "避免使用与本意相悖的词")
 (item "提防使用不同之处较小的名称")
 (item "拼写前后不一致就是误导")
 (vl-append
  10
  (badcode "
std::vector<int> account_list; // _list就是一个误导, accounts会更好

bool sendToZoneServer(); // 和下面的函数差别很小
bool sendToZoneServers(); // sendToAllZoneServers会好点
")))

(slide
 #:title "1.3 原则:做有意义的区分"
 (item "代码是写给人看的,仅仅是满足编译器的要求,就会引起混乱")
 (item "以数字系列命名(a1,a2,...),纯属误导")
 (item "无意义的废话: a, an, the, Info, Data")
 (vl-append 
  10
  (badcode "
void copy(char a1[], char a2[]) {
  for (size_t i = 0; a1[i] != '\0'; i++)
     a2[i] = a1[i];
}
")
  (goodcode "
void copy(char source[], char dest[]) {
  for (size_t i = 0; source[i] != '\0'; i++)
     dest[i] = source[i];
}
")))

(slide
 #:title "1.4 原则:使用可读的名字"
 (item "避免过度使用缩写")
 (item "可读的名字交流方便")
 (item "BAD: class XLQY, class FCNV, class LTQManager"))

(slide
 #:title "1.5 原则:使用可搜索的名字"
 (item "避免使用Magic Number")
 (item "避免使用单字母,或出现频率极高的短字母组合(注意度的把握)")
 (vl-append
  10
  (badcode "
if (obj->base->id == 4661) // 4661是啥玩意?
{
   usetype = Cmd::XXXXXXX;
}

int e; // 怎么查找?
XXXX:iterator it; // 变量作用的范围比较大的时候,也不见得是个好名字
")
  (goodcode "

#define OJBECT_FEEDBACK_CARD 4661

if (OJBECT_FEEDBACK_CARD == obj->base->id)
{
   usetype = Cmd::XXXXXXX;
}
")))

(slide
 #:title "1.6 原则:避免使用编码"
 (item "匈牙利标记法"
       (subitem "Windows API时代留下的玩意")
       (subitem "wdXX, dwXXX, strXXX")
       (subitem "变量类型变换导致名不副实"))
 (item "成员前缀"
       (subitem "m_name, m_xxx")
       (subitem "基本上都无视,为何要多次一举"))
 (item "接口和实现"
       (subitem "IXXX, I-接口修饰前缀")
       (subitem "CXXX, C-类修饰前缀")
       (subitem "这些修饰多数时候都是废话")))

(slide
 #:title "1.7 原则:名字尽量来自解决方案领域或问题领域"
 (item  "使用解决方案领域名称 "
        (subitem "都出自CS,术语、算法名、模式名、数学术语尽管用。")
        (subitem "如AccountVisitor,Visitor模式实现的Account类"))
 (item "使用问题领域的名称"
       (subitem "我们代码里面多数都是这些名称,不明白找策划问问")))

(slide
 #:title "1.8 原则:适当使用有意义的语境"
 (item "良好命名的类、函数、名称空间来放置名称,给读者提供语境")
 (item "只有两三个变量,给名称前加前缀")
 (item "事不过三,变量超过三个考虑封装成概念,添加struct或class")
 (vl-append
  10
  (badcode "
// 看着整齐?使用方便?
DWORD love_ensure_type_;  //当前的爱情保险类型
DWORD love_ensure_ret_; //购买爱情保险回应标示
DWORD love_ensure_total_; //现在已经盖章数目
DWORD love_ensure_..._;  //...
DWORD love_ensure_..._;  //...
")))

(slide
 #:title "我们的原则:"
 (item "名副其实")
 (item "避免误导")
 (item "做有意义的区分")
 (item "使用可读的名字")
 (item "使用可搜索的名字")
 (item "避免使用编码")
 (item "名字尽量来自问题/解决方案领域")
 (item "适当使用有意义的语境"))

(slide
 #:title "我们的规范:"
 (item "文件名"
       (subitem "首字母大写,多个词组合起来")
       (subitem "如: SceneUser.h Sept.h"))
 (item "类名/名称空间名"
       (subitem "首字母大写，多个词组合起来")
       (subitem "使用名词或名词词组")
       (subitem "避免使用C前缀，如:CSept")
       (subitem "如: SceneUser SeptWar")))

(slide
 #:title "我们的规范:"
 (item "函数名"
       (subitem "首字母小写")
       (subitem "使用动词或动词词组")
       (subitem "避免使用孤立的全局函数,可以封装在类或名称空间里面")
       (subitem "get, set, is前缀的使用")
       (subitem "如: fuckYou(), levelup()"))
 (item "变量名"
       (subitem "全部字母小写,多个词以下划线分隔")
       (subitem "私有成员变量加后缀\"_\",公有变量不用")
       (subitem "避免使用孤立的全局变量,可以封装在类或名称空间里面")
       (subitem "如: quest_id, questid_")))
