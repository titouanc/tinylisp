(define (facrec acc n) (if (< n 1) acc (facrec (* acc n) (- n 1))))
(define (fac n) (facrec 1 n))

(fac 15)
