(load "reversi-package")
(use-package 'reversi-package)

;; (reversi #'random-strategy #'random-strategy)

;; (reversi #'random-strategy #'human)

(defun mobility (player board)  ;; Peor que mix-count-mobility-3
  "The number of moves a player has."
  (length (legal-moves player board)))

;; (reversi #'human (alpha-beta-searcher 2 #'count-difference))

(defun count-difference (player board)  ;; Peor que mobility tras varios tests
  "Count player's pieces minus opponent's pieces."
  (let ((brd (get-board board)))
    (- (reduce #'+ (mapcar #'(lambda (row) (count player row)) brd))
       (reduce #'+ (mapcar #'(lambda (row) (count (opponent player) row)) brd)))))

(defun other-mobility (player board)
  (- 0 (length (legal-moves (opponent player) board))))

(defun both-mobility (player board)
  (+ (mobility player board) (other-mobility player board)))

;; Fuente http://web.eecs.utk.edu/~zzhang61/docs/reports/2014.04%20-%20Searching%20Algorithms%20in%20Playing%20Othello.pdf
(defparameter matrix-solution-2
  '((120 -20 20 5  5  20 -20 120)
    (-20 -40 -5 -5 -5 -5 -40 -20)
    (20  -5  15 3  3  15 -5  20)
    (5   -5  3  3  3  3  -5  5)
    (5   -5  3  3  3  3  -5  5)
    (20  -5  15 3  3  15 -5  20)
    (-20 -40 -5 -5 -5 -5 -40 -20)
    (120 -20 20 5  5  20 -20 120)))

(defun get-elemento (player elem-tablero elem-solucion)
  (cond ((equal player elem-tablero) elem-solucion)
        ((equal (opponent player) elem-tablero) (- 0 elem-solucion))
        (T 0)))

(defun suma-fila (player fila-tablero fila-solucion)
  (reduce #'+ (mapcar #'(lambda (x y) (get-elemento player x y)) fila-tablero fila-solucion)))

(defun suma-matriz (player board solucion)
  (reduce #'+ (mapcar #'(lambda (x y) (suma-fila player x y)) board solucion)))

(defun matrix-solution-2-algorith (player board)
  (suma-matriz player (get-board board) matrix-solution-2))

;; Para mejorar: https://github.com/norvig/paip-lisp/blob/master/docs/chapter18.md
(defun mobility-matrix-solution-2-algorith-2 (player board)
  (let ((foo (matrix-solution-2-algorith player board))
        (bar (mobility player board)))
    (cond ((and (< foo 0) (not (equal bar 0))) (/ foo bar))
          ((and (< foo 0) (equal bar 0)) foo)
          (t (* foo bar)))))

;; Copiado de https://github.com/norvig/paip-lisp/blob/master/docs/chapter18.md
(defparameter *weights*
  '#(0   0   0  0  0  0  0   0   0 0
     0 120 -20 20  5  5 20 -20 120 0
     0 -20 -40 -5 -5 -5 -5 -40 -20 0
     0  20  -5 15  3  3 15  -5  20 0
     0   5  -5  3  3  3  3  -5   5 0
     0   5  -5  3  3  3  3  -5   5 0
     0  20  -5 15  3  3 15  -5  20 0
     0 -20 -40 -5 -5 -5 -5 -40 -20 0
     0 120 -20 20  5  5 20 -20 120 0
     0   0   0  0  0  0  0   0   0 0))

;; Basado en modified-weighted-squares de https://github.com/norvig/paip-lisp/blob/master/docs/chapter18.md
(defun modified-matrix-solution-2-algoriths (player board)
  "Like matrix-solution-2-algorith, but don't take off for moving
  near an occupied corner."
  (let ((w (matrix-solution-2-algorith player board)))
    (dolist (corner '(11 18 81 88))
      (when (not (eql (bref board corner) empty))
        (dolist (c (neighbors corner))
          (when (not (eql (bref board c) empty))
            (incf w (* (- 5 (aref *weights* c))
                       (if (eql (bref board c) player)
                           +1 -1)))))))
    w))

(defun mobility-modified-matrix-solution-2-algoriths (player board)
  (let ((foo (modified-matrix-solution-2-algoriths player board))
        (bar (mobility player board)))
    (cond ((and (< foo 0) (not (equal bar 0))) (/ foo bar))
          ((and (< foo 0) (equal bar 0)) foo)
          (t (* foo bar)))))

(defun mobility-modified-matrix-solution-2-algoriths-2 (player board)
  (let ((foo (modified-matrix-solution-2-algoriths player board))
        (bar (mobility player board)))
    (cond ((and (< foo 0) (not (equal bar 0))) (/ foo bar))
          ((and (< foo 0) (equal bar 0)) foo)
          ((equal foo 0) bar)
          (t (* foo bar)))))

(defun both-mobility-modified-matrix-solution-2-algoriths (player board)
  (+  (mobility-modified-matrix-solution-2-algoriths player board)
    (other-mobility player board)))

(defun both-mobility-modified-matrix-solution-2-algoriths-2 (player board)
    (+  (mobility-modified-matrix-solution-2-algoriths-2 player board)
        (other-mobility player board)))

(round-robin
 (list (alpha-beta-searcher 2 #'both-mobility-modified-matrix-solution-2-algoriths-2)
       (alpha-beta-searcher 2 #'mobility-modified-matrix-solution-2-algoriths)
       (alpha-beta-searcher 2 #'mobility-modified-matrix-solution-2-algoriths-2)
       )
 500
 10
 '(both-mobility-modified-matrix-solution-2-algoriths-2
   mobility-modified-matrix-solution-2-algoriths
   mobility-modified-matrix-solution-2-algoriths-2
  )
 )