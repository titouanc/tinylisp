(define range (lambda (min max) (if (< min max) (cons min (range (+ 1 min) max)) #n)))

(define sq (lambda (x) (* x x)))

(define squares (map sq (range 0 12)))
(define sum-squares (reduce + squares 0))

(display sum-squares)
(map display squares)
