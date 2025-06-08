# gaf.netlist - gEDA Netlist Extraction and Generation
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

import os
import sys
import importlib.util
from gettext import gettext as _

## Module name prefix for netlister backends.
BACKEND_PREFIX = 'gnet_'

## Backend load path.
load_path = []

def list_backends():
    """Get a sorted list of available netlister backends."""
    backend_names = set()

    for dir_name in load_path:
        try:
            with os.scandir(dir_name) as entries:
                for entry in entries:
                    if not entry.name.startswith(BACKEND_PREFIX):
                        continue
                    if entry.name.endswith('.py') and entry.is_file():
                        name = entry.name[len(BACKEND_PREFIX):-3]
                        backend_names.add(name)
        except OSError as e:
            sys.stderr.write(_("Can't open directory %s: %s\n") % (dir_name, e.strerror))
            continue

    return sorted(backend_names)

def load(backend_name):
    """Load a specific netlister backend by name."""
    module_name = BACKEND_PREFIX + backend_name

    for path in load_path:
        module_path = os.path.join(path, module_name + '.py')
        if os.path.exists(module_path):
            spec = importlib.util.spec_from_file_location(module_name, module_path)
            if spec is None or spec.loader is None:
                raise ImportError(f"Could not load backend '{module_name}'")

            module = importlib.util.module_from_spec(spec)
            sys.modules[module_name] = module

            spec.loader.exec_module(module)
            return module

    raise ImportError(f"Backend '{module_name}' not found in load_path.")