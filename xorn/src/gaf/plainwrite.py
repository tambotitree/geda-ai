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

## \namespace gaf.plainwrite
## Writing gEDA schematic/symbol files.

from gettext import gettext as _
import xorn.fileutils
import xorn.proxy
import xorn.storage
import xorn.base64
import gaf.plainformat

## Current gEDA \c PACKAGE_DATE_VERSION.

RELEASE_VERSION = 20121203

## Current schematic/symbol file format version.

FILEFORMAT_VERSION = 2

## Return the gEDA file header string.
#
# This function simply returns the DATE_VERSION and FILEFORMAT_VERSION
# formatted as a gEDA file header.

def file_format_header():
    return 'v %s %u\n' % (RELEASE_VERSION, FILEFORMAT_VERSION)

## Write a symbol or schematic to a file in libgeda format.
#
# \param [in] f    A file-like object to which to write
# \param [in] rev  The symbol or schematic which should be written
#
# \returns \c None
#
# \throws ValueError if an object with an unknown type is encountered

def write_file(f, rev):
    f.write(file_format_header())
    for ob in rev.toplevel_objects():
        write_object(f, ob)

## Format a line style to a string.

def format_line(line):
    if line.dash_style == 0:
        return '%d %d %d -1 -1' % (line.width,
                                   line.cap_style,
                                   line.dash_style)
    if line.dash_style == 1:
        return '%d %d %d -1 %d' % (line.width,
                                   line.cap_style,
                                   line.dash_style,
                                   line.dash_space)

    return '%d %d %d %d %d' % (line.width,
                               line.cap_style,
                               line.dash_style,
                               line.dash_length,
                               line.dash_space)

## Format a fill style to a string.

def format_fill(fill):
    if fill.type == 2:
        return '%d %d %d %d %d %d' % (fill.type,
                                      fill.width,
                                      fill.angle0,
                                      fill.pitch0,
                                      fill.angle1,
                                      fill.pitch1)
    if fill.type == 3:
        return '%d %d %d %d -1 -1' % (fill.type,
                                      fill.width,
                                      fill.angle0,
                                      fill.pitch0)

    return '%d -1 -1 -1 -1 -1' % fill.type

## Return the libgeda ripper direction of a bus object.
#
# Search the schematic for ripper components connecting to the bus.
# If rippers are found and are oriented in the same direction, return
# \c -1 or \c 1 depending on the orientation, otherwise return \c 0.

def bus_ripper_direction(bus_ob):
    bus_data = bus_ob.data()
    found_neg = False
    found_pos = False
    for ob in xorn.proxy.RevisionProxy(bus_ob.rev).toplevel_objects():
        data = ob.data()
        if not isinstance(data, xorn.storage.Component) \
               or data.symbol.basename != 'busripper-1.sym':
            continue

        if data.angle == 0:
            x = data.x + 200.
            y = data.y + 200.
        elif data.angle == 90:
            x = data.x - 200.
            y = data.y + 200.
        elif data.angle == 180:
            x = data.x - 200.
            y = data.y - 200.
        elif data.angle == 270:
            x = data.x + 200.
            y = data.y - 200.
        else:
            continue  # invalid angle

        # check for vertical bus
        if bus_data.width == 0 and bus_data.x == x \
               and y >= min(bus_data.y, bus_data.y + bus_data.height) \
               and y <= max(bus_data.y, bus_data.y + bus_data.height):
            if data.angle == 0 or data.angle == 90:
                found_pos = 1
            else:
                found_neg = 1

        # check for horizontal bus
        if bus_data.height == 0 and bus_data.y == y \
               and x >= min(bus_data.x, bus_data.x + bus_data.width) \
               and x <= max(bus_data.x, bus_data.x + bus_data.width):
            if data.angle == 0 or data.angle == 270:
                found_pos = 1
            else:
                found_neg = 1

    if found_neg and found_pos:
        # Found inconsistent ripperdir for bus. Setting to 0.
        return 0
    if found_neg:
        return -1
    if found_pos:
        return 1
    return 0

## Write an object to a file in libgeda format.
#
# It follows the post-20000704 release file format that handles the
# line type and fill options.
#
# \param [in] f   A file-like object to which to write.
# \param [in] ob  The object which should be written.
#
# \return \c None.
#
# \throw ValueError if an object with an unknown type is encountered

