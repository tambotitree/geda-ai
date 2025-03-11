/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's Library
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef STRUCT_H
#define STRUCT_H

#include <glib.h>  /* Include needed to make GList work. */

/* Wrappers around a new list mechanism */
typedef struct _GedaList SELECTION;
typedef struct _GedaList GedaPageList;

/* gschem structures (gschem) */
typedef struct st_complex COMPLEX;
typedef struct st_line LINE;
typedef struct st_path_section PATH_SECTION;
typedef struct st_path PATH;
typedef struct st_circle CIRCLE;
typedef struct st_arc ARC;
typedef struct st_box BOX;
typedef struct st_picture PICTURE;
typedef struct st_text TEXT;
typedef struct st_point sPOINT;
typedef struct st_transform TRANSFORM;
typedef struct st_bezier BEZIER;

typedef struct st_object OBJECT;
typedef struct st_page PAGE;
typedef struct st_toplevel TOPLEVEL;
typedef struct st_color COLOR;
typedef struct st_undo UNDO;
typedef struct st_bounds BOUNDS;

typedef struct st_conn CONN;

/* netlist structures (gnetlist) */
typedef struct st_netlist NETLIST;
typedef struct st_cpinlist CPINLIST;
typedef struct st_net NET;

/* sch check structures (gschcheck) */
typedef struct st_schcheck SCHCHECK;
typedef struct st_chkerrs CHKERRS;

/* Managed text buffers */
typedef struct _TextBuffer TextBuffer;

/* Component library objects */
typedef struct _CLibSource CLibSource;
typedef struct _CLibSymbol CLibSymbol;

/* Component library search modes */
typedef enum { CLIB_EXACT=0, CLIB_GLOB } CLibSearchMode;

/* f_open behaviour flags.  See documentation for f_open_flags() in
   f_basic.c. */
typedef enum { F_OPEN_RC           = 1,
               F_OPEN_CHECK_BACKUP = 2,
               F_OPEN_RESTORE_CWD  = 4,
} FOpenFlags;

/*! \brief line end style for an open line of an object */
typedef enum {END_NONE, END_SQUARE, END_ROUND} OBJECT_END;

/*! \brief line style of lines, rect, circles, arcs */
typedef enum {TYPE_SOLID, TYPE_DOTTED, TYPE_DASHED, TYPE_CENTER, TYPE_PHANTOM} OBJECT_TYPE;

/*! \brief fill style of objects like cirle, rect, path */
typedef enum {FILLING_HOLLOW, FILLING_FILL, FILLING_MESH, FILLING_HATCH} OBJECT_FILLING;

struct st_line {
  int x[2];
  int y[2];
};

struct st_point {
  gint x;
  gint y;
};

#define LINE_END1 0
#define LINE_END2 1

typedef enum {
    PATH_MOVETO,
    PATH_MOVETO_OPEN,
    PATH_CURVETO,
    PATH_LINETO,
    PATH_END
} PATH_CODE;

struct st_path_section {
  PATH_CODE code;
  int x1;
  int y1;
  int x2;
  int y2;
  int x3;
  int y3;
};

struct st_path {
  PATH_SECTION *sections; /* Bezier path segments  */
  int num_sections;       /* Number with data      */
  int num_sections_max;   /* Number allocated      */
};

struct st_arc {
  int x, y; /* world */

  int radius;

  int start_angle;
  int sweep_angle;
};

#define ARC_CENTER 0
#define ARC_RADIUS 1
#define ARC_START_ANGLE 2
#define ARC_SWEEP_ANGLE 3

struct st_bezier {
  int x[4];
  int y[4];
};

struct st_box {
  /* upper is considered the origin */
  int upper_x, upper_y; /* world */	
  int lower_x, lower_y;

};

#define BOX_UPPER_LEFT 0
#define BOX_LOWER_RIGHT 1
#define BOX_UPPER_RIGHT 2
#define BOX_LOWER_LEFT 3

struct st_picture {
  GdkPixbuf *pixbuf;
  gchar *file_content;
  gsize file_length;

  double ratio;
  char *filename;
  int angle;
  char mirrored;
  char embedded;

