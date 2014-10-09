#!/bin/guile -s
!#

;;; how to run ? 
;;; 1>./sicp-procedure.scm
;;; 2> guile -s sicp-procedure.scm

(define (show desc value)
	(display desc)
	(display " : ")
	(display value)
	(newline))

(define (my-abs x)
	(cond 
		((> x 0) x)
		((= x 0) 0)
		((< x 0) (- x))))

(define (my-abs2 x)
	(if (< x 0) 
		(- x) 
		x ))

(show "my-abs" (my-abs 100))
(show "my-abs" (my-abs 0))
(show "my-abs" (my-abs -100))

(show "my-abs2" (my-abs2 100))
(show "my-abs2" (my-abs2 0))
(show "my-abs2" (my-abs2 -100))


;;;
;;; Reference: 
;;; http://people.csail.mit.edu/jaffer/r5rs_toc.html
;;;

;;; 4.1 Primitive expression types

;;; 4.1.1 Variable references
(define x 28)
(show "Variable" x)

;;; 4.1.2 Literal expressions
(show "quote" (quote (a b c)))
(show "quote" 'a)
(show "quote" '(a b c))

;;; 4.1.4 lambda express
(show "lambda" ((lambda (x) (+ x x)) 4))

(define add4
	(let ((x 4))
		(lambda (y) (+ x y))))

(show "lambda" (add4 5))
(show "lambda" ((lambda (x y . z) z) 3 4 5 6 7 8 9)) ;; z is a list

;;; 4.1.5 Conditionals
(show "if" (if (> 3 2) 'yes 'no))
(show "if" (if (> 2 2) 'yes 'no))
(show "if" (if (> 3 2) (- 3 2) (+ 3 2)))

;;; 4.1.6 Assignments
(define x 2)
(set! x 4)
(show "set!" x)

;;; 4.2 Derived expression types

;;; 4.2.1 Conditionals
(show "cond"  
	(cond 
		((> 3 3) 'greater)
		((< 3 3) 'less)
		(else 'equal)))

(show "case"
	(case (car '(a d)) 
		((a e i o u) 'vowel')
		((w y) 'semivowel')
		(else 'consonant)))
