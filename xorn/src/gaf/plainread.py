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

## \namespace gaf.plainread
## Reading gEDA schematic/symbol files.
#
# The gEDA file format is a space separated list of characters and
# numbers in plain ASCII.  The meaning of each item is described in
# the file format documentation which can be found in \c
# doc/geda/file_format_spec or
# [here](http://wiki.geda-project.org/geda:file_format_spec).

import codecs, os, sys
from gettext import gettext as _
import xorn.base64
import xorn.proxy
import xorn.storage
import gaf.attrib
import gaf.read
import gaf.ref
from gaf.plainformat import *

## Describes the properties of a gEDA schematic/symbol file format version.
#
# The file format version of a gEDA file is described by a line of the form
#
# \verbatim
# v release_version fileformat_version
# \endverbatim
#
# where \a release_version is an eight-digit number representing a
# date and \c fileformat_version is a low integer (usually \c 1 or \c
# 2).  These represent the file format features that are used:
#
# Feature                            | Release version | File format version |
# -------------------------------------------|------------------------|------|
# Text alignment attribute                   | >= February 20th, 2000 |      |
# Line and fill attributes                   | > September 4th, 2000  |      |
# Bus pins, whichend and ripperdir attribute | > August 25th, 2002    |      |
# Text objects with multiple lines           | (October 2003)         | >= 1 |
# Path objects                               | (November 2008)        | >= 2 |
#
# Depending on the version of the file format, the file is parsed
# differently.  The unspecified parameters in the older file formats
# are set to default values.
#
# In older libgeda file format versions there was no information about
# the active end of pins.

class FileFormat:
    ## Create a new instance from a version number pair and derive its
    ## properties.
    def __init__(self, release_ver, fileformat_ver):
        ## libgeda release version number
        self.release_ver = release_ver
        ## libgeda file format version number
        self.fileformat_ver = fileformat_ver

        ## Is the text alignment attribute supported?
        self.supports_text_alignment = release_ver >= VERSION_20000220
        # yes, above greater or equal (not just greater) is correct.
        # The format change occurred in 20000220

        ## Are line and fill attributes supported?
        self.supports_linefill_attributes = release_ver > VERSION_20000704

        ## Are bus pins, whichend and ripperdir attributes supported?
        self.enhanced_pinbus_format = release_ver > VERSION_20020825

        ## Can text objects have multiple lines?
        self.supports_multiline_text = fileformat_ver >= 1

## Helper function for \ref sscanf.

def parse_token(s, fmt):
    if fmt == '%c':
        if len(s) != 1:
            raise ValueError
        return s

    if fmt == '%d':
        return int(s)

    if fmt == '%u':
        val = int(s)
        if val < 0:
            raise ValueError
        return val

    if fmt == '%s':
        return s

    raise ValueError, "Invalid format token: '%s'" % fmt

## Parse a string of space-separated values.
#
# This is a mock-up version of the standard \c sscanf(3).  The format
# string must consist of zero or more tokens separated by a space,
# optionally followed by a newline character.  The format string must
# exactly match this pattern.  Only the tokens \c %%c, \c %%d, \c %%s,
# and \c %%u are allowed.
#
# \throw ValueError if the string does not match the format
# \throw ValueError if an invalid format token is passed
#
# \return a tuple containing the parsed values

def sscanf(s, fmt):
    while fmt.endswith('\n'):
        if not s.endswith('\n'):
            raise ValueError
        fmt = fmt[:-1]
        s = s[:-1]

    if s.endswith('\n'):
        raise ValueError

    # str.split without an argument treats any whitespace character as
    # a separator and removes empty strings from the result
    stok = s.split()
    # but gEDA/gaf doesn't allow trailing whitespace
    if stok and stok[0][0] != s[0]:
        raise ValueError

    fmttok = fmt.split(' ')

    # gEDA/gaf ignores extra fields
    if len(stok) > len(fmttok):
        del stok[len(fmttok):]

    if len(stok) != len(fmttok):
        raise ValueError

    return [parse_token(st, ft) for (st, ft) in zip(stok, fmttok)]