  /* upper is considered the origin */
  int upper_x, upper_y; /* world */	
  int lower_x, lower_y;

};

#define PICTURE_UPPER_LEFT 0
#define PICTURE_LOWER_RIGHT 1
#define PICTURE_UPPER_RIGHT 2
#define PICTURE_LOWER_LEFT 3


struct st_text {
  int x, y;		/* world origin */

  char *string;			/* text stuff */
  char *disp_string;
  int length;
  int size;
  int alignment;	
  int angle;
};

struct st_complex {
  int x, y;		/* world origin */

  int angle;				/* orientation, only multiples
                                         * of 90 degrees allowed */   
  /* in degrees */
  int mirror;

  GList *prim_objs;			/* Primitive objects */
  /* objects which make up the */
  /* complex */
};

struct st_circle {
  int center_x, center_y; /* world */
  int radius;
};

#define CIRCLE_CENTER 0
#define CIRCLE_RADIUS 1

struct st_object {
  int type;				/* Basic information */
  int sid;
  char *name;

  PAGE *page; /* Parent page */

  int w_top;				/* Bounding box information */
  int w_left;				/* in world coords */
  int w_right;
  int w_bottom;
  TOPLEVEL *w_bounds_valid_for;

  COMPLEX *complex;
  LINE *line; 
  CIRCLE *circle; 
  ARC *arc;
  BOX *box;
  TEXT *text;
  PICTURE *picture;
  PATH *path;

  GList *conn_list;			/* List of connections */
  /* to and from this object */

  /* every graphical primitive have more or less the same options. */
  /* depending on its nature a primitive is concerned with one or more */
  /* of these fields. If not, value must be ignored. */
  OBJECT_END line_end;
  OBJECT_TYPE line_type;
  int line_width;
  int line_space;
  int line_length;

  OBJECT_FILLING fill_type;
  int fill_width;
  int fill_angle1, fill_pitch1;
  int fill_angle2, fill_pitch2;

  gboolean complex_embedded;                    /* is embedded component? */
  gchar *complex_basename;              /* Component Library Symbol name */
  OBJECT *parent;                       /* Parent object pointer */

  int color; 				/* Which color */
  int dont_redraw;			/* Flag to skip redrawing */
  int selectable;			/* object selectable flag */
  int selected;				/* object selected flag */
  int locked_color; 			/* Locked color (used to save */
  /* the object's real color */
  /* when the object is locked) */

  /* controls which direction bus rippers go */
  /* it is either 0 for un-inited, */
  /* 1 for right, -1 for left (horizontal bus) */
  /* 1 for up, -1 for down (vertial bus) */
  int bus_ripper_direction;             /* only valid on buses */


  int font_text_size;			/* used only with fonts defs */
  GList *font_prim_objs;			/* used only with fonts defs */

  int whichend;    /* for pins only, either 0 or 1 */
  int pin_type;    /* for pins only, either NET or BUS */

  GList *attribs;       /* attribute stuff */
  int show_name_value;
  int visibility;
  OBJECT *attached_to;  /* when object is an attribute */
  OBJECT *copied_to;    /* used when copying attributes */

  GList *weak_refs; /* Weak references */
}; 


/*! \brief Structure for connections between OBJECTs
 *
 * The st_conn structure contains a single connection
 * to another object.
 * The connection system in s_conn.c uses this struct
 */
struct st_conn {
  /*! \brief The "other" object connected to this one */
  OBJECT *other_object;
  /*! \brief type of connection. Always in reference to how the "other"
    object is connected to the current one */
  int type;
  /*! \brief x coord of the connection position */
  int x;
  /*! \brief y coord of the connection position */
  int y;		
  /*! \brief which endpoint of the current object caused this connection */
  int whichone;
  /*! \brief which endpoint of the "other" object caused this connection */
  int other_whichone;
};

struct st_bounds {
  gint min_x;
  gint min_y;
  gint max_x;
  gint max_y;
};

/** A structure to store a 2D affine transform.
 *
 *  The transforms get stored in a 3x3 matrix. Code assumes the bottom row to
 *  remain constant at [0 0 1].
 */
struct st_transform {
  gdouble m[2][3];    /* m[row][column] */
};

