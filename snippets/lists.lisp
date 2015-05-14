(define (range min max) (if (< min max) (cons min (range (+ 1 min) max)) #n))
(define (sq  x) (* x x))

(define N 10000)
(define sum-squares (reduce + (map sq (range 0 N)) 0))

(display sum-squares)
