#lang racket

(require db)

(struct build-history 
  (id 
   status 
   revisions 
   buildtime 
   buildlog))

(struct qahost
  (name
   user
   ip
   status
   runing))

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

(define (drop-table table)
  (query-exec db-conn (string-append "drop table if exists " table)))

(define (init-db)
  (drop-table "build_history")
  (drop-table "qahosts")
  (query-exec db-conn 
              (string-append "create table build_history("
                             " id int unsigned not null auto_increment,"
                             " status int not null,"
                             " revisions varchar(255) not null default '',"
                             " buildtime datetime not null,"
                             " buildlog text,"
                             " primary key (id)"
                             ")"))
 
  (query-exec db-conn 
              (string-append "create table qahosts("
                             " name varchar(32) not null default '',"
                             " user varchar(32) not null default '',"
                             " ip varchar(32) not null default '',"
                             " status int not null default '0',"
                             " running int unsigned not null default '0',"
                             " log text,"
                             " primary key (name)"
                             ")")))

(define (prepare-test-data)
  (for ([i (build-list 10 values)])
    (query-exec db-conn 
                "insert into build_history(status,revisions,buildtime,buildlog) values(?, ?, ?, ?)"
                1 "1235|5678|90000" "2015-02-03" "测试下看看!!"))
  (for ([i (build-list 10 values)])
    (query-exec db-conn 
                "insert into qahosts(name,user,ip,running,log) values(?, ?, ?, ?, ?)"
                (string-append "QA" (number->string i)) 
                "david" 
                "127.0.0.1"
                1
                "test.....")))

(define (get-build-history)
  (rows->dict (query db-conn "select * from build_history")
              #:key "id"
              #:value '#("status" "revisions" "buildtime" "buildlog")))

(define (get-qa-hosts)
  (rows->dict (query db-conn "select * from qahosts")
              #:key "name"
              #:value '#("user" "ip" "status" "running" "log")))

;(init-db)
;(prepare-test-data)