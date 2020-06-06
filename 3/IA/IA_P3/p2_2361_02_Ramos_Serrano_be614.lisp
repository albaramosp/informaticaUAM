;; Alias que aparece en el ranking

(defvar *alias* '|BMMMS2A2|)

;; Función de evaluación heurística
(defun eval-fn (player board)
  (+  (mobility-modified-matrix-solution-2-algoriths-2 player board)
    (other-mobility player board)))

;; Funciones auxiliares
(defparameter matrix-solution-2
  '((120 -20 20 5  5  20 -20 120)
    (-20 -40 -5 -5 -5 -5 -40 -20)
    (20  -5  15 3  3  15 -5  20)
    (5   -5  3  3  3  3  -5  5)
    (5   -5  3  3  3  3  -5  5)
    (20  -5  15 3  3  15 -5  20)
    (-20 -40 -5 -5 -5 -5 -40 -20)
    (120 -20 20 5  5  20 -20 120)))

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

(defun mobility (player board)
  "The number of moves a player has."
  (length (legal-moves player board)))

(defun other-mobility (player board)
  (- 0 (length (legal-moves (opponent player) board))))

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

(defun modified-matrix-solution-2-algoriths (player board)
  (let ((w (matrix-solution-2-algorith player board)))
    (dolist (corner '(11 18 81 88))
      (when (not (eql (bref board corner) empty))
        (dolist (c (neighbors corner))
          (when (not (eql (bref board c) empty))
            (incf w (* (- 5 (aref *weights* c))
                       (if (eql (bref board c) player)
                           +1 -1)))))))
    w))
    
(defun mobility-modified-matrix-solution-2-algoriths-2 (player board)
  (let ((foo (modified-matrix-solution-2-algoriths player board))
        (bar (mobility player board)))
    (cond ((and (< foo 0) (not (equal bar 0))) (/ foo bar))
          ((and (< foo 0) (equal bar 0)) foo)
          ((equal foo 0) bar)
          (t (* foo bar)))))