# ===========================================================================
#   ax_geda_python_environment_check.m4 - Require a working Python3 env
# ===========================================================================
# Copyleft  2025 John Ryan  maintainer geda-ai
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
# ===========================================================================

AC_DEFUN([AX_GEDA_PYTHON_ENVIRONMENT_CHECK], [
  AC_MSG_CHECKING([if running inside a conda environment])
  if test -n "$CONDA_PREFIX"; then
    AC_MSG_RESULT([yes ($CONDA_PREFIX)])
    PKG_CONFIG_PATH="$CONDA_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
    export PKG_CONFIG_PATH
  else
    AC_MSG_RESULT([no])
    AC_MSG_WARN([
=========================================================
 🤔 It appears you are not in a conda environment.

 You may run into problems unless you activate one:
   conda create -n geda-ai python=3.9
   conda activate geda-ai
   pip install -r requirements.txt
 Then re-run:
   ./configure --prefix=$HOME/geda

 System Python lacks headers and .pc files. Avoid it. 😤
=========================================================
    ])
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

 🐍 A Python with headers and .pc files is required.

 😤 You chose... system Python? That was unwise.
=========================================================
    ])
    AC_MSG_ERROR([no usable Python interpreter found])
  fi
  AC_MSG_RESULT([$python_ver])
  AC_SUBST([PYTHON_VERSION], [$python_ver])
])