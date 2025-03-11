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

## \namespace gaf.fileformat
## File formats.

import os.path

FORMAT_SCH, FORMAT_SYM, FORMAT_SCH_XML, FORMAT_SYM_XML = xrange(4)

VALID_FORMATS = {
    'sch': FORMAT_SCH,
    'sym': FORMAT_SYM,
    'schxml': FORMAT_SCH_XML,
    'symxml': FORMAT_SYM_XML
}

## Raised when the format of a file isn't specified and can't be
## guessed from the file name.

class UnknownFormatError(Exception):
    pass

def guess_format(path):
    basename = os.path.basename(path).lower()
    if basename.endswith('.sch'):
        return FORMAT_SCH
    if basename.endswith('.sym'):
        return FORMAT_SYM
    if basename.endswith('.sch.xml'):
        return FORMAT_SCH_XML
    if basename.endswith('.sym.xml'):
        return FORMAT_SYM_XML
    raise UnknownFormatError
