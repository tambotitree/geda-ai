# geda-python.m4 - Minimal Python environment checker for gEDA
# Copyleft 2025 John Ryan, maintainer geda-ai

AC_DEFUN([GEDA_PYTHON_SETUP], [

  AC_ARG_VAR([PYTHON], [Path to the Python interpreter to use (default: python3)])

  # Try to find Python if not set
  if test -z "$PYTHON"; then
    AC_PATH_PROGS([PYTHON], [python3 python], [:])
  fi

  AC_MSG_CHECKING([for Python interpreter])
  if test "$PYTHON" = ":"; then
    AC_MSG_ERROR([No usable Python interpreter found])
  else
    AC_MSG_RESULT([$PYTHON])
  fi

  # Get Python version
  AC_MSG_CHECKING([Python version])
  python_ver=`$PYTHON -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")' 2>/dev/null || echo no`
  if test "$python_ver" = "no"; then
    AC_MSG_ERROR([Unable to determine Python version])
  fi
  AC_MSG_RESULT([$python_ver])
  AC_SUBST([PYTHON_VERSION], [$python_ver])

  # Try to locate python.pc
  AC_MSG_CHECKING([for python.pc file])
  python_pc_path="$(find $($PYTHON -c 'import sysconfig; print(sysconfig.get_config_var("LIBDIR"))') -name "python*.pc" 2>/dev/null | head -n1)"
  if test -z "$python_pc_path"; then
    AC_MSG_ERROR([python.pc file not found in expected path])
  fi
  AC_MSG_RESULT([$python_pc_path])

  # Set PKG_CONFIG_PATH before calling pkg-config
  pkgconfig_dir=$(dirname "$python_pc_path")
  PKG_CONFIG_PATH="$pkgconfig_dir:$PKG_CONFIG_PATH"
  export PKG_CONFIG_PATH
  AC_MSG_NOTICE([PKG_CONFIG_PATH set to: $PKG_CONFIG_PATH])

  # Now extract Python build flags
  AC_MSG_CHECKING([for PYTHON_CFLAGS and PYTHON_LIBS])
  PYTHON_CFLAGS=`pkg-config --cflags python-${python_ver} 2>/dev/null`
  PYTHON_LIBS=`pkg-config --libs python-${python_ver} 2>/dev/null`

  if test -z "$PYTHON_CFLAGS" || test -z "$PYTHON_LIBS"; then
    AC_MSG_ERROR([Failed to get Python flags via pkg-config])
  fi
  AC_MSG_RESULT([OK])
  AC_SUBST([PYTHON_CFLAGS])
  AC_SUBST([PYTHON_LIBS])

])