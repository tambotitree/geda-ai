/* gEDA - GPL Electronic Design Automation
 * gnetlist - gEDA Netlist
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

#include <config.h>

#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"

/* used by the extract functions below */
#define DELIMITERS ",; "

/* hack rename this to be s_return_tail */
/* update object_tail or any list of that matter */
NETLIST *s_netlist_return_tail(NETLIST * head)
{
    NETLIST *nl_current = NULL;
    NETLIST *ret_struct = NULL;

    nl_current = head;
    while (nl_current != NULL) {	/* goto end of list */
	ret_struct = nl_current;
	nl_current = nl_current->next;
    }

    return (ret_struct);
}

/* hack rename this to be s_return_head */
/* update object_tail or any list of that matter */
NETLIST *s_netlist_return_head(NETLIST * tail)
{
    NETLIST *nl_current = NULL;
    NETLIST *ret_struct = NULL;

    nl_current = tail;
    while (nl_current != NULL) {	/* goto end of list */
	ret_struct = nl_current;
	nl_current = nl_current->prev;
    }

    return (ret_struct);
}


/* returns new node */
NETLIST *s_netlist_add(NETLIST * ptr)
{
    NETLIST *new_node;

    new_node = (NETLIST *) g_malloc(sizeof(NETLIST));

    /* setup node information */
    new_node->nlid = 0;
    new_node->cpins = NULL;
    new_node->component_uref = NULL;
    new_node->object_ptr = NULL;
    new_node->hierarchy_tag = NULL;
    new_node->composite_component = FALSE;

    /* Setup link list stuff */
    new_node->next = NULL;

    if (ptr == NULL) {
	new_node->prev = NULL;	/* setup previous link */
	return (new_node);
    } else {
	new_node->prev = ptr;	/* setup previous link */
	ptr->next = new_node;
	return (ptr->next);
    }
}

void s_netlist_print(NETLIST * ptr)
{
    NETLIST *nl_current = NULL;

    nl_current = ptr;

    if (nl_current == NULL) {
	return;
    }

    while (nl_current != NULL) {

	if (nl_current->nlid != -1) {

	    if (nl_current->component_uref) {
		printf("component %s \n", nl_current->component_uref);
	    } else {
		printf("component SPECIAL \n");
	    }

	    if (nl_current->hierarchy_tag) {
		printf("Hierarchy tag: %s\n", nl_current->hierarchy_tag);
	    }

	    if (nl_current->cpins) {
		s_cpinlist_print(nl_current->cpins);
	    }

	    printf("\n");
	}

	nl_current = nl_current->next;
    }
    printf("\n");
}

void s_netlist_post_process(TOPLEVEL * pr_current, NETLIST * head)
{
    NETLIST *nl_current;
    CPINLIST *pl_current;

    if (verbose_mode) {
	printf("\n- Staring post processing\n");
	printf("- Naming nets:\n");
    }

    /* this pass gives all nets a name, whether specified or creates a */
    /* name */
    nl_current = head;
    while (nl_current != NULL) {
	if (nl_current->cpins) {
	    pl_current = nl_current->cpins;
	    while (pl_current != NULL) {

		if (pl_current->plid != -1) {
		    verbose_print("p");
		}

		if (pl_current->plid != -1 && pl_current->nets) {

		    g_free(pl_current->net_name);

		    verbose_print("n");

		    /* only name nets of components which */
		    /* have a uref */
		    if (nl_current->component_uref) {
			pl_current->net_name =
			    s_net_name(pr_current,
				       head,
				       pl_current->nets,
				       nl_current->hierarchy_tag,
				       pl_current->type);

			/* put this name also in the first
			   node of the nets linked list */
			if (pl_current->net_name && pl_current->nets) {
			    if (pl_current->nets->next) {
				pl_current->nets->next->net_name =
				    g_strdup (pl_current->net_name);
			    }
			}
		    }
		}

		pl_current = pl_current->next;
	    }
	}
	nl_current = nl_current->next;
    }

    verbose_done();
    if (verbose_mode) {
	printf("- Renaming nets:\n");
    }

    s_rename_all(pr_current, head);

    verbose_done();
    if (verbose_mode) {
	printf("- Resolving hierarchy:\n");
    }
    s_hierarchy_post_process(pr_current, head);

    verbose_done();
    if (pr_current->hierarchy_uref_mangle == FALSE) {
	if (verbose_mode) {
	    printf("- Removing refdes mangling:\n");
	}
	s_hierarchy_remove_uref_mangling(pr_current, head);
    }

    verbose_done();
}

void s_netlist_name_named_nets (TOPLEVEL *pr_current,
				NETLIST *named_netlist,
				NETLIST *unnamed_netlist) {

  NETLIST *nl_current;
  CPINLIST *pl_current;
  NET *n_current;
  char *net_name;

  if (verbose_mode) {
    printf("\n- Staring post processing\n");
    printf("- Naming nets of graphical objects:\n");
  }

  /* this pass gives all nets a name, whether specified or creates a */
  /* name */
  nl_current = unnamed_netlist;
  while (nl_current != NULL) {
    if (nl_current->cpins) {
      pl_current = nl_current->cpins;
      while (pl_current != NULL) {

	if (pl_current->plid != -1) {
	  verbose_print("p");
	}

	if (pl_current->plid != -1 && pl_current->nets) {
	  verbose_print("n");
	  net_name = NULL;
	  n_current = pl_current->nets;
	  while (n_current != NULL) {
	    g_free (n_current->net_name);
	    n_current->net_name = s_netlist_netname_of_netid(pr_current,
							     named_netlist,
							     n_current->nid);

	    if (n_current->net_name != NULL) {
	      net_name = n_current->net_name;
	    }
	    n_current = n_current->next;
	  }
	  if (net_name != NULL) {
	    pl_current->net_name = g_strdup(net_name);
	  }
	}
	pl_current = pl_current->next;
      }
    }
    nl_current = nl_current->next;
  }

  verbose_done();

}

char *s_netlist_netname_of_netid (TOPLEVEL *pr_current,
				  NETLIST *netlist_head,
				  int net_id) {

  NETLIST *nl_current;
  CPINLIST *pl_current;
  NET *n_current;

  nl_current = netlist_head;

  /* walk through the list of components, and through the list
   * of individual pins on each, looking for the net identifier
   */
  while (nl_current != NULL) {
    pl_current = nl_current->cpins;
    while (pl_current != NULL) {
      if (pl_current->net_name) {
	n_current = pl_current->nets;
	while (n_current != NULL) {
	  if (n_current->nid == net_id) {
	    return (g_strdup(n_current->net_name));
	  }
	  n_current = n_current->next;
	}
      }
      pl_current = pl_current->next;
    }
    nl_current = nl_current->next;
    }
  return NULL;
}
