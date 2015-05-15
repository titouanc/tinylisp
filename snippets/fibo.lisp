(define (fibo n1 n2 i) (if (< i 1) n1 (fibo n2 (+ n1 n2) (- i 1))))
(define (fib n) (fibo (real 1) (real 1) n))

(display (fib 1475))