def write_object(f, ob):
    data = ob.data()

    if isinstance(data, xorn.storage.Line):
        f.write('%c %d %d %d %d %d %s\n' % (
                gaf.plainformat.OBJ_LINE,
                data.x,
                data.y,
                data.x + data.width,
                data.y + data.height,
                data.color,
                format_line(data.line)))
    elif isinstance(data, xorn.storage.Net):
        if data.is_pin:
            if data.is_inverted:
                x0, y0 = data.x + data.width, data.y + data.height
                x1, y1 = data.x, data.y
            else:
                x0, y0 = data.x, data.y
                x1, y1 = data.x + data.width, data.y + data.height

            f.write('%c %d %d %d %d %d %d %d\n' % (
                    gaf.plainformat.OBJ_PIN,
                    x0, y0, x1, y1,
                    data.color,
                    data.is_bus,
                    data.is_inverted))
        elif data.is_bus:
            f.write('%c %d %d %d %d %d %d\n' % (
                    gaf.plainformat.OBJ_BUS,
                    data.x,
                    data.y,
                    data.x + data.width,
                    data.y + data.height,
                    data.color,
                    bus_ripper_direction(ob)))
        else:
            f.write('%c %d %d %d %d %d\n' % (
                    gaf.plainformat.OBJ_NET,
                    data.x,
                    data.y,
                    data.x + data.width,
                    data.y + data.height,
                    data.color))
    elif isinstance(data, xorn.storage.Box):
        f.write('%c %d %d %d %d %d %s %s\n' % (
                gaf.plainformat.OBJ_BOX,
                data.x,
                data.y,
                data.width,
                data.height,
                data.color,
                format_line(data.line),
                format_fill(data.fill)))
    elif isinstance(data, xorn.storage.Circle):
        f.write('%c %d %d %d %d %s %s\n' % (
                gaf.plainformat.OBJ_CIRCLE,
                data.x,
                data.y,
                data.radius,
                data.color,
                format_line(data.line),
                format_fill(data.fill)))
    elif isinstance(data, xorn.storage.Component):
        if data.symbol.embedded:
            basename = 'EMBEDDED' + data.symbol.basename
        else:
            basename = data.symbol.basename

        f.write('%c %d %d %d %d %d %s\n' % (
                gaf.plainformat.OBJ_COMPLEX,
                data.x,
                data.y,
                data.selectable,
                data.angle,
                data.mirror,
                basename))

        if data.symbol.embedded:
            emb_rev = xorn.storage.Revision(data.symbol.prim_objs)
            gaf.plainformat.transform(
                emb_rev, data.x, data.y, data.angle, data.mirror)
            f.write('[\n')
            for emb_ob in xorn.proxy.RevisionProxy(emb_rev).toplevel_objects():
                write_object(f, emb_ob)
            f.write(']\n')
    elif isinstance(data, xorn.storage.Text):
        # string can have multiple lines (seperated by \n's)
        f.write('%c %d %d %d %d %d %d %d %d %d\n' % (
                gaf.plainformat.OBJ_TEXT,
                data.x,
                data.y,
                data.color,
                data.text_size,
                data.visibility,
                data.show_name_value,
                data.angle,
                data.alignment,
                data.text.count('\n') + 1))
        f.write(data.text + '\n')
    elif isinstance(data, xorn.storage.Path):
        f.write('%c %d %s %s %d\n' % (
                gaf.plainformat.OBJ_PATH,
                data.color,
                format_line(data.line),
                format_fill(data.fill),
                data.pathdata.count('\n') + 1))
        f.write(data.pathdata + '\n')
    elif isinstance(data, xorn.storage.Arc):
        f.write('%c %d %d %d %d %d %d %s\n' % (
                gaf.plainformat.OBJ_ARC,
                data.x,
                data.y,
                data.radius,
                data.startangle,
                data.sweepangle,
                data.color,
                format_line(data.line)))
    elif isinstance(data, xorn.storage.Picture):
        f.write('%c %d %d %d %d %d %d %d\n%s\n' % (
                gaf.plainformat.OBJ_PICTURE,
                data.x,
                data.y,
                data.width,
                data.height,
                data.angle,
                data.mirror,
                data.pixmap.embedded,
                data.pixmap.filename))

        if data.pixmap.embedded:
            xorn.base64.encode(f, data.pixmap.data, delim = '.')
    else:
        raise ValueError, \
            _("Encountered an object with unknown type %s") % type(data)

    # save any attributes
    attribs = ob.attached_objects()
    if attribs:
        f.write('{\n')
        for attrib in attribs:
            write_object(f, attrib)
        f.write('}\n')
