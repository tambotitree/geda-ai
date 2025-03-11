;;; gEDA - GPL Electronic Design Automation
;;; gnetlist - gEDA Netlist
;;; Copyright (C) 1998-2010 Ales Hvezda
;;; Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
;;;
;;; This program is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
;;; MA 02111-1301 USA.

;;  PCB format

(use-modules (ice-9 format))


(define (PCB:display-connections nets)
  (apply format #f "~:@{~A-~A ~}\n" nets))


; This function is replaced with the above one. Due to non existent
; verification, this function is left commented out.
; /spe, 2002-01-08
;(define (PCB:display-connections nets)
;  (if (not (null? nets))
;      (string-append
;       (car (car nets)) "-" (car (cdr (car nets))) " "
;       (PCB:display-connections (cdr nets)))
;      "\n"))


(define (PCB:write-net netnames)
  (if (not (null? netnames))
      (let ((netname (car netnames)))
	(display netname)
	(display "\t")
	(display (gnetlist:wrap (PCB:display-connections (gnetlist:get-all-connections netname)) 200 " \\"))
	(PCB:write-net (cdr netnames)))))


(define (PCB output-filename)
  (set-current-output-port (gnetlist:output-port output-filename))
  (PCB:write-net (gnetlist:get-all-unique-nets "dummy"))
  (close-output-port (current-output-port)))
