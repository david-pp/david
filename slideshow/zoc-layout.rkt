#lang slideshow

(require "codify.rkt")

(current-font-size 25)

(slide
 (cc-superimpose (bitmap "img/paper.jpg")
                 (chapter "6. 格式——错落有致")))

(slide
 #:title "6.1 原则:像写文章一样写代码"
 (item "人如其文，代码同此理")
 (item "整洁一致的代码赏心悦目，宾至如归")
 (item "格式不统一的代码，总是无法让人淡定")
 (item "代码之美：能工作vs.可读性"))
 

(slide #:title "6.2 垂直格式"
       (item "垂直段落"
             (subitem "靠近：语义相近的靠近")
             (subitem "间隔：不相近的以空行隔开"))
       (item "垂直距离"
             (subitem "变量声明应该靠近使用的位置")
             (subitem "相关函数放在一起")
             (subitem "概念相关的语句放在一起"))
       (item "垂直顺序"
             (subitem "自顶而下")
             (subitem "遵循函数调用依赖的顺序：被调用的放在下面")))
             

(slide #:title "6.3 横向格式"
       (item "缩进：有效区分代码块和作用域")
       (item "空格：挤在一起看着别扭")
       (item "换行：一行尽量不要太长")
       (item "垂直对齐：无所谓"))

(slide #:title "我们的原则:"
       (colorize (para "像写文章一样来些代码，讲究段落层次，错落有致！") "red"))

