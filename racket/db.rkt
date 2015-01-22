#lang racket
(require db)
(require db/util/testing)

;;
;; Better
;; 
(define pool
  (connection-pool
   (lambda ()
     (displayln "connection !")
     (mysql-connect #:server "127.0.0.1" 
                 #:user "david" 
                 #:password "123456"
                 #:database "david"))
   #:max-idle-connections 1))

;;
;; Best
;;
(define db-conn
  (virtual-connection
   (connection-pool
    (lambda ()
      (displayln "connection !")
      (mysql-connect #:server "127.0.0.1" 
                     #:user "david" 
                     #:password "123456"
                     #:database "david"))
    #:max-idle-connections 10)))


(define (init-db)
  (query-exec "create table build_history ()"))


;(define c1 (connection-pool-lease pool))
;(define c2 (connection-pool-lease pool))

(define (run) 
  (thread (lambda ()
            (define c (connection-pool-lease pool))
            (sleep 2)
            (for ([(n d) (in-query c "select * from the_numbers")])
              (printf "~a: ~a\n" n d))
            (disconnect c))))
