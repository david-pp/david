#lang slideshow

(require "codify.rkt")

(define code-c "
static inline char *duplicateStringValue(const char *value,
                                         unsigned int length = unknown) {
  if (length == unknown)
    length = (unsigned int)strlen(value);

  // Avoid an integer overflow in the call to malloc below by limiting length lenth.....
  // to a sane value.
  if (length >= (unsigned)Value::maxInt)
    length = Value::maxInt - 1;

  char *newString = static_cast<char *>(malloc(length + 1));
  JSON_ASSERT_MESSAGE(newString != 0,
                      \"in Json::Value::duplicateStringValue(): \"
                      \"Failed to allocate string value buffer\");
  memcpy(newString, value, length);
  newString[length] = 0;
  return newString;
}")

(slide
 (frame (codify code-c))
 (frame (codify code-c)))


(slide
 (t "代码之道-整洁的代码")
 (tt "by David"))

(slide
 #:title "谨记：代码是写给人看的"
 (bitmap "./cleancode.png"))

(slide
 (t "0 代码就像我们的孩子"))

(slide
 #:title "0.1 代码永存、混乱当道"
 (item "混乱的制造者"
       (subitem "规范的缺失")
       (subitem "程序员作茧自缚"))
 (item "混乱的代价"
       (subitem "新人如坠云雾")
       (subitem "添加新特性，需要披荆斩棘")
       (subitem "修改BUG，如同大海捞针")))

(slide
 #:title "0.2 我们的态度"
 (item "保持代码的整洁")
 (item "Make It Better and Better ...")
 (item "改变“只要代码能跑就不要去动它的”想法"))

(slide
 #:title "0.3 Clean Code"
 (t "KISS：风格统一、代码逻辑简洁明了，直截了当！")
 (t "让重复代码见鬼去吧！"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; 1.命名
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(slide
 (bt "1. 取个好名字"))

(slide
 #:title "1.1 名副其实"
 (t "good VS. evil")
 (hc-append
  10
  (scale-to-fit (bitmap "./cleancode.png") 500 500)
  (scale-to-fit (bitmap "./cleancode.png") 500 500))
 (item "XXXXXXXXXXXX")
 (item "XXXXXXXXXXXX"))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; 2.函数
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(slide
 (bt "1. 函数：过程的抽象"))