## Replace "\r\n" line endings with "\n" line endings.

def strip_carriage_return(f):
    for line in f:
        if line.endswith('\r\n'):
            yield line[:-2] + '\n'
        else:
            yield line

## Read a symbol or schematic file in libgeda format from a file object.
#
# \param [in] f                   A file-like object from which to read
# \param [in] name                The file name displayed in warning and
#                                 error messages
# \param [in] log                 An object to which errors are logged
# \param [in] load_symbol         Function for loading referenced symbol files
# \param [in] load_pixmap         Function for loading referenced pixmap files
# \param [in] force_boundingbox   <i>currently unused</i>
#
# \returns a transient xorn.proxy.RevisionProxy instance containing
#          the file's contents
#
# \throws gaf.read.ParseError if the file is not a valid gEDA
#                             schematic/symbol file

def read_file(f, name, log, load_symbol, load_pixmap,
              force_boundingbox = False):
    f = codecs.iterdecode(f, 'utf-8')
    f = strip_carriage_return(f)

    def lineno_incrementor(f):
        for line in f:
            yield line
            log.lineno += 1
    f = lineno_incrementor(f)

    # "Stack" of outer contexts for embedded components
    object_lists_save = []

    # Last read object.  Attributes and embedded components attach to this.
    ob = None

    # This is where read objects end up.  Will be swapped for embedded comps.
    rev = xorn.storage.Revision()

    format = FileFormat(0, 0)  # no file format definition at all

    for line in f:
        if not line:
            continue
        objtype = line[0]

        if objtype == OBJ_LINE:
            data = read_line(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_NET:
            data = read_net(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_BUS:
            data = read_bus(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_BOX:
            data = read_box(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_PICTURE:
            data = read_picture(line, f, format, log, load_pixmap)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_CIRCLE:
            data = read_circle(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_COMPLEX:
            data = read_complex(line, format, log, load_symbol)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_TEXT:
            data = read_text(line, f, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_PATH:
            data = read_path(line, f, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_PIN:
            data = read_pin(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == OBJ_ARC:
            data = read_arc(line, format, log)
            if data is not None:
                ob = rev.add_object(data)
        elif objtype == STARTATTACH_ATTR:
            if ob is None:
                log.error(_("unexpected attribute list start marker"))
                continue
            if not isinstance(rev.get_object_data(ob), xorn.storage.Net) and \
               not isinstance(rev.get_object_data(ob), xorn.storage.Component):
                log.error(_("can't attach attributes to this object type"))
                continue

            while True:
                try:
                    line = f.next()
                except StopIteration:
                    log.error(_("unterminated attribute list"))
                    break

                if not line:
                    continue

                if line[0] == ENDATTACH_ATTR:
                    break

                if line[0] != OBJ_TEXT:
                    log.error(
                        _("tried to attach a non-text item as an attribute"))
                    continue

                attrib = read_text(line, f, format, log)
                if attrib is not None:
                    rev.relocate_object(rev.add_object(attrib), ob, None)

            ob = None
        elif objtype == START_EMBEDDED:
            if ob is None:
                log.error(_("unexpected embedded symbol start marker"))
                continue
            component_data = rev.get_object_data(ob)
            if type(component_data) != xorn.storage.Component:
                log.error(_("embedded symbol start marker following "
                            "non-component object"))
                continue
            if not component_data.symbol.embedded:
                log.error(_("embedded symbol start marker following "
                            "component with non-embedded symbol"))
                continue
            if component_data.symbol.prim_objs is not None:
                log.error(_("embedded symbol start marker following "
                            "embedded symbol"))
                continue
            object_lists_save.append((rev, ob))
            rev = xorn.storage.Revision()
            component_data.symbol.prim_objs = rev
        elif objtype == END_EMBEDDED:
            if not object_lists_save:
                log.error(_("unexpected embedded symbol end marker"))
                continue
            rev, ob = object_lists_save.pop()
        elif objtype == ENDATTACH_ATTR:
            log.error(_("unexpected attribute list end marker"))
        elif objtype == INFO_FONT:
            # NOP
            pass
        elif objtype == COMMENT:
            # do nothing
            pass
        elif objtype == VERSION_CHAR:
            try:
                objtype, release_ver, fileformat_ver = \
                    sscanf(line, "%c %u %u\n")
            except ValueError:
                try:
                    objtype, release_ver = sscanf(line, "%c %u\n")
                except ValueError:
                    log.error(_("failed to parse version string"))
                    continue
                fileformat_ver = 0

            assert objtype == VERSION_CHAR

            # 20030921 was the last version which did not have a fileformat
            # version.
            if release_ver <= VERSION_20030921:
                fileformat_ver = 0

            if fileformat_ver == 0:
                log.warn(_("Read an old format sym/sch file! "
                           "Please run g[sym|sch]update on this file"))

            format = FileFormat(release_ver, fileformat_ver)
        else:
            log.error(_("read garbage"))

    for ob in rev.get_objects():
        data = rev.get_object_data(ob)
        if not isinstance(data, xorn.storage.Component) \
               or not data.symbol.embedded:
            continue
        if data.symbol.prim_objs is None:
            log.error(_("embedded symbol is missing"))
            continue

        # rotate/translate objects back to normal
        gaf.plainformat.untransform(
            data.symbol.prim_objs, data.x, data.y, data.angle, data.mirror)

        # un-hide overwritten attributes in embedded symbol
        ob = xorn.proxy.ObjectProxy(rev, ob)
        visibility = {}
        for attached in gaf.attrib.find_attached_attribs(ob):
            attached_name, attached_value = \
                gaf.attrib.parse_string(attached.text)
            visibility[attached_name] = attached.visibility
        for inherited in gaf.attrib.find_inherited_attribs(ob):
            inherited_name, inherited_value = \
                gaf.attrib.parse_string(inherited.text)
            if inherited_name in visibility:
                inherited.visibility = visibility[inherited_name]

    if not format.enhanced_pinbus_format:
        pin_update_whichend(rev, force_boundingbox, log)

    return xorn.proxy.RevisionProxy(rev)

## Guess the orientation of pins.
#
# Calculates the bounding box of all pins in the revision.  The end of
# a pin that is closer to the boundary of the box is set as the active
# end.
#
# \return \c None.
#
# \warning This function is not implemented.  See Xorn bug #148.

def pin_update_whichend(rev, force_boundingbox, log):
    for ob in rev.get_objects():
        data = rev.get_object_data(ob)
        if isinstance(data, xorn.storage.Net) and data.is_pin:
            log.error(_("file is lacking pin orientation information"))
            break


## Construct a line attribute object.
#
# The line parameters which are not used for the specified dash style
# are set to \c 0.

def normalized_line(width, cap_style, dash_style, dash_length, dash_space):
    if dash_style == 0 or dash_style == 1:
        dash_length = 0.
    if dash_style == 0:
        dash_space = 0.
    return xorn.storage.LineAttr(
        width, cap_style, dash_style, dash_length, dash_space)

## Construct a fill attribute object.
#
# The fill parameters which are not used for the specified fill type
# are set to \c 0.

def normalized_fill(type, width, angle0, pitch0, angle1, pitch1):
    if type != 2 and type != 3:
        width = 0.
        angle0 = 0
        pitch0 = 0.
    if type != 2:
        angle1 = 0
        pitch1 = 0.
    return xorn.storage.FillAttr(
        type, width, angle0, pitch0, angle1, pitch1)

## Read a circle object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a circle object

def read_circle(buf, format, log):
    try:
        if not format.supports_linefill_attributes:
            type, x1, y1, radius, color = sscanf(buf, "%c %d %d %d %d\n")
            circle_width = 0
            circle_end = 0
            circle_type = 0
            circle_length = -1
            circle_space = -1

            circle_fill = 0
            fill_width = 0
            angle1 = -1
            pitch1 = -1
            angle2 = -1
            pitch2 = -1
        else:
            type, x1, y1, radius, color, circle_width, \
            circle_end, circle_type, circle_length, circle_space, \
            circle_fill, fill_width, angle1, pitch1, angle2, pitch2 = sscanf(
                buf, "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse circle object"))
        return None

    if type != OBJ_CIRCLE:
        raise ValueError

    if radius == 0:
        log.warn(_("circle has radius zero"))
    elif radius < 0:
        log.warn(_("circle has negative radius (%d), setting to 0") % radius)
        radius = 0

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("circle has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    return xorn.storage.Circle(
        x = x1,
        y = y1,
        radius = radius,
        color = color,
        line = normalized_line(
            width = circle_width,
            cap_style = circle_end,
            dash_style = circle_type,
            dash_length = circle_length,
            dash_space = circle_space),
        fill = normalized_fill(
            type = circle_fill,
            width = fill_width,
            angle0 = angle1,
            pitch0 = pitch1,
            angle1 = angle2,
            pitch1 = pitch2))

## Read an arc object from a string in gEDA format.
#
# A negative or null radius is not allowed.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe an arc object

def read_arc(buf, format, log):
    try:
        if not format.supports_linefill_attributes:
            type, x1, y1, radius, start_angle, sweep_angle, color = sscanf(
                buf, "%c %d %d %d %d %d %d\n")
            arc_width = 0
            arc_end = 0
            arc_type = 0
            arc_space = -1
            arc_length = -1
        else:
            type, x1, y1, radius, start_angle, sweep_angle, color, \
            arc_width, arc_end, arc_type, arc_length, arc_space = sscanf(
                buf, "%c %d %d %d %d %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse arc object"))
        return None

    if type != OBJ_ARC:
        raise ValueError

    if radius == 0:
        log.warn(_("arc has radius zero"))
    elif radius < 0:
        log.warn(_("arc has negative radius (%d), setting to 0") % radius)
        radius = 0

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("arc has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    return xorn.storage.Arc(
        x = x1,
        y = y1,
        radius = radius,
        startangle = start_angle,
        sweepangle = sweep_angle,
        color = color,
        line = normalized_line(
            width = arc_width,
            cap_style = arc_end,
            dash_style = arc_type,
            dash_length = arc_length,
            dash_space = arc_space))

## Read a box object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a box object

def read_box(buf, format, log):
    try:
        if not format.supports_linefill_attributes:
            type, x1, y1, width, height, color = sscanf(
                buf, "%c %d %d %d %d %d\n")
            box_width = 0
            box_end = 0
            box_type = 0
            box_length = -1
            box_space = -1

            box_filling = 0
            fill_width = 0
            angle1 = -1
            pitch1 = -1
            angle2 = -1
            pitch2 = -1
        else:
            type, x1, y1, width, height, color, \
            box_width, box_end, box_type, box_length, box_space, \
            box_filling, fill_width, angle1, pitch1, angle2, pitch2 = sscanf(
                buf, "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse box object"))
        return None



    if type != OBJ_BOX:
        raise ValueError

    if width == 0 or height == 0:
        log.warn(_("box has width/height zero"))

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("box has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    # In libgeda, a box is internally represented by its lower right
    # and upper left corner, whereas in the file format, it is
    # described as its lower left corner and its width and height.
    #
    # We don't care and just use the file format representation.

    return xorn.storage.Box(
        x = x1,
        y = y1,
        width = width,
        height = height,
        color = color,
        line = normalized_line(
            width = box_width,
            cap_style = box_end,
            dash_style = box_type,
            dash_length = box_length,
            dash_space = box_space),
        fill = normalized_fill(
            type = box_filling,
            width = fill_width,
            angle0 = angle1,
            pitch0 = pitch1,
            angle1 = angle2,
            pitch1 = pitch2))

## Read a bus object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a bus object

def read_bus(buf, format, log):
    try:
        if not format.enhanced_pinbus_format:
            type, x1, y1, x2, y2, color = sscanf(
                buf, "%c %d %d %d %d %d\n")
            ripper_dir = 0
        else:
            type, x1, y1, x2, y2, color, ripper_dir = sscanf(
                buf, "%c %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse bus object"))
        return None

    if type != OBJ_BUS:
        raise ValueError

    if x1 == x2 and y1 == y2:
        log.warn(_("bus has length zero"))

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("bus has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    if ripper_dir < -1 or ripper_dir > 1:
        log.warn(_("bus has invalid ripper direction (%d)") % ripper_dir)
        ripper_dir = 0  # isn't used

    return xorn.storage.Net(
        x = x1,
        y = y1,
        width = x2 - x1,
        height = y2 - y1,
        color = color,
        is_bus = True,
        is_pin = False,
        is_inverted = False)

## Read a component object from a string in gEDA format.
#
# If the symbol is not embedded, use the function \a load_symbol to
# load it from the component library.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a component object

def read_complex(buf, format, log, load_symbol):
    try:
        type, x1, y1, selectable, angle, mirror, basename = sscanf(
            buf, "%c %d %d %d %d %d %s\n")
    except ValueError:
        log.error(_("failed to parse complex object"))
        return None

    if type != OBJ_COMPLEX:
        raise ValueError

    if angle not in [0, 90, 180, 270]:
        log.warn(_("component has invalid angle (%d), setting to 0") % angle)
        angle = 0

    if mirror != 0 and mirror != 1:
        log.warn(_("component has invalid mirror flag (%d), "
                   "setting to 0") % mirror)
        mirror = 0

    # color = DEFAULT_COLOR

    if basename.startswith('EMBEDDED'):
        symbol = gaf.ref.Symbol(basename[8:], None, True)
    else:
        symbol = load_symbol(basename, False)
        assert not symbol.embedded

    return xorn.storage.Component(
        x = x1,
        y = y1,
        selectable = selectable,
        angle = angle,
        mirror = mirror,
        symbol = symbol)

## Read a line object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a line object

def read_line(buf, format, log):
    try:
        if not format.supports_linefill_attributes:
            type, x1, y1, x2, y2, color = sscanf(buf, "%c %d %d %d %d %d\n")
            line_width = 0
            line_end = 0
            line_type = 0
            line_length = -1
            line_space = -1
        else:
            type, x1, y1, x2, y2, color, \
            line_width, line_end, line_type, line_length, line_space = sscanf(
                buf, "%c %d %d %d %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse line object"))
        return None

    if type != OBJ_LINE:
        raise ValueError

    # Null length line are not allowed.  If such a line is detected, a
    # message is issued.
    if x1 == x2 and y1 == y2:
        log.warn(_("line has length zero"))

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("line has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    return xorn.storage.Line(
        x = x1,
        y = y1,
        width = x2 - x1,
        height = y2 - y1,
        color = color,
        line = normalized_line(
            width = line_width,
            cap_style = line_end,
            dash_style = line_type,
            dash_length = line_length,
            dash_space = line_space))

## Read a net object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a net object

def read_net(buf, format, log):
    try:
        type, x1, y1, x2, y2, color = sscanf(buf, "%c %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse net object"))
        return None

    if type != OBJ_NET:
        raise ValueError

    if x1 == x2 and y1 == y2:
        log.warn(_("net has length zero"))

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("net has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    return xorn.storage.Net(
        x = x1,
        y = y1,
        width = x2 - x1,
        height = y2 - y1,
        color = color,
        is_bus = False,
        is_pin = False,
        is_inverted = False)

## Read a path object from a string and a file in gEDA format.
#
# Creates a path object from the string \a first_line and reads as
# many lines describing the path as specified there from \a f.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw gaf.read.ParseError if not enough lines could be read from the file
# \throw ValueError          if \a first_line doesn't describe a path object

def read_path(first_line, f, format, log):
    try:
        type, color, \
        line_width, line_end, line_type, line_length, line_space, \
        fill_type, fill_width, angle1, pitch1, angle2, pitch2, \
        num_lines = sscanf(
            first_line, "%c %d %d %d %d %d %d %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse path object"))
        return None

    if type != OBJ_PATH:
        raise ValueError

    # Checks if the required color is valid.
    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("path has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    pathstr = ''
    for i in xrange(0, num_lines):
        try:
            line = f.next()
        except StopIteration:
            log.error(_("unexpected end of file after %d lines "
                        "while reading path") % i)
            break

        pathstr += line

    if pathstr.endswith('\n'):
        pathstr = pathstr[:-1]

    return xorn.storage.Path(
        pathdata = pathstr.encode('utf-8'),
        color = color,
        line = normalized_line(
            width = line_width,
            cap_style = line_end,
            dash_style = line_type,
            dash_length = line_length,
            dash_space = line_space),
        fill = normalized_fill(
            type = fill_type,
            width = fill_width,
            angle0 = angle1,
            pitch0 = pitch1,
            angle1 = angle2,
            pitch1 = pitch2))

## Read a picture object from a string and a file in gEDA format.
#
# Creates a picture object from the string \a first_line.  If the
# pixmap is not embedded, uses the function \a load_pixmap to load it
# from an external file.  If the pixmap is embedded, reads its data in
# base64 encoding from \a f.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw gaf.read.ParseError if the picture data could be read from the file
# \throw ValueError          if \a first_line doesn't describe a picture object

def read_picture(first_line, f, format, log, load_pixmap):
    try:
        type, x1, y1, width, height, angle, mirrored, embedded = sscanf(
            first_line, "%c %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse picture definition"))
        return None

    if type != OBJ_PICTURE:
        raise ValueError

    if width == 0 or height == 0:
        log.warn(_("picture has width/height zero"))

    if mirrored != 0 and mirrored != 1:
        log.warn(_("picture has wrong 'mirrored' parameter (%d), "
                   "setting to 0") % mirrored)
        mirrored = 0

    if angle not in [0, 90, 180, 270]:
        log.warn(_("picture has unsupported angle (%d), setting to 0") % angle)
        angle = 0

    try:
        filename = f.next()
    except StopIteration:
        log.error(_("unexpected end of file while reading picture file name"))
        filename = ''
    else:
        if filename.endswith('\n'):
            filename = filename[:-1]

        # Handle empty filenames
        if not filename:
            log.warn(_("image has no filename"))
            filename = None

    if embedded != 1:
        if embedded != 0:
            log.warn(_("picture has wrong 'embedded' parameter (%d), "
                       "setting to not embedded") % embedded)
        pixmap = load_pixmap(filename, False)
        assert not pixmap.embedded
    else:
        pixmap = gaf.ref.Pixmap(filename, None, True)
        # Read the encoded picture
        try:
            pixmap.data = xorn.base64.decode(f, delim = '.')
        except xorn.base64.DecodingError as e:
            log.error(_("failed to load image from embedded data: %s")
                      % e.message)
            pixmap.data = ''

    return xorn.storage.Picture(
        x = x1,
        y = y1,
        width = width,
        height = height,
        angle = angle,
        mirror = mirrored,
        pixmap = pixmap)

## Read a pin object from a string in gEDA format.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw ValueError          if \a buf doesn't describe a pin object

def read_pin(buf, format, log):
    try:
        if not format.enhanced_pinbus_format:
            type, x1, y1, x2, y2, color = sscanf(buf, "%c %d %d %d %d %d\n")
            pin_type = 0
            whichend = -1
        else:
            type, x1, y1, x2, y2, color, pin_type, whichend = sscanf(
                buf, "%c %d %d %d %d %d %d %d\n")
    except ValueError:
        log.error(_("failed to parse pin object"))
        return None

    if type != OBJ_PIN:
        raise ValueError

    if whichend == -1:
        log.warn(_("pin does not have the whichone field set--"
                   "verify and correct manually!"))
    elif whichend < -1 or whichend > 1:
        log.warn(_("pin has invalid whichend (%d), "
                   "setting to first end") % whichend)
        whichend = 0

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("pin has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    if pin_type == 0:
        is_bus = False
    elif pin_type == 1:
        is_bus = True
    else:
        log.warn(_("pin has invalid type (%d), setting to 0") % pin_type)
        is_bus = False

    if whichend != 1:
        is_inverted = False
    else:
        x1, y1, x2, y2 = x2, y2, x1, y1
        is_inverted = True

    return xorn.storage.Net(
        x = x1,
        y = y1,
        width = x2 - x1,
        height = y2 - y1,
        color = color,
        is_bus = is_bus,
        is_pin = True,
        is_inverted = is_inverted)

## Read a text object from a string and a file in gEDA format.
#
# Creates a text object from the string \a first_line and reads as
# many lines of text as specified there from \a f.
#
# \throw gaf.read.ParseError if the string could not be parsed
# \throw gaf.read.ParseError if not enough lines could be read from the file
# \throw ValueError          if \a first_line doesn't describe a text object

def read_text(first_line, f, format, log):
    try:
        if format.supports_multiline_text:
            type, x, y, color, size, visibility, show_name_value, angle, \
            alignment, num_lines = sscanf(
                first_line, "%c %d %d %d %d %d %d %d %d %d\n")
        elif not format.supports_text_alignment:
            type, x, y, color, size, visibility, show_name_value, angle = \
                sscanf(first_line, "%c %d %d %d %d %d %d %d\n")
            alignment = LOWER_LEFT  # older versions didn't have this
            num_lines = 1           # only support a single line
        else:
            type, x, y, color, size, visibility, show_name_value, angle, \
            alignment = sscanf(
                first_line, "%c %d %d %d %d %d %d %d %d\n")
            num_lines = 1           # only support a single line
    except ValueError:
        log.error(_("failed to parse text object"))
        return None

    if type != OBJ_TEXT:
        raise ValueError

    if size == 0:
        log.warn(_("text has size zero"))

    if angle not in [0, 90, 180, 270]:
        log.warn(_("text has unsupported angle (%d), setting to 0") % angle)
        angle = 0

    if alignment not in [LOWER_LEFT, MIDDLE_LEFT, UPPER_LEFT,
                         LOWER_MIDDLE, MIDDLE_MIDDLE, UPPER_MIDDLE,
                         LOWER_RIGHT, MIDDLE_RIGHT, UPPER_RIGHT]:
        log.warn(_("text has unsupported alignment (%d), "
                   "setting to LOWER_LEFT") % alignment)
        alignment = LOWER_LEFT

    if color < 0 or color >= MAX_OBJECT_COLORS:
        log.warn(_("text has invalid color (%d), setting to %d")
                 % (color, DEFAULT_COLOR))
        color = DEFAULT_COLOR

    if num_lines <= 0:
        log.error(_("text has invalid number of lines (%d)") % num_lines)

    text = ''
    for i in xrange(0, num_lines):
        try:
            line = f.next()
        except StopIteration:
            log.error(_("unexpected end of file after %d lines of text") % i)
            break

        text += line

    if text.endswith('\n'):
        text = text[:-1]

    tmp = text.replace('\\\\', '')
    if tmp.count('\\_') % 2:
        log.warn(_("mismatched overbar markers"))
    if '\\' in tmp.replace('\\_', ''):
        log.warn(_("stray backslash character(s)"))

    return xorn.storage.Text(
        x = x,
        y = y,
        color = color,
        text_size = size,
        visibility = visibility,
        show_name_value = show_name_value,
        angle = angle,
        alignment = alignment,
        text = text.encode('utf-8'))
