#lang racket

;; http://www.scheme.com/tspl4/further.html

(call/cc
 (lambda (k)
   (* 5 4)))

(call/cc
 (lambda (k)
   (* 5 (k 4))))

(+ 2
   (call/cc
    (lambda (k)
      (* 5 (k 4)))))

;;
;; nonlocal exit from recursion
;;
(define (product ls)
  (call/cc
   (lambda (break)
     (let f ([ls ls])
       (display "*\n")
       (cond
         [(null? ls ) 1]
         [(= (car ls) 0) (break 0)]
         [else (* (car ls) (f (cdr ls)))])))))

(product '(1 2 3 4 5))
(product '(1 2 0 4 5))

(let ([x (call/cc (lambda (k) k))])
  (display x)
  (x (lambda (ignore) "hi")))

(((call/cc (lambda (k) k)) (lambda (x) x)) "HEY!")

;;
;; pass the continuation a different value
;;

(define retry #f)

(define (factorial x)
  (if (= x 0)
      (call/cc 
       (lambda (k)
         (set! retry k) 1))
      (* x (factorial (- x 1)))))

(factorial 3)
(retry 1)
(retry 2)
(retry 5)


;;
;; multitasking
;; 
(define lwp-list '())
(define lwp
  (lambda (thunk)
    (set! lwp-list (append lwp-list (list thunk)))))

(define start
  (lambda ()
    (let ([p (car lwp-list)])
      (set! lwp-list (cdr lwp-list))
      (p))))

(define pause
  (lambda ()
    (call/cc
      (lambda (k)
        (lwp (lambda () (k #f)))
        (start)
        ))))

(lwp (lambda () (let f () (pause) (display "h") (f))))
(lwp (lambda () (let f () (pause) (display "e") (f))))
(lwp (lambda () (let f () (pause) (display "y") (f))))
(lwp (lambda () (let f () (pause) (display "!") (f))))
(lwp (lambda () (let f () (pause) (newline) (f))))

;(start)


(define (yinyang) 
  (let* ((yin
          ((lambda (cc) (display #\@) cc)
           (call/cc (lambda (c) c))))         ;;; C1
         (yang
          ((lambda (cc) (display #\*) cc)
           (call/cc (lambda (c) c)))))        ;;; C2
    (yin yang)))

;(yinyang)




