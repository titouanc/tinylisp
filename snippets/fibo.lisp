(define (fibo n1 n2 i) (if (< i 1) n1 (fibo n2 (+ n1 n2) (- i 1))))
(define (fib n) (fibo 1 1 n))

(display (fib 42))

(define (max-fib i) (if (< (fib i) (fib (- i 1))) i (max-fib (+ 1 i))))

(define i (max-fib 1))
(display i (fib (- i 2)) (fib (- i 1)) (fib i) (fib (+ 1 i)))

(apply + (map fib (range 0 1000))))
