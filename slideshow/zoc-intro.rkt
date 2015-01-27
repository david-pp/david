#lang slideshow

(require "codify.rkt")

(slide
 (t "代码之道-整洁的代码")
 (tt "by David"))

(slide
 #:title "谨记:代码是写给人看的"
 (bitmap "img/cleancode.png"))

(slide
 #:title "推荐阅读"
 (item "《代码整洁之道》")
 (item "《编写可读代码的艺术》")
 (item "《重构:改善既有代码的设计》"))

(slide
 (t "0 代码就像我们的孩子"))

(slide
 #:title "0.1 代码永存、混乱当道"
 (item "混乱的制造者"
       (subitem "规范的缺失")
       (subitem "程序员作茧自缚"))
 (item "混乱的代价"
       (subitem "新人如坠云雾")
       (subitem "添加新特性,需要披荆斩棘")
       (subitem "修改BUG,如同大海捞针")))

(slide
 #:title "0.2 我们的态度"
 (item "保持代码的整洁")
 (item "Make It Better and Better ...")
 (item "改变“只要代码能跑就不要去动它的”想法"))

(slide
 #:title "0.3 Clean Code"
 (t "KISS:风格统一、代码逻辑简洁明了,直截了当!")
 (t "让重复代码见鬼去吧!"))