#lang slideshow

(require "codify.rkt")

(current-font-size 25)

(slide
 (chapter "5. 注释")
 (bitmap "img/good-bad.jpg"))

(slide
 #:title "5.1 原则:别给糟糕的代码加注释，重新写吧！"
 (item "注释的“好”与“恶”"
       (subitem "什么也比不上放置良好的注释有用")
       (subitem "什么也不会比乱七八糟的注释更有本事搞乱一个模块")
       (subitem "什么也不会比陈旧、提供错误信息的注释更有破坏性"))
 (item "通常注释意味着什么"
       (subitem "失败的表达")
       (subitem "没有几个人修改代码的时候会去及时完善注释")
       (subitem "不能美化糟糕的代码，只会让人更恶心")))

(slide #:title "5.2 原则:保留好的注释"
       (item "法律信息（CopyRight...）")
       (item "对意图的解释（特别是一些复杂的逻辑）")
       (item "警告（一些影响运行或者其它类似的信息）")
       (item "TODO注释（临时性的注释，将来要整理的代码）"))

(slide #:title "5.3 原则:拒绝坏的注释"
       (item "喃喃自语，废话连篇（不要把看代码的人当成超级小白）")
       (item "误导性的注释（不要也罢，但不能出来害人啊！）")
       (item "形式化的注释（完全就是浪费时间和生命）")
       (item "括号后面的注释（意味你的函数太庞大了)")
       (item "归属或签名（SVN知道你是谁，不要留太多名）")
       (item "注释掉的代码（或许永远都不会用到，交给SVN吧）"))



(slide #:title "我们的原则:"
       (item "别给糟糕的代码加注释，重新写吧！")
       (item "保留好的注释")
       (item "拒绝坏的注释")
       (item "PS.每当写下一行注释的时候，都要思考下："
             (subitem "是不是名字去的不好？")
             (subitem "是不是函数太过庞大了？算法太复杂了？")
             (subitem "是不是放错位置的了？")
             (subitem "...")))

