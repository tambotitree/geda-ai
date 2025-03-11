;; Minimal Scheme unit-test framework
;; Copyright (C) 2010 Peter Brett <peter@peter-b.co.uk>
;;
;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

;; Example of usage
;; ----------------
;;
;; The following program:
;;
;;   (use-modules (unit-test))
;;   (begin-test 'SuccessfulTest
;;     (assert-true #t)
;;     (assert-equal 1 1))
;;     (assert-thrown 'misc-error (error "Blah ~A" "Blah"))
;;   (begin-test 'FailTest
;;     (assert-equal #t "string"))
;;   (report-tests)
;;
;; Produces the output:
;;
;;   PASS: SuccessfulTest
;;   FAIL: FailTest
;;     assert-equal: expected: #t got: "string"
;;   Test summary
;;   Passed: 1
;;   Failed: 1
;;   Skipped: 0
;;

(define-module (unit-test)
  #:use-module (ice-9 pretty-print)
  #:export (assert-true
            assert-equal
            assert-same-strings
            %assert-thrown
            tests-passed?
            report-tests
            %begin-test)
  #:export-syntax (skip-test
                   begin-test
                   assert-thrown))

(or (defined? 'define-syntax)
    (use-modules (ice-9 syncase)))

(define *failed-tests* '())
(define *passed-tests* '())
(define *skipped-tests* '())

(define (assert-true result)
  (if result
      #t
      (throw 'test-failed-exception
             (simple-format #f "  assert-true: got: ~S" result))))

(define (assert-equal expected result)
  (if (equal? expected result)
      #t
      (throw 'test-failed-exception
             (simple-format #f "  assert-equal: expected: ~S got: ~S"
                            expected result))))

(define (assert-same-strings expected result)
  (let ((sorted-expected (sort-list expected string<?))
        (sorted-result (sort-list result string<?)))
    (if (equal? sorted-expected sorted-result)
        #t
        (throw 'test-failed-exception
               (simple-format #f "  assert-same-strings: expected: ~S got: ~S"
                              sorted-expected sorted-result)))))

(define (%assert-thrown key thunk)
  (catch key
         (lambda ()
           (thunk)
           (throw 'test-failed-exception
                  (simple-format #f "  assert-thrown: expected exception: ~S"
                                 key)))
         (lambda (key . args) #t)))

(define (%begin-test name test-thunk)
  (gc)
  (let ((test-success #t)
        (test-fail-msg #f))

    (catch #t test-thunk
           (lambda (key . args)
             (set! test-success #f)
             (set! test-fail-msg
                   (if (eqv? key 'test-failed-exception)
                       (car args)
                       (format #f "  unexpected exception: ~S" (cons key args))))))

    (if test-success
        (begin
          (format #t "PASS: ~A\n" name)
          (set! *passed-tests* (cons name *passed-tests*)))
        (begin
          (format #t "FAIL: ~A\n" name)
          (and test-fail-msg
               (format #t "~A\n" test-fail-msg))
          (set! *failed-tests* (cons name *failed-tests*))))))

(define-syntax begin-test
    (syntax-rules ()
      ((_ name . test-forms)
       (%begin-test name (lambda () . test-forms)))))

(define-syntax skip-test
  (syntax-rules ()
    ((_ name . test-forms)
     (begin
       (format #t "SKIP: ~A\n" name)
       (set! *skipped-tests* (cons name *skipped-tests*))))))

(define-syntax assert-thrown
    (syntax-rules ()
      ((_ key . test-forms)
       (%assert-thrown key (lambda () . test-forms)))))

(define (tests-passed?) (null? *failed-tests*))

(define (report-tests)
  (display "Test summary")(newline)
  (display "Passed:  ") (display (length *passed-tests*)) (newline)
  (display "Failed:  ") (display (length *failed-tests*)) (newline)
  (display "Skipped: ") (display (length *skipped-tests*)) (newline))
