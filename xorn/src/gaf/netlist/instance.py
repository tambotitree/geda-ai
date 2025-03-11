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

## \namespace gaf.netlist.instance
## Building one hierarchical netlist from instantiated sheet blueprints.

import sys

## Instantiation of a schematic page.
#
# For every schematic instantiation (either via the command line, or
# via the \c source= attribute), a Sheet object is created, and its
# \a blueprint attribute is set to the instantiated
# gaf.netlist.blueprint.Schematic object.

class Sheet:
    def __init__(self, netlist, blueprint, instantiating_component):
        self.netlist = netlist
        # schematic which is traversed
        self.blueprint = blueprint

        self.instantiating_component = instantiating_component

        self.components = []
        self.components_by_blueprint = {}
        self.local_nets = []
        self.local_nets_by_blueprint = {}

        self.error = blueprint.error
        self.warn = blueprint.warn

        netlist.sheets.append(self)
        if instantiating_component is not None:
            instantiating_component.subsheets.append(self)

        # prefix which is attached to all component and net names
        # found in this schematic
        next_parent = self.instantiating_component
        namespace_parts = []
        while next_parent is not None:
            namespace_parts.insert(0, next_parent.blueprint.refdes)
            next_parent = next_parent.sheet.instantiating_component
        self.namespace = tuple(namespace_parts)

        # Starting internal netlist creation

        for net_blueprint in self.blueprint.nets:
            LocalNet(self, net_blueprint)

        for component_blueprint in self.blueprint.components:
            Component(self, component_blueprint)

## %Component.
#
# Represents a component in the netlist, either read from a toplevel
# schematic or instantiated via a subschematic symbol.

class Component:
    def __init__(self, sheet, blueprint):
        self.sheet = sheet
        self.blueprint = blueprint

        # set by netlist ctor
        self.refdes = None

        self.cpins = [] # list of ComponentPin
        self.cpins_by_blueprint = {}
        self.cpins_by_number = {}

        self.subsheets = []

        self.error = blueprint.error
        self.warn = blueprint.warn

        sheet.components.append(self)
        sheet.components_by_blueprint[blueprint] = self

        # create component pins
        for pin_blueprint in blueprint.pins:
            if pin_blueprint.ob is not None and pin_blueprint.ob.data().is_bus:
                # ignore bus pins
                continue

            ComponentPin(
                self, pin_blueprint,
                sheet.local_nets_by_blueprint[pin_blueprint.net])

## Pin on a component.
#
# One such object exists for every pin on a component in the netlist.

class ComponentPin:
    def __init__(self, component, blueprint, local_net):
        ## The component object to which this pin belongs.
        self.component = component

        ## The blueprint Pin object of which this is an instance.
        #
        # \c None for an artificial pin created via the \c "net=" attribute.
        self.blueprint = blueprint

        #self.is_bus = blueprint is not None and blueprint.ob.data().is_bus

        ## The LocalNet object to which this pin is connected.
        #
        # For real pins, this is just the net instance for the
        # blueprint pin's net
        self.local_net = local_net

        self.error = blueprint.error
        self.warn = blueprint.warn

        component.cpins.append(self)
        local_net.cpins.append(self)

        assert blueprint not in component.cpins_by_blueprint
        component.cpins_by_blueprint[blueprint] = self

        if blueprint.number is not None:
            component.cpins_by_number[blueprint.number] = self

class LocalNet:
    def __init__(self, sheet, blueprint):
        self.sheet = sheet
        self.blueprint = blueprint
        self.cpins = []
        self.net = None

        sheet.local_nets.append(self)
        sheet.local_nets_by_blueprint[blueprint] = self
