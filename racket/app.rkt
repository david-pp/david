#lang racket
(require web-server/servlet
         web-server/servlet-env)

(define name "david!")

(define (my-app req)
  (displayln "visiting...")
  (define binding (request-bindings req))
  (when (exists-binding? 'name binding)
    (displayln (extract-binding/single 'name binding)))
  (response/xexpr
   `(html (head (title "Hello!"))
          (body (p "Hey out there!测试一下!!")
                (p "aaaaaaaaaa")
                (p ,name)))))
 
(serve/servlet my-app
               #:port 8080
               #:listen-ip #f
               #:servlet-path "/app"
               #:command-line? #t
               #:extra-files-paths (list
                                    (build-path "E:/Code/david/bootstrap/")))