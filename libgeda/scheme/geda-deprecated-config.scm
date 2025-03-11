;; gEDA - GPL Electronic Design Automation
;; gschem - gEDA Schematic Capture - Scheme API
;; Copyright (C) 2012 Peter Brett <peter@peter-b.co.uk>
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
;; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
;;

;; This file contains deprecated configuration functions for RC files.
;; The use gEDA RC files for configuration is being replaced by the
;; use of key-value files that are parsed rather than executed. This
;; will allow configuration to be safely written back to disk, and
;; will have security benefits.  The functions defined in this file
;; are intended for use in legacy RC files during the transition
;; process.

(or (defined? 'define-syntax)
    (use-modules (ice-9 syncase)))

;; ===================================================================
;; Utility functions and macros
;; ===================================================================

;; Returns an RC function closure to replace the legacy configuration
;; function OLD-ID.  The returned closure takes an arbitrary number of
;; arguments, and does nothing other than print a deprecation message
;; the first time it is called.
(define (rc-dead-config old-id)
  ;; FIXME more helpful error message with link to documentation.
  (define (deprecation-warning)
    (format (current-error-port)
"WARNING: The RC file function '~A' is deprecated and does nothing.

" old-id))
  (let ((warned? #f))
    (lambda args
      (or warned? (begin (deprecation-warning) (set! warned? #t))))))

;; Convenience macro for using rc-dead-config.
;;
;;   define-rc-dead-config OLD-ID
;;
;; Creates a dead rc configuration function called OLD-ID.
(define-syntax define-rc-dead-config
  (syntax-rules ()
    ((_ old-id)
     (define old-id (rc-dead-config (quote old-id))))))

;; ===================================================================
;; Deprecated libgeda configuration functions
;; ===================================================================

(define-rc-dead-config postscript-prolog)

;; ===================================================================
;; Deprecated gschem configuration functions
;; ===================================================================

(define-rc-dead-config output-capstyle)
(define-rc-dead-config output-color)
(define-rc-dead-config output-orientation)
(define-rc-dead-config output-type)
(define-rc-dead-config paper-size)
(define-rc-dead-config paper-sizes)
(define-rc-dead-config print-command)
(define-rc-dead-config setpagedevice-orientation)
(define-rc-dead-config setpagedevice-pagesize)

(define-rc-dead-config net-style)
(define-rc-dead-config bus-style)
(define-rc-dead-config pin-style)
(define-rc-dead-config line-style)
(define-rc-dead-config net-endpoint-mode)
(define-rc-dead-config net-midpoint-mode)
(define-rc-dead-config object-clipping)
(define-rc-dead-config text-origin-marker)
(define-rc-dead-config text-display-zoomfactor)
(define-rc-dead-config text-feedback)

(define (untitled-name arg)
  (format (current-error-port)
"WARNING: The RC file function 'untitled-name' should only be used in gschemrc.

"))

(define-rc-dead-config scrollbar-update)
