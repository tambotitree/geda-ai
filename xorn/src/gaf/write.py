# gaf - Python library for manipulating gEDA files
# Copyright (C) 1998-2010 Ales Hvezda
# Copyright (C) 1998-2010 gEDA Contributors (see ChangeLog for details)
# Copyright (C) 2013-2020 Roland Lutz
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

## \namespace gaf.write
## Writing schematic/symbol files.

import xorn.fileutils
import gaf.fileformat
import gaf.plainwrite
import gaf.xmlwrite

## Save a symbol or schematic file.
#
# See \ref xorn.fileutils.write for a description of the keyword
# arguments.
#
# \returns \c None
#
# \throws gaf.fileformat.UnknownFormatError
# \throws IOError, OSError if a filesystem error occurred
# \throws ValueError       if an object with an unknown type is encountered

def write(rev, filename, format = None, write_kwds = {}, **format_kwds):
    if format is None:
        format = gaf.fileformat.guess_format(filename)

    def write_func(f):
        write_file(f, rev, format, **format_kwds)

    xorn.fileutils.write(filename, write_func, **write_kwds)

## Write a symbol or schematic to a file.
#
# \param [in] f    A file-like object to which to write
# \param [in] rev  The symbol or schematic which should be written
#
# \returns \c None
#
# \throws ValueError if \a format is not a valid file format
# \throws ValueError if an object with an unknown type is encountered

def write_file(f, rev, format, **kwds):
    if format == gaf.fileformat.FORMAT_SYM or \
       format == gaf.fileformat.FORMAT_SCH:
        return gaf.plainwrite.write_file(f, rev, **kwds)
    if format == gaf.fileformat.FORMAT_SYM_XML or \
       format == gaf.fileformat.FORMAT_SCH_XML:
        return gaf.xmlwrite.write_file(
            f, rev, format == gaf.fileformat.FORMAT_SYM_XML, **kwds)
    raise ValueError
