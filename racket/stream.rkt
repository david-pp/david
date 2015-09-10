#lang racket

(require racket/stream)

(define (integers-starting-from n)
  (stream-cons n (integers-starting-from (+ n 1))))

(define s1 (integers-starting-from 1))
(define s2 (integers-starting-from 2))


(define (withdraw balance amount-stream)
  (stream-cons balance 
               (withdraw (- balance (stream-first amount-stream))
                         (stream-rest amount-stream))))

;(define b1 (withdraw 100 b1))

(define ones (stream-cons 1 ones))

(define amounts (stream 10 20 30 40))

(define W1 (withdraw 100 amounts))


(define reds (stream-cons "red" reds))
(define red-blues (stream-add-between reds "blue"))

(define (fib n)
  (if (<= n 1)
      n
      (+ (fib (- n 1)) (fib (- n 2)))))

(define fib2
  (let ([memo '((0 . 0) (1 . 1))])
    (lambda (n)
      (let ([prev-ans (assoc n memo)])
        (if prev-ans
            (cdr prev-ans)
            (let ([ans (+ (fib (- n 1)) (fib (- n 2)))])
              (set! memo (cons (cons n ans) memo))
              ans))))))


(define (stream-add s1 s2)
  (stream-cons (+ (stream-first s1) (stream-first s2))
               (stream-add (stream-rest s1) (stream-rest s2))))
  

(define fibs
  (stream-cons 0 
               (stream-cons 1 
                            (stream-add (stream-rest fibs) fibs))))


; apply an operation (op v e) to each element e of the stream
    (define (op-on-stream op v s)
      (s-map (lambda (e) (op v e)) s) )
     
    (define (square x) (* x x))
     
    (define (scale-stream stream factor)
      (s-map (lambda (x) (* x factor)) stream))
     
    (define (partial-sums s)
      (if (s-null? s) s-empty
          (s-cons (s-first s) 
                       (op-on-stream + (s-first s) (partial-sums (s-rest s))))
          )
      )
     
    (define (pi-summands n)
      (s-cons (/ 1.0 n)
                   (s-map - (pi-summands (+ n 2)))))
     
    (define pi-stream
      (scale-stream (partial-sums (pi-summands 1)) 4))



