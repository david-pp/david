#lang racket

(require web-server/templates)
(require "model.rkt")

(define h (hash-keys (get-qa-hosts)))


(define (t1)
  (let ([hosts h])
    (include-template "hostinfo.html")))


(string-append (t1) (t1))