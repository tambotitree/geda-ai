# debug-macros.m4 - Test macros for debugging Autotools

AC_DEFUN([AX_DEBUG_TRACE_TEST],
[
  # This macro simply prints a notice to confirm it was executed.
  # We use it to verify that 'autoconf --trace' is working.
  AC_MSG_NOTICE([*** AX_DEBUG_TRACE_TEST executed ***])
])
