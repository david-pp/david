#lang racket
(require web-server/servlet
         web-server/servlet-env)

(define (my-app req)
  (response/xexpr
   `(html (head (title "Hello world!"))
          (body (p "Hey out there!测试一下!!")
                (p "aaaaaaaaaa")))))
 
(serve/servlet my-app
               #:port 8080
               #:listen-ip #f
               #:servlet-path "/app"
               #:command-line? #t
               #:extra-files-paths (list
                                    (build-path "E:/Code/david/bootstrap/")))