; my first "real program" in scheme
; a brainf___ interpreter, you'll need to end your program in @
; tested with DrRacket #lang scheme

; you may want some port flushing for interactive use
; the io has been a little clunky in the terminal, so setting the input port to a string could be a solution

(define program-counter-wrapper
  (lambda ()
    (let ((x 0))
      (lambda (amount)
        (set! x (+ x amount)) x))))

(define position-counter-wrapper
  (lambda ()
    (let ((level 0))
      (lambda (symbol)
        (cond
          ((eq? symbol #\>)
           (set! level (+ level 1)) level)
          ((eq? symbol #\<)
           (set! level (- level 1)) level)
          (else level))))))

(define data-get 
  (lambda ()
    (vector-ref data (position-counter #t))))

; adjusts data
(define data-adjust
  (lambda (symbol)
    (letrec (
             (data-set!
              (lambda (x)
                (vector-set! data (position-counter #t) x))))
    (cond
      ((eq? symbol #\+)
       (cond
         ((< (data-get) 255) (data-set! (+ (data-get) 1)))
         (else (data-set! 0))))
      ((eq? symbol #\-)
       (cond
         ((> (data-get) 0) (data-set! (- (data-get) 1)))
         (else (data-set! 255))))
      (else #f)))))

; i/o
(define io
  (lambda (symbol)
    (letrec (
             (data-set!
              (lambda (x)
                (vector-set! data (position-counter #t) x))))
    (cond
      ; output
      ((eq? symbol #\.)
       (display (integer->char (data-get))))
      ; input
      ((eq? symbol #\,)
       (let ((char (read-char)))
         (cond
           ((or (eq? char #\newline) (not (in? char "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))) (io symbol))
           (else (data-set! (char->integer char))))))
      (else #f)))))

; increments and returns current instruction number
(define program-counter (program-counter-wrapper))
; gets and or sets current position over data
(define position-counter (position-counter-wrapper))
(define data (make-vector 100000 0))

; the no-op way of jumping to a later matching bracket
(define to-next-bracket
  (lambda (instructions)
    (cond
      ((null? instructions) '())
      ; If we find brackets inside, jump there, continue from there.
      ((eq? (car instructions) #\[)
       ; By increasing the level of recursion ...
       (to-next-bracket (to-next-bracket (cdr instructions))))
       ; ... the following non-recursive return brings us to the proper level.
      ((eq? (car instructions) #\]) (cdr instructions))
      (else (to-next-bracket (cdr instructions))))))
      
(define interpret
  (lambda (instructions)
    (cond
      ; end-program conditions, out of instructions or program instructions limit exceeded
      ((null? instructions) (void))
      ((> (program-counter 1) 100000) (newline) (display "PROCESS TIME OUT. KILLED!!!") (exit))
      ((eq? (car instructions) #\[)
       (cond
         ; skip to matching bracket
         ; according to spec, starts at the closing bracket. Compensate by incremementing program counter by one.
         ((= (data-get) 0) (program-counter 1) (interpret (to-next-bracket (cdr instructions))))
         (else
          (let ((result (interpret (cdr instructions))))
            (cond
              ; if the sub program returns null, repeat the current function from the start
              ((null? result) (interpret instructions))
              ; else call interpret on the instructions returned by sub program
              (else (interpret result)))))))
      ; if the loop ends, return the position to continue interpreting from
      ((eq?  (car instructions) #\])
       (cond
         ((= (data-get) 0) (cdr instructions))
         ; I thought that maybe I was douple counting an operation but seriously, what is an operation? right.
         (else  '() )))
      (else 
       ; these will ignore irrelevent instructions
       (position-counter (car instructions))
       (data-adjust (car instructions))
       (io (car instructions))
       ; interpret next instruction
       (interpret (cdr instructions))))))

(define in?
  (lambda (c s)
    (letrec 
        (
         (inner
          (lambda (l)
            (cond
              ((null? l) #f)
              ((eq? c (car l)) #t)
              (else (inner (cdr l)))))))
      (inner (string->list s)))))

(define read-instructions
  (lambda ()
    (let  
      ((char (read-char)))
      (cond
        ; define end program delimiter here
        ((in? char ";") '())
        ((in? char "+-.,<>[]") (cons char (read-instructions)))
        ; else throw away the read
        (else (read-instructions))))))

; here's how you can get input for now

(display "enter bf program ending with ;") (newline)
(interpret (read-instructions))
