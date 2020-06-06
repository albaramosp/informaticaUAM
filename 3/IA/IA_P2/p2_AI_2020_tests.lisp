;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Tests for the required functions in lab 2

(defparameter node-nevers
   (make-node :city 'Nevers) )
(defparameter node-paris
   (make-node :city 'Paris :parent node-nevers))
(defparameter node-nancy
   (make-node :city 'Nancy :parent node-paris))
(defparameter node-reims
   (make-node :city 'Reims :parent node-nancy))
(defparameter node-calais
   (make-node :city 'Calais :parent node-reims))

(defparameter node-calais-2
   (make-node :city 'Calais :parent node-paris))

(defparameter node-marseille-ex6
   (make-node :city 'Marseille :depth 12 :g 10 :f 20) )

(defun node-g-<= (node-1 node-2)
  (<= (node-g node-1)
      (node-g node-2)))

(defparameter *uniform-cost*
  (make-strategy
   :name 'uniform-cost
   :node-compare-p #'node-g-<=))

(defparameter node-paris-ex7
  (make-node :city 'Paris :depth 0 :g 0 :f 20) )

(defparameter node-nancy-ex7
  (make-node :city 'Nancy :depth 2 :g 50 :f 50) )

(defparameter node-calais-ex7
  (make-node :city 'Calais :depth 3 :g 150 :f 0) )




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 1:
;;
;;
(f-h '() *heuristic*) ;; NIL
(f-h 'Nantes '()) ;; NIL
(f-h 'Nantes *heuristic*) ;;  75.0 
(f-h 'Marseille *heuristic*) ;; 145.0
(f-h 'Lyon *heuristic*) ;; 105.0
(f-h 'Madrid *heuristic*)  ;; NIL
(f-h 'Nancy *heuristic*) ;; 50.0


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 2:
;;
;;

(navigate 'Avignon '()) ;; NIL
(navigate '() *trains*) ;; NIL
(navigate 'Madrid *trains*) ;; NIL
(action-cost (car (navigate 'Avignon *trains*))) ;; 30.0
(mapcar #'action-cost (navigate 'Avignon *trains*)) ;; (30.0 16.0)
(action-final (car (navigate 'Avignon *trains*))) ;; LYON
(mapcar #'action-final (navigate 'Paris *trains*)) ;; (CALAIS NANCY NEVERS ORLEANS ST-MALO)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 3:
;;
;;
(f-goal-test node-calais '() '()) ;; NIL 
(f-goal-test node-calais '(Calais Marseille) '(Paris Limoges)) ;; NIL
(f-goal-test node-paris '(Calais Marseille) '(Paris)) ;; NIL
(f-goal-test node-calais '(Calais Marseille) '(Paris Nancy)) ;; T
(f-goal-test node-calais '(Calais Marseille) '()) ;; T

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 4:
;;
;;

(f-search-state-equal '() '()) ;; NIL
(f-search-state-equal node-calais '() '()) ;; NIL
(f-search-state-equal node-calais node-calais-2 '()) ;; T
(f-search-state-equal node-calais node-calais-2 '(Reims)) ;; NIL
(f-search-state-equal node-calais node-calais-2 '(Nevers)) ;; T
(f-search-state-equal node-nancy node-paris '()) ;; NIL
(f-search-state-equal node-calais node-calais '(Paris Nancy)) ;; T


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 6:
;;
;; NOTE: In order to run the tests from this point on, you must 
;; have solved exercise 5, that is, you must have created the 
;; structure *travel* 
;;
;; ALSO NOTE: The output of tehse examples is "as is", the same as
;; it appears on the allegro console. The console "cuts" some of 
;; the outputs when these are too complex. Your output might be 
;; slightly diffferent than this one.

(expand-node node-marseille-ex6 '()) ;; NIL
(expand-node '() *travel*) ;; NIL
(node-g (car (expand-node node-marseille-ex6 *travel*))) ;; 26.0
(node-city (node-parent (car (expand-node node-marseille-ex6 *travel*)))) ;; MARSEILLE
(node-parent (node-parent (car (expand-node node-marseille-ex6 *travel*)))) ;; NIL
(mapcar #'(lambda (x) (node-city (node-parent x))) (expand-node node-nancy *travel*)) ;; (NANCY NANCY)
(node-g (car (expand-node node-marseille-ex6 *travel*))) ;; 26.0



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 7:
;;

(insert-nodes-strategy (list '()) 
                              '()
                              *A-star*) 
;; (NIL)
(defparameter sol-ex7 (insert-nodes-strategy (list node-paris-ex7 node-nancy-ex7) 
                                             (list node-calais-ex7)
                                             *A-star*))
(defparameter other-ex7 (insert-nodes-strategy sol-ex7 (list node-marseille-ex6) *A-star*)) 


(mapcar #'(lambda (x) (node-city x)) sol-ex7) ;; (CALAIS PARIS NANCY)
(mapcar #'(lambda (x) (node-g x)) sol-ex7) ;; (150 0 50)
(mapcar #'(lambda (x) (node-city x)) other-ex7) ;; (CALAIS PARIS MARSEILLE NANCY)
(mapcar #'(lambda (x) (node-g x)) other-ex7) ;; (150 0 10 50)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 8:
;;

(graph-search *travel* '()) ;; NIL
(graph-search '() *A-star*) ;; NIL
(node-g (graph-search *travel* *A-star*)) ;; 202.0
(node-city (graph-search *travel* *A-star*)) ;; CALAIS
(node-city (node-parent (graph-search *travel* *A-star*))) ;; PARIS


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 10:
;;
(solution-path NIL) ;; NIL
(solution-path (a-star-search *travel*)) ;; (MARSEILLE TOULOUSE LIMOGES ORLEANS PARIS CALAIS)
(action-sequence NIL) ;; NIL
(action-sequence (a-star-search *travel*))
;; (#S(ACTION :NAME NAVIGATE :ORIGIN MARSEILLE :FINAL TOULOUSE :COST 65.0)
;;  #S(ACTION :NAME NAVIGATE :ORIGIN TOULOUSE :FINAL LIMOGES :COST 25.0)
;;  #S(ACTION :NAME NAVIGATE :ORIGIN LIMOGES :FINAL ORLEANS :COST 55.0)
;;  #S(ACTION :NAME NAVIGATE :ORIGIN ORLEANS :FINAL PARIS :COST 23.0)
;;  #S(ACTION :NAME NAVIGATE :ORIGIN PARIS :FINAL CALAIS :COST 34.0))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 11:
;;
(graph-search *travel* *depth-first*)
;; #S(NODE
;;    :CITY CALAIS
;;    :PARENT #S(NODE
;;               :CITY PARIS
;;               :PARENT #S(NODE
;;                          :CITY ST-MALO
;;                          :PARENT #S(NODE
;;                                     :CITY NANTES
;;                                     :PARENT #S(NODE
;;                                                :CITY TOULOUSE
;;                                                :PARENT #S(NODE
;;                                                           :CITY LYON
;;                                                           :PARENT #S(NODE
;;                                                                      :CITY AVIGNON
;;                                                                      :PARENT #S(NODE
;;                                                                                 :CITY MARSEILLE
;;                                                                                 :PARENT NIL
;;                                                                                 :ACTION NIL
;;                                                                                 :DEPTH 0
;;                                                                                 :G 0
;;                                                                                 :H 0
;;                                                                                 :F 0)
;;                                                                      :ACTION #S(ACTION
;;                                                                                 :NAME NAVIGATE
;;                                                                                 :ORIGIN MARSEILLE
;;                                                                                 :FINAL AVIGNON
;;                                                                                 :COST 16.0)
;;                                                                      :DEPTH 1
;;                                                                      :G 16.0
;;                                                                      :H 135.0
;;                                                                      :F 151.0)
;;                                                           :ACTION #S(ACTION
;;                                                                      :NAME NAVIGATE
;;                                                                      :ORIGIN AVIGNON
;;                                                                      :FINAL LYON
;;                                                                      :COST 30.0)
;;                                                           :DEPTH 2
;;                                                           :G 46.0
;;                                                           :H 105.0
;;                                                           :F 151.0)
;;                                                :ACTION #S(ACTION
;;                                                           :NAME NAVIGATE
;;                                                           :ORIGIN LYON
;;                                                           :FINAL TOULOUSE
;;                                                           :COST 60.0)
;;                                                :DEPTH 3
;;                                                :G 106.0
;;                                                :H 130.0
;;                                                :F 236.0)
;;                                     :ACTION #S(ACTION
;;                                                :NAME NAVIGATE
;;                                                :ORIGIN TOULOUSE
;;                                                :FINAL NANTES
;;                                                :COST 80.0)
;;                                     :DEPTH 4
;;                                     :G 186.0
;;                                     :H 75.0
;;                                     :F 261.0)
;;                          :ACTION #S(ACTION
;;                                     :NAME NAVIGATE
;;                                     :ORIGIN NANTES
;;                                     :FINAL ST-MALO
;;                                     :COST 20.0)
;;                          :DEPTH 5
;;                          :G 206.0
;;                          :H 65.0
;;                          :F 271.0)
;;               :ACTION #S(ACTION
;;                          :NAME NAVIGATE
;;                          :ORIGIN ST-MALO
;;                          :FINAL PARIS
;;                          :COST 40.0)
;;               :DEPTH 6
;;               :G 246.0
;;               :H 30.0
;;               :F 276.0)
;;    :ACTION #S(ACTION :NAME NAVIGATE :ORIGIN PARIS :FINAL CALAIS :COST 34.0)
;;    :DEPTH 7
;;    :G 280.0
;;    :H 0.0
;;    :F 280.0)
(graph-search *travel* *breadth-first*)
;; #S(NODE
;;    :CITY CALAIS
;;    :PARENT #S(NODE
;;               :CITY PARIS
;;               :PARENT #S(NODE
;;                          :CITY ST-MALO
;;                          :PARENT #S(NODE
;;                                     :CITY NANTES
;;                                     :PARENT #S(NODE
;;                                                :CITY TOULOUSE
;;                                                :PARENT #S(NODE
;;                                                           :CITY MARSEILLE
;;                                                           :PARENT NIL
;;                                                           :ACTION NIL
;;                                                           :DEPTH 0
;;                                                           :G 0
;;                                                           :H 0
;;                                                           :F 0)
;;                                                :ACTION #S(ACTION
;;                                                           :NAME NAVIGATE
;;                                                           :ORIGIN MARSEILLE
;;                                                           :FINAL TOULOUSE
;;                                                           :COST 65.0)
;;                                                :DEPTH 1
;;                                                :G 65.0
;;                                                :H 130.0
;;                                                :F 195.0)
;;                                     :ACTION #S(ACTION
;;                                                :NAME NAVIGATE
;;                                                :ORIGIN TOULOUSE
;;                                                :FINAL NANTES
;;                                                :COST 80.0)
;;                                     :DEPTH 2
;;                                     :G 145.0
;;                                     :H 75.0
;;                                     :F 220.0)
;;                          :ACTION #S(ACTION
;;                                     :NAME NAVIGATE
;;                                     :ORIGIN NANTES
;;                                     :FINAL ST-MALO
;;                                     :COST 20.0)
;;                          :DEPTH 3
;;                          :G 165.0
;;                          :H 65.0
;;                          :F 230.0)
;;               :ACTION #S(ACTION
;;                          :NAME NAVIGATE
;;                          :ORIGIN ST-MALO
;;                          :FINAL PARIS
;;                          :COST 40.0)
;;               :DEPTH 4
;;               :G 205.0
;;               :H 30.0
;;               :F 235.0)
;;    :ACTION #S(ACTION :NAME NAVIGATE :ORIGIN PARIS :FINAL CALAIS :COST 34.0)
;;    :DEPTH 5
;;    :G 239.0
;;    :H 0.0
;;    :F 239.0)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;
;; Exercise 12:
;;
(time (graph-search *travel* *A-star*))
;; Evaluation took:
;;   0.000 seconds of real time
;;   0.000027 seconds of total run time (0.000027 user, 0.000000 system)
;;   100.00% CPU
;;   100,258 processor cycles
;;   0 bytes consed

(time (graph-search *travel-new* *A-star*))
;; Evaluation took:
;;   0.001 seconds of real time
;;   0.001622 seconds of total run time (0.001622 user, 0.000000 system)
;;   200.00% CPU
;;   6,140,122 processor cycles
;;   786,368 bytes consed

(time (graph-search *travel-cero* *A-star*))
;; Evaluation took:
;;   0.004 seconds of real time
;;   0.003367 seconds of total run time (0.003367 user, 0.000000 system)
;;   75.00% CPU
;;   12,743,535 processor cycles
;;   1,834,960 bytes consed