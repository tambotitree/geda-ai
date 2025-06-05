## ------------------------                                 -*- Autoconf -*-
## Python 3.x file handling
## Adapted from python2.m4
## ------------------------
# Copyright (C) 2025
# License: GPL or FSF Unlimited Copying
#
# This macro locates a usable Python 3 interpreter and sets up install paths.

AC_DEFUN([AM_PATH_PYTHON3],
 [
  m4_define_default([_AM_PYTHON3_INTERPRETER_LIST],
[python3 python3.13 python3.12 python3.11 python3.10 python3.9 python3.8 python3.7 python3.6 python3.5 python3.4 python3.3 python3.2 python3.1 python3.0])

  AC_ARG_VAR([PYTHON], [the Python 3 interpreter])

  if test -z "$PYTHON"; then
    AC_PATH_PROGS([PYTHON], _AM_PYTHON3_INTERPRETER_LIST, [:])
  fi
  
  AC_CACHE_CHECK([for Python 3 interpreter], [am_cv_python3], [
    am_cv_python3="`$PYTHON -c 'import sys; sys.stdout.write(sys.version[:3])' 2>/dev/null || echo no`"
  ])
  
  if test "$am_cv_python3" = "no"; then
    AC_MSG_ERROR([no suitable Python 3 interpreter found])
  fi

  AC_SUBST([PYTHON_VERSION], [$am_cv_python3])
  AC_SUBST([PYTHON_PREFIX], ['${prefix}'])
  AC_SUBST([PYTHON_EXEC_PREFIX], ['${exec_prefix}'])

  AC_CACHE_CHECK([for $PYTHON platform], [am_cv_python_platform],
    [am_cv_python_platform=`$PYTHON -c "import sys; sys.stdout.write(sys.platform)"`])
  AC_SUBST([PYTHON_PLATFORM], [$am_cv_python_platform])

  am_python_setup_sysconfig="\
import sys\n\
try:\n\
    import sysconfig\n\
    use_sysconfig = True\n\
except ImportError:\n\
    use_sysconfig = False\n"

  AC_CACHE_CHECK([for $PYTHON script directory], [am_cv_python_pythondir], [
    am_cv_python_pythondir=`$PYTHON -c "$am_python_setup_sysconfig\nif use_sysconfig:\n    print(sysconfig.get_path('purelib'))\nelse:\n    from distutils import sysconfig\n    print(sysconfig.get_python_lib(0,0))"`
  ])
  AC_SUBST([pythondir], [$am_cv_python_pythondir])
  AC_SUBST([pkgpythondir], [\${pythondir}/$PACKAGE])

  AC_CACHE_CHECK([for $PYTHON extension module directory], [am_cv_python_pyexecdir], [
    am_cv_python_pyexecdir=`$PYTHON -c "$am_python_setup_sysconfig\nif use_sysconfig:\n    print(sysconfig.get_path('platlib'))\nelse:\n    from distutils import sysconfig\n    print(sysconfig.get_python_lib(1,0))"`
  ])
  AC_SUBST([pyexecdir], [$am_cv_python_pyexecdir])
  AC_SUBST([pkgpyexecdir], [\${pyexecdir}/$PACKAGE])
 ])
