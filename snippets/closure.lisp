(define plus (lambda (x) (lambda (y) (+ x y))))
(define plus3 (plus 3))

(display (plus3 39))

(define plus #n)
(define plus3 #n)

(define (outer) (lambda (inner) (lambda (inner-inner) (dmp inner-inner))))
(((outer) (outer)) (outer))
