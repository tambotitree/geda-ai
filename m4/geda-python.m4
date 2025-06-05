# geda-python.m4 - Python environment detection and dependency file generation
AC_DEFUN([GEDA_PYTHON_SETUP], [
  AC_ARG_VAR([PYTHON], [Path to the Python interpreter to use (default: python3)])

  # Try to find python3 if not set
  if test -z "$PYTHON"; then
    AC_PATH_PROGS([PYTHON], [python3 python], [:])
  fi

  AC_MSG_CHECKING([Python version])
  python_ver="`$PYTHON -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")' 2>/dev/null || echo no`"
  if test "$python_ver" = "no"; then
    AC_MSG_NOTICE([
=========================================================
 💥 Python3 >= 3.6 not found via pkg-config or pythonX-config
---------------------------------------------------------
 🤔 Are you running inside a conda environment?

    Current CONDA_PREFIX: ${CONDA_PREFIX:-not set}

    If not set, activate your conda environment:
      conda create -n geda-ai python=3.9
      conda activate geda-ai
      pip install -r requirements.txt

    Then re-run:
      ./configure --prefix=$HOME/geda

 🐍 A python with headers and .pc files is required.

 😤 You chose... system Python? That was unwise.
=========================================================
    ])
    AC_MSG_ERROR([no usable Python interpreter found])
  fi
  AC_MSG_RESULT([$python_ver])
  AC_SUBST([PYTHON_VERSION], [$python_ver])

  # Check if we're in a conda environment
  AC_MSG_CHECKING([if using conda])
  if $PYTHON -c 'import os, sys; print("yes" if "CONDA_PREFIX" in os.environ else "no")' 2>/dev/null | grep -q yes; then
    AC_MSG_RESULT([yes])
    USING_CONDA="yes"
  else
    AC_MSG_RESULT([no])
    USING_CONDA="no"
  fi

  # Check if we're in a pip virtual environment (if not using conda)
  if test "$USING_CONDA" = "no"; then
    AC_MSG_CHECKING([if using pip virtualenv])
    if $PYTHON -c 'import sys; print("yes" if hasattr(sys, "real_prefix") or (hasattr(sys, "base_prefix") and sys.base_prefix != sys.prefix) else "no")' 2>/dev/null | grep -q yes; then
      AC_MSG_RESULT([yes])
      USING_VENV="yes"
    else
      AC_MSG_RESULT([no])
      USING_VENV="no"
    fi
  fi

  AC_SUBST([USING_CONDA])
  AC_SUBST([USING_VENV])

  # Create fallback requirements.txt from environment.yml.in if needed
  if test "$USING_CONDA" = "yes"; then
    :
  else
    AC_MSG_NOTICE([Creating fallback requirements.txt])
    cat > requirements.txt <<EOF
# Auto-generated fallback pip requirements
python==@PYTHON_VERSION@
guile
cairo
gtk2
gettext
glib
pango
gdk-pixbuf
pkgconfig
EOF
  fi
])
