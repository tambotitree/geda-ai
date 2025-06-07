# Python Type Bindings for Xorn Storage

This directory contains Python C extension bindings for schematic objects.  
Each C file corresponds to a specific schematic type and defines a `PyTypeObject`  
that is exposed as a type under the `xorn.storage` module.

## Defined Types

| C File | `PyTypeObject` | Python Type |
|--------|----------------|--------------|
| `data_arc.c` | `ArcType` | `xorn.storage.Arc` |
| `data_box.c` | `BoxType` | `xorn.storage.Box` |
| `data_circle.c` | `CircleType` | `xorn.storage.Circle` |
| `data_component.c` | `ComponentType` | `xorn.storage.Component` |
| `data_fillattr.c` | `FillAttrType` | `xorn.storage.FillAttr` |
| `data_line.c` | `LineType` | `xorn.storage.Line` |
| `data_lineattr.c` | `LineAttrType` | `xorn.storage.LineAttr` |
| `data_net.c` | `NetType` | `xorn.storage.Net` |
| `data_path.c` | `PathType` | `xorn.storage.Path` |
| `data_picture.c` | `PictureType` | `xorn.storage.Picture` |
| `data_text.c` | `TextType` | `xorn.storage.Text` |