struct st_undo {

  /* one of these is used, depending on if you are doing in-memory */
  /* or file based undo state saving */	
  char *filename;
  GList *object_list;

  /* either UNDO_ALL or UNDO_VIEWPORT_ONLY */
  int type;

  /* viewport information */
  int x, y;
  double scale;

  /* up and down the hierarchy */
  int up;
  /* used to control which pages are viewable when moving around */
  int page_control;
  /* value of page->CHANGED */
  int CHANGED;

  /* textual description of the action */
  const char *desc;

  /* translation offset (used for origin placement if pan/zoom is disabled) */
  int tx, ty;

  UNDO *prev;
  UNDO *next;
};


struct st_page {

  TOPLEVEL* toplevel;
  int pid;

  GList *_object_list;
  SELECTION *selection_list; /* new selection mechanism */
  GList *place_list;
  OBJECT *object_lastplace; /* the last found item */
  GList *connectible_list;  /* connectible page objects */

  char *page_filename; 
  gboolean is_untitled;
  int CHANGED;			/* changed flag */
  gboolean exists_on_disk;
  struct timespec last_modified;

  gchar *patch_filename;
  gboolean patch_descend;
  gboolean patch_seen_on_disk;
  struct timespec patch_mtime;

  /* Undo/Redo Stacks and pointers */	
  /* needs to go into page mechanism actually */
  UNDO *undo_bottom;	
  UNDO *undo_current;
  UNDO *undo_tos; 	/* Top Of Stack */

  /* up and down the hierarchy */
  /* this holds the pid of the parent page */
  int up;
  /* int down; not needed */

  /* used to control which pages are viewable when moving around */
  int page_control;

  /* backup variables */
  char saved_since_first_loaded;
  gint ops_since_last_backup;
  gchar do_autosave_backup;

  GList *weak_refs; /* Weak references */
};

/*! \brief Type of callback function for calculating text bounds */
typedef int(*RenderedBoundsFunc)(void *, OBJECT *, int *, int *, int *, int *);

/*! \brief Type of callback function for object damage notification */
typedef int(*ChangeNotifyFunc)(void *, OBJECT *);

/*! \brief Type of callback function for querying loading of backups */
typedef gboolean(*LoadBackupQueryFunc)(void *, GString *);

/* structures below are for gnetlist */

/* for every component in the object database */
struct st_netlist {

  int nlid;

  char *component_uref;

  OBJECT *object_ptr;

  CPINLIST *cpins;		

  char *hierarchy_tag;
  int composite_component;

  NETLIST *prev;
  NETLIST *next;
};


/* for every pin on a component */
struct st_cpinlist {
  int plid;
  int type;                             /* PIN_TYPE_NET or PIN_TYPE_BUS */

  char *pin_number;
  char *net_name;			/* this is resolved at very end */
  char *pin_label;

  NET *nets;

  CPINLIST *prev;
  CPINLIST *next;
};

/* the net run connected to a pin */
struct st_net {

  int nid;

  int net_name_has_priority;
  char *net_name;
  char *pin_label;

  char *connected_to; /* new to replace above */

  NET *prev;
  NET *next;
};

/* By Jamil Khatib */
/* typedef struct st_chkerrs CHKERRS; */

/* Schem check struct */
struct st_schcheck {
  int no_errors;                /* No of Errors */
  int no_warnings;              /* No of Warinings */

  CHKERRS * sheet_errs;

  CHKERRS *float_nets;           /* Header of the list of floating nets */
  int net_errs;                 /* No of floating nets */

  OBJECT *float_pins;           /* Header of List of floating pins*/
  int pin_errs;                 /* No of floating pins */

  int net_names;                /* No of mismatched net names */
};


struct st_chkerrs{

  OBJECT * err_obj;
  CHKERRS * next;

};



struct st_color {
  guint8 r, g, b, a;
  gboolean enabled;
};

/* used by the rc loading mechanisms */
typedef struct {
  int   m_val;
  char *m_str;
} vstbl_entry;

/* Used by g_rc_parse_handler() */
typedef void (*ConfigParseErrorFunc)(GError **, void *);

#endif
