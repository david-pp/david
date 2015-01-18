#lang slideshow

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; slideshow里面显示代码片段
;; 用法：
;; (codify "int main ()
;; {
;;    return 0;
;; }")
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(provide codify)


(define (codify-line line linenum [size 14])
  (if (number? linenum)
      (hc-append
       (colorize (text (number->string linenum) (cons 'bold 'modern) size) "blue")
       (text "   ")
       (text line (cons 'bold 'modern) size))
      (text line (cons 'bold 'modern) size)))

(define (codify codetext #:showline [showline #t] #:size [size 14])
  (define linenum 0)
  (apply vl-append 
         (map (lambda (line) 
                (set! linenum (+ linenum 1))
                (codify-line line 
                             (if showline 
                                 linenum
                                 #f)
                             size)) 
              (string-split codetext "\n"))))

#|
;;
;; 语法高亮
;;

(define keywords 
  '("main"
    "int"
    "return"
    "void"
    ))

(define (keywords? word)
  (list? 
   (member word keywords)))

(define (codify-line line)
  (para
   (map (lambda (word)
          (if (keywords? word)
              (colorize (tt word) "blue")
              (tt word)))
        (string-split #:trim? #f #:repeat? #t line))))
|#