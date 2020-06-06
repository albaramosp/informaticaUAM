;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun newton (f df-dx max-iter x0 &optional (tol-abs 0.0001))
  "Zero of a function using the Newton-Raphson method


    INPUT:  f:        function whose zero we wish to find
            df-dx:    derivative of f
            max-iter: maximum number of iterations 
            x0:       initial estimation of the zero (seed)
            tol-abs:  tolerance for convergence


    OUTPUT: estimation of the zero of f, NIL if not converged"

  (unless (or (= max-iter 0) (< (abs (funcall df-dx x0)) 0.0001)) ;;End of algorithm because the max-iter number was reached without fulfilling the condition
          (let ((x (- x0 (/ (funcall f x0) (funcall df-dx x0))))) ;;Stores de value of Xn+1
            (if (< (abs (- x x0)) tol-abs) ;;The estimation of Xn+1 is convergent
              x
              (newton f df-dx (- max-iter 1) x tol-abs))))) ;;Try again with Xn+1

;; TESTS
;; Ejemplo dado:
;; >> (newton #'sin #'cos 50 2.0) 
;; 3.1415927
;; Caso base:
;; >> (newton #'sin #'cos 0 2.0) 
;; NIL
;; Casos típicos:
;; >> (newton #'sin #'cos 50 1)
;; 0.0
;; >> (newton #'sin #'cos 50 pi)
;; 3.141592653589793d0
;; >> (newton #'sin #'cos 50 6)
;; 6.2831855
;; Caso especial:
;; >> (newton #'sin #'cos 50 (/ pi 2)) 
;; NIL


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun newton-all (f df-dx max-iter seeds &optional (tol-abs 0.0001))
  "Zeros of a function using the Newton-Raphson method


    INPUT:  f:        function whose zero we wish to find
            df-dx:    derivative of f
            max-iter: maximum number of iterations 
            seeds:    list of initial estimations of the zeros 
            tol-abs:  tolerance for convergence


    OUTPUT: list of estimations of the zeros of f"
  
  ;;Applies the Newton function to all the elements in the list of seeds
  (mapcar #'(lambda (x) (newton f df-dx max-iter x tol-abs)) seeds))
  

;; TESTS
;; Ejemplo dado:
;; >> (newton-all #'sin #'cos 50 (mapcar #'eval '((/ pi 2) 1.0 2.0 4.0 6.0))) 
;; (NIL 0.0 3.1415927 3.1415927 6.2831855)
;; Caso base:
;; >> (newton-all #'sin #'cos 0 (mapcar #'eval '((/ pi 2) 1.0 2.0 4.0 6.0))) 
;; Casos típicos:
;; (newton-all #'sin #'cos 50 '(1.0 2.0 4.0 6.0))
;; (0.0 3.1415927 3.1415927 6.2831855)
;; (NIL NIL NIL NIL NIL)
;; Caso especial:
;; >> (newton-all #'sin #'cos 50 NIL) 
;; NIL
;; >> (newton-all #'sin #'cos 0 '((NIL))) 
;; (NIL)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun combine-elt-lst (elt lst)
  "Combines an element with all the elements of a list


    INPUT:  elt: element 
            lst: list 


    OUTPUT: list of pairs, such that 
               the first element of the pair is elt. 
               the second element is an element from lst"
  
  ;;For each element of the list, create a list/pair with it and the element
  (unless (or (null elt)  (null lst))
    (mapcar #'(lambda(x) (list elt x)) lst)))

;; TESTS
;; Ejemplo dado:
;; >> (combine-elt-lst 'a '(1 2 3))
;; ((A 1) (A 2) (A 3))
;; Caso base:
;; >> (combine-elt-lst '() '())
;; NIL
;; >> (combine-elt-lst 'a '())
;; NIL
;; >> (combine-elt-lst '() '(1 2 3))
;; NIL
;; Casos típicos:
;; >> (combine-elt-lst 'a '((/ 6 2) (+ 2 8) o))
;; ((A (/ 6 2)) (A (+ 2 8)) (A O))
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun combine-lst-lst (lst1 lst2)
  "Calculate the cartesian product of two lists


    INPUT:  lst1: list 
            lst2: list 


    OUTPUT: list of pairs, of the cartesian product"
  
  ;;For each element of the first list, combine it with each element of the second list using the previous function
  (unless (or (null lst1)  (null lst2))
    (mapcar #'(lambda(x) (combine-elt-lst x lst2)) lst1)))

;; TESTS
;; Ejemplo dado:
;; >> (combine-lst-lst '(a b c) '(1 2))
;; ((((A 1) (A 2)) ((B 1) (B 2)) ((C 1) (C 2)))
;; Caso base:
;; >> (combine-lst-lst '() '())
;; NIL
;; >> (combine-lst-lst '(a b c) '())
;; NIL
;; >> (combine-lst-lst '() '(1 2))
;; NIL
;; Casos típicos:
;; >> (combine-lst-lst '(a (- 3 1) (cos pi 2)) '((/ 6 2) (+ 2 8) o))
;; (((A (/ 6 2)) (A (+ 2 8)) (A O)) (((- 3 1) (/ 6 2)) ((- 3 1) (+ 2 8)) ((- 3 1) O)) (((COS PI 2) (/ 6 2)) ((COS PI 2) (+ 2 8)) ((COS PI 2) O)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun combine-list-of-lsts (lolsts)
  "Combinations of N elements, each of wich


   INPUT:  lstolsts: list of N sublists (list1 ... listN)


   OUTPUT: list of sublists of N elements, such that in each 
           sublist the first element is from list1
                 the second element is from list 2
                 ...
                 the Nth element is from list N"
  (cond 
    ((null lolsts) '(NIL))
    ((null (rest lolsts)) (mapcar #'list (car lolsts)))
    ( t 
      (let ((scalar (combine-list-of-lsts (cdr lolsts))))
        (apply #'append (mapcar #'(lambda(x) (mapcar #'(lambda(y) (cons x y)) scalar)) (car lolsts)))))))

;; TESTS
;; Ejemplo dado:
;; >> (combine-list-of-lsts '((a b c) (+ -) (1 2 3 4)))
;; ((A + 1) (A + 2) (A + 3) (A + 4) (A - 1) (A - 2) (A - 3) (A - 4) (B + 1) (B + 2) (B + 3) (B + 4) (B - 1) (B - 2) (B - 3) (B - 4) (C + 1) (C + 2) (C + 3) (C + 4) (C - 1) (C - 2) (C - 3) (C - 4))
;; Caso base:
;; >> (combine-list-of-lsts '())
;; (NIL)
;; >> (combine-list-of-lsts '(()))
;; NIL
;; >> (combine-list-of-lsts '(() (1 2)))
;; NIL
;; >> (combine-list-of-lsts '((1 2)))
;; ((1) (2))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun scalar-product (x y)
  "Calculates the scalar product of two vectors both vectors should be of the same size and with numeric values
 
   INPUT:  x: vector, represented as a list
           y: vector, represented as a list
 
   OUTPUT: scalar product between x and y


   NOTES: 
        * Implemented with mapcar"
  
  ;;The scalar product is the sum of the product of each element in the vectors
  ;;First, calculate all the products and store them in a list
  ;;Finally, sum all of the results
  (unless (or (null x)  (null y))
    (reduce #'+ (mapcar #'(lambda (a b) (* a b)) x y))))

;; TESTS
;; Ejemplo dado:
;; >> (scalar-product '(1 2 3) '(3 -1 5))
;; 16
;; >> (scalar-product '() '())
;; NIL
;; >> (scalar-product '(1 2) '())
;; NIL
;; >> (scalar-product '() '(1 2))
;; NIL
;; Caso atípico: 
;; Respuesta no correcta pero al no poder usar lenght no podemos saber si dos vectores son del mismo tamaño o no.
;; Igualmente en la definición de los parametros de la función se pide que ambos vectores sean del mismo tamaño,
;; no nos hacemos responsables de errores por el uso incorrecto de la función.
;; >> (scalar-product '(5) '(1 2))
;; 5

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; euclidean-norm


(defun euclidean-norm (x)
  "Calculates the euclidean (l2) norm of a vector
   
    INPUT:  x: vector, represented as a list with numeric values


    OUTPUT: euclidean norm of x"

;;The euclidean norm is the squared root of the sum of the square of each element of the vector
;;First, create a list with the square of each element
;;Then, sum all the results
;;Finally, calculate the squared root of the sum
(sqrt (reduce #'+ (mapcar #'(lambda (a) (* a a)) x))))

;; TESTS
;; Ejemplo dado:
;; >> (euclidean-norm '(3 -1 5))
;; 5.91608
;; >> (euclidean-norm '())
;; 0.0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; euclidean-distance

(defun euclidean-distance (x y) 
  "Calculates the euclidean (l2) distance between two numeric vectors of the same length
 
    INPUT: x: vector, represented as a list
           y: vector, represented as a list


    OUTPUT: euclidean distance between x and y"
  
  ;;For the euclidean distance, we calculate the euclidean norm of the difference between 2 vectors
  ;;First, create a list with the differences of each element of the vectors
  ;;Then, calculate the square of each element in that list and sum the results
  ;;Finally, calculate the squared root of the sum 
  (unless (or (null x)  (null y))
  (sqrt (reduce #'+ (mapcar #'(lambda (c) (* c c)) (mapcar #'(lambda (a b) (- b a)) x y))))))

;; TESTS
;; Ejemplo dado:
;; >> (euclidean-distance '(1 2 3) '(3 -1 5))
;; 4.1231055
;; >> (euclidean-distance '() '())
;; NIL
;; >> (euclidean-distance '(1 2) '())
;; NIL
;; >> (euclidean-distance '() '(1 2))
;; NIL
;; Caso atípico:
;; Both vectors should be of the same length
;; >> (euclidean-distance '(1 2 3 4) '(3 -1 5))
;; 4.1231055

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun cosine-similarity (x y) 
  "Calculates the cosine similarity between two numeric vectors of the same length


    INPUT:  x: vector, representad as a list
            y: vector, representad as a list


    OUTPUT: cosine similarity between x and y


    NOTES: 
       * Evaluates to NIL (not defined)
         if at least one of the vectors has zero norm.
       * The two vectors are assumed to have the same length"
 
 ;;First, calculate the euclidean norm of the vectors
 ;;Then, the scalar product of the vectors
  (unless (or (null x)  (null y)) ;;Error if a vector has an euclidean norm of 0, that is, the vector is null
    (/ (scalar-product x y) (* (euclidean-norm x) (euclidean-norm y))))) ;;The cosine similarity is the division between the scalar product and the product of the norms

;; TESTS
;; Ejemplo dado:
;; >> (cosine-similarity '(1 2 3) '(-2 1 3))
;; 0.6428571
;; >>  (cosine-similarity '() '())
;; NIL
;; >>  (cosine-similarity '(1 2) '())
;; NIL
;; >>  (cosine-similarity '() '(1 2))
;; NIL
;; Caso atípico:
;; Both vectors should be of the same length
;; >> (cosine-similarity '(1 2 3 4) '(-2 1 3))
;; 0.439155


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun angular-distance (x y) 
  "Calculates the angular distance between two vectors


   INPUT:  x: vector, representad as a list
           y: vector, representad as a list


   OUTPUT: cosine similarity between x and y


   NOTES: 
      * Evaluates to NIL (not well defined)
        if at least one of the vectors has zero norm.
      * The two vectors are assumed to have the same length"
  (unless (or (null x)  (null y))
    (/ (acos (cosine-similarity x y)) pi))) ;;Reverse of cosine similarity divided by pi

;; TESTS
;; Ejemplo dado:
;; >> (angular-distance '(1 2 3) '(-2 1 3))
;; 0.27774892163415615d0
;; >>  (angular-distance '() '())
;; NIL
;; >>  (angular-distance '(1 2) '())
;; NIL
;; >>  (angular-distance '() '(1 2))
;; NIL
;; Caso atípico:
;; Both vectors should be of the same length
;; >> (angular-distance '(1 2 3 4) '(-2 1 3))
;; 0.35527790561153083d0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; select-vectors


(defun select-vectors (lst-vectors test-vector similarity-fn &optional (threshold 0))
    "Selects from a list the vectors whose similarity to a 
     test vector is above a specified threshold. 
     The resulting list is ordered according to this similarity.
 
     INPUT:  lst-vectors:   list of vectors
             test-vector:   test vector, representad as a list
             similarity-fn: reference to a similarity function
             threshold:     similarity threshold (default 0)
      
     OUTPUT: list of pairs. Each pair is a list with
             a vector and a similarity score.
             The vectors are such that their similarity to the 
             test vector is above the specified threshold.
             The list is ordered from larger to smaller 
             values of the similarity score 
     
     NOTES: 
        * Uses remove-if and sort"
  (unless (or (null lst-vectors)  (null test-vector))
    (sort (remove-if #'(lambda (y) (null y))
            (mapcar #'(lambda (x)
                        (let ((similarity (funcall similarity-fn x test-vector))) ;;Store the similarity result in a variable
                          (unless (< similarity threshold) ;;Only store similarities that are greater than the threshold, if defined
                            (list x similarity)))) 
                    lst-vectors))
          #'(lambda(a b) (> (abs a) (abs b)))
          :key #'second))) ;;Sort the pairs of the resulting list using the similarity value

;; TESTS
;; Ejemplo dado:
;; >> (select-vectors '((-1 -1 -1) (-1 -1 1) (-1 1 1) (1 1 1)) '(1 1 1) #'cosine-similarity 0.2)   
;; (((1 1 1) 1.0) ((-1 1 1) 0.33333334))
;; >>  (select-vectors '() '(1 1 1) #'cosine-similarity 0.2)
;; NIL
;; >>  (select-vectors '((-1 -1 -1) (-1 -1 1) (-1 1 1) (1 1 1)) '() #'cosine-similarity 0.2)
;; NIL
 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun nearest-neighbor (lst-vectors test-vector distance-fn)
  "Selects from a list the vector that is closest to the 
   reference vector according to the specified distance function 
 
   INPUT:  lst-vectors:   list of vectors
           ref-vector:    reference vector, represented as a list
           distance-fn:   reference to a distance function
      
   OUTPUT: List formed by two elements:
           (1) the vector that is closest to the reference vector 
               according to the specified distance function
           (2) The corresponding distance value.


   NOTES: 
      * The implementation is recursive
      * It ignores the vectors in lst-vectors for which the 
        distance value cannot be computed."
   (unless (or (null lst-vectors)  (null test-vector))
    (first (sort (remove-if #'(lambda (y) (null y))
			                      (let ((distance (funcall distance-fn (car lst-vectors) test-vector) ))
                      				(list (list (car lst-vectors) distance) 
                                    (nearest-neighbor (cdr lst-vectors) test-vector distance-fn))))
                  #'(lambda(a b) (< (abs a) (abs b))) 
                  :key #'second))))

;; TESTS
;; Ejemplo dado:
;; >> (nearest-neighbor '((-1 -1 -1) (-2 2 2) (-1 -1 1)) '(1 1 1) #'angular-distance)
;; ((-2 2 2) 0.3918265514242322d0)
;; >> (nearest-neighbor '() '(1 1 1) #'angular-distance)
;; NIL
;; >> (nearest-neighbor '((-1 -1 -1) (-2 2 2) (-1 -1 1)) '() #'angular-distance)
;; NIL


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun backward-chaining-aux (goal lst-rules pending-goals)
  (when (some #'(lambda (x) (equal goal (cadr x))) lst-rules)
        (let ((found (car (member-if #'(lambda (x) (equal goal (cadr x))) lst-rules))))
          (if (null (car found))
            T
            (if (every #'(lambda (x) (equal x T)) (mapcar  #'(lambda (y) (backward-chaining-aux y (remove-if #'(lambda (x) (equal found x)) lst-rules) (cons goal pending-goals))) (car found)))
              T
              (backward-chaining-aux goal (remove-if #'(lambda (x) (equal found x)) lst-rules) pending-goals))))))

;; TESTS
;; Ejemplo dado:
;; >> (backward-chaining-aux 'Q '((NIL A) (NIL B) ((P) Q) ((L M) P) ((B L) M) ((A P) L) ((A B) L)) NIL)
;; T
;; >> (backward-chaining-aux 'P '((NIL A) (NIL B)  ((L M) P) ((B L) M) ((A P) L) ((A B) L)) '(Q))
;; T
;; >> (backward-chaining-aux '() '((NIL A) (NIL B)  ((L M) P) ((B L) M) ((A P) L) ((A B) L)) '(Q))
;; NIL
;; >> (backward-chaining-aux 'Q '() NIL)
;; NIL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun backward-chaining (goal lst-rules)
  "Backward-chaining algorithm for propositional logic
 
   INPUT: goal:      symbol that represents the goal
          lst-rules: list of pairs of the form 
                     (<antecedent>  <consequent>)
                     where <antecedent> is a list of symbols
                     and  <consequent> is a symbol


   OUTPUT: T (goal derived) or NIL (goal cannot be derived)


   NOTES: 
        * Implemented with some, every" 

  (unless (or (null goal)  (null lst-rules))
    (backward-chaining-aux goal lst-rules NIL)))

;; TESTS
;; Ejemplo dado:
;; >> (backward-chaining 'Q '((NIL A) (NIL B) ((P) Q) ((L M) P) ((B L) M) ((A P) L) ((A B) L)))
;; T
;; >> (backward-chaining 'Q '((NIL A) (NIL B) ((P) Q) ((L M) P) ((B L) M) ((A P) L)))
;; NIL
;; >>  (backward-chaining '() '((NIL A) (NIL B) ((P) Q) ((L M) P) ((B L) M) ((A P) L)))
;; NIL
;; >> (backward-chaining 'Q '())
;; NIL

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 (defun new-paths (path node net)
  (mapcar #'(lambda(n) 
        (cons n path)) 
                (rest (assoc node net))))

;; TESTS
;; Casos típicos:
;; >> (new-paths '(e c) 'e '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; ((B E C) (F E C))
;; >> (new-paths '(b a) 'b '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; ((A B A) (D B A) (E B A) (F B A))
;; Casos especiales:
;; >> (new-paths '(b a) '() '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (new-paths '() 'a '( (a b) (b c) (c a) (d e) (e f) ))
;; ((B))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Breadth-first-search in graphs
;;;
(defun bfs (end queue net)
  (if (null queue) 
      NIL
    (let* ((path (first queue))
           (node (first path)))
      (if (eql node end) 
          (reverse path)
        (bfs end 
             (append (rest queue) 
                     (new-paths path node net)) 
             net))))) 

;; TESTS
;; Casos típicos:
;; >> (bfs 'f '((c)) '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; (C E F)
;; >> (bfs 'f '((c)) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; (C A B F)
;; Casos especiales:
;; >> (bfs 'f '(()) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (bfs 'f '((a)) '( (a b) (b c) (c a) (d e) (e f) ))
;; el algoritmo entra en un bucle infinito  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun shortest-path (start end net)
  (bfs end (list (list start)) net))    


;; TESTS
;; Casos típicos:
;; >> (shortest-path 'c 'f '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; (C E F)
;; >> (shortest-path 'c 'f '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; (C A B F)
;; >> (shortest-path 'a 'c '( (a b e) (b c) (d c) (e d) ))
;; (A B C)
;; Casos especiales:
;; >> (shortest-path '() 'f '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (shortest-path 'a 'f '( (a b) (b c) (c a) (d e) (e f) ))
;; el algoritmo entra en un bucle infinito 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun new-paths-improved (path node net)
  "Gets the children of a node and adds them to the queue of nodes to be explored, only if they have not appeared yet.
   If the child is already present in the path, NIL is added and removed afterwards.
 
   INPUT: path: list of nodes already visited, to add the children to it
          node: node to get its children
          net:  graph represented as an adjacent list with a list of sublists of the form
                    (<node> <children>) 
                    where <node> is a node of the graph
                    and   <children> are its children nodes  

   OUTPUT: list of sublists of the form 
            (<node path>)
            where node is the child 
            and path are the nodes forming the path already visited"

  (remove NIL (mapcar #'(lambda(n)
      (if (member n path)
        NIL
        (cons n path)))
                (rest (assoc node net)))))

;; TESTS
;; Casos típicos:
;; >> (new-paths-improved '(e c) 'e '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; ((B E C) (F E C))
;; >> (new-paths-improved '(b a) 'b '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; ((D B A) (E B A) (F B A))
;; Casos especiales:
;; >> (new-paths-improved '(b a) '() '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (new-paths-improved '() 'a '( (a b) (b c) (c a) (d e) (e f) ))
;; ((B))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
(defun bfs-improved (end queue net)
   "Breadth-First-Search improved taking into account repeated nodes
 
   INPUT: end:   goal node
          queue: list of sublists with the nodes -and their path- pending to be visited in order of creation, so to ensure that
                 the nodes of a level are all explored before exploring the ones of a deeper level.
          net:   graph represented as an adjacent list with a list of sublists of the form
                    (<node> <children>) 
                    where <node> is a node of the graph
                    and   <children> are its children nodes  

   OUTPUT: list of nodes with the path to the goal (goal reached) or NIL (goal cannot be reached)"

  (if (null queue) 
      NIL
    (let* ((path (first queue))
           (node (first path)))
      (if (eql node end) 
          (reverse path)
        (bfs-improved end 
             (append (rest queue) 
                     (new-paths-improved path node net)) 
             net))))) 

;; TESTS
;; Casos típicos:
;; >> (bfs-improved 'f '((c)) '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; (C E F)
;; >> (bfs-improved 'f '((c)) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; (C A B F)
;; Casos especiales:
;; >> (bfs-improved 'f '(()) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (bfs-improved 'f '((a)) '( (a b) (b c) (c a) (d e) (e f) ))
;; NIL 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun shortest-path-improved (end queue net)
     "Gets the shortest path from a node to another in a graph using the BFS algorithm. Avoids infinite loop in graph with cycles.
  
    INPUT:  end:   goal node
            queue: list of sublists with the nodes -and their path- pending to be visited in order of creation, so to ensure that
                   the nodes of a level are all explored before exploring the ones of a deeper level.
            net:   graph represented as an adjacent list with a list of sublists of the form
                      (<node> <children>) 
                      where <node> is a node of the graph
                      and   <children> are its children nodes  

    OUTPUT: list of nodes with the path to the goal (goal reached) or NIL (goal cannot be reached)"

  (bfs-improved end queue net))

;; TESTS
;; Casos típicos:
;; >> (shortest-path-improved 'f '((c)) '((a d) (b d f) (c e) (d f) (e b f) (f)))
;; (C E F)
;; >> (shortest-path-improved 'f '((c)) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; (C A B F)
;; Casos especiales:
;; >> (shortest-path-improved 'f '(()) '( (a b c d e) (b a d e f) (c a g) (d a b g h) (e a b g h) (f b h) (g c d e h) (h d e f g) ))
;; NIL
;; >> (shortest-path-improved 'f '((a)) '( (a b) (b c) (c a) (d e) (e f) ))
;; NIL 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;