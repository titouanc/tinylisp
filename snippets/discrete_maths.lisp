(define fac (lambda (n) (if (< n 1) 1 (* n (fac (- n 1))))))

(define pow (lambda (x y) (if (< y 1) 1 (* x (pow x (- y 1))))))

(pow 3 (fac 3))
