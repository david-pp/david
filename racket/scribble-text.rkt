#lang scribble/text

@(define PI 3.14)
@(define (3x x)
   (add-between (list x x x) " "))

@(define (greater? x)
   (if (> x 0)
       "true"
       "false"))

Programming languages should
be designed not by piling
feature on top of feature, but
blah blah blah.@PI

@(if (> 2 0) @list{true} "false ")

@(cond
   [(> 2 0) "2>0"]
   [(> 3 0) "3>0"])

@3x{fuckyou}

@greater?[10]



@include["scribble-included.rkt"]
