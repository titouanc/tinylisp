(define (range min max) (if (< min max) (cons min (range (+ 1 min) max)) #n))
(define (sq  x) (* x x))

(define N 100)
(define sum-squares (apply + (map sq (range 0 N))))

(display sum-squares)
