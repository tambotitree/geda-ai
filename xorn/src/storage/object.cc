/* Copyright (C) 2013-2020 Roland Lutz

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "internal.h"
#include <stdlib.h>
#include <algorithm>
#include "key_iterator.h"


/** \brief Return whether an object exists in a revision.  */

bool xorn_object_exists_in_revision(xorn_revision_t rev, xorn_object_t ob)
{
	return rev->obstates.find(ob) != rev->obstates.end();
}

/** \brief Get the type of an object in a given revision.
 *
 * Returns \ref xorn_obtype_none if the object doesn't exist in the
 * revision.  */

xorn_obtype_t xorn_get_object_type(xorn_revision_t rev, xorn_object_t ob)
{
	std::map<xorn_object_t, obstate *>::const_iterator i
		= rev->obstates.find(ob);

	if (i == rev->obstates.end())
		return xorn_obtype_none;

	return i->second->type;
}

/** \brief Get a pointer to an object's data in a given revision.
 *
 * The returned data structure (including referenced strings) must not
 * be changed by the caller; use xorn_set_object_data or its type-safe
 * equivalents to change the object.
 *
 * \return Returns a pointer to a data structure matching \a type
 * (e.g., if \a type is \c xornsch_obtype_net, returns a pointer to a
 * \c xornsch_net structure).  If the object doesn't exist in \a rev,
 * or \a type doesn't match its type, returns \c NULL.
 *
 * Example:
 * \snippet functions.c get object data
 *
 * \note Try not to use this function.  There are type-specific
 * functions available (\c xornsch_get_net_data etc.) which offer the
 * same functionality but are type-safe.  */

void const *xorn_get_object_data(xorn_revision_t rev, xorn_object_t ob,
				 xorn_obtype_t type)
{
	std::map<xorn_object_t, obstate *>::const_iterator i
		= rev->obstates.find(ob);

	if (i == rev->obstates.end() || i->second->type != type)
		return NULL;

	return i->second->data;
}

/** \brief Get the location of an object in the object structure.
 *
 * \param rev                Revision to examine
 * \param ob                 Object whose location to return
 * \param attached_to_return Pointer to a variable where to write the
 *                           object to which \a ob is attached
 * \param position_return    Pointer to a variable where to write the
 *                           index of \a ob relative to its sibling
 *                           objects
 *
 * Both pointer arguments may be \c NULL to indicate that the caller
 * isn't interested in the return value.
 *
 * \return Returns \c 0 and writes the appropriate values to \a
 * attached_to_return and \a position_return if \a ob exists in \a
 * rev.  Otherwise, doesn't touch the values and returns \c -1.  */

int xorn_get_object_location(xorn_revision_t rev, xorn_object_t ob,
			     xorn_object_t *attached_to_return,
			     unsigned int *position_return)
{
	std::map<xorn_object_t, xorn_object_t>::const_iterator i
		= rev->parent.find(ob);
	if (i == rev->parent.end())
		return -1;

	if (attached_to_return != NULL)
		*attached_to_return = i->second;

	if (position_return != NULL) {
		std::vector<xorn_object_t> const &children
			= rev->children[i->second];
		*position_return = find(children.begin(),
					children.end(), ob) - children.begin();
	}
	return 0;
}

static void dump_children(xorn_revision_t rev, xorn_object_t attached_to,
			  xorn_object_t **objects_return, size_t *count_return)
{
	std::map<xorn_object_t, std::vector<xorn_object_t> >::const_iterator i
		= rev->children.find(attached_to);

	if (i == rev->children.end())
		return;

	for (std::vector<xorn_object_t>::const_iterator j
		     = i->second.begin(); j != i->second.end(); ++j) {
		(*objects_return)[(*count_return)++] = *j;
		dump_children(rev, *j, objects_return, count_return);
	}
}

/** \brief Return a list of all objects in a revision.
 *
 * A list of \ref xorn_object_t values is allocated and written to,
 * and its location is written to the variable pointed to by \a
 * objects_return.  The number of objects is written to the variable
 * pointed to by \a count_return.  If the list is empty or there is
 * not enough memory, \a *objects_return may be set to \c NULL.
 *
 * The objects are returned in their actual order.  Attached objects
 * are listed after the object they are attached to.
 *
 * \return Returns \c 0 on success and \c -1 if there is not enough
 *         memory.
 *
 * \note You should free the returned list using \c free(3).
 *
 * Example:
 * \snippet functions.c get objects  */

int xorn_get_objects(
	xorn_revision_t rev,
	xorn_object_t **objects_return, size_t *count_return)
{
	*objects_return = (xorn_object_t *) malloc(
		rev->obstates.size() * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !rev->obstates.empty())
		return -1;

	dump_children(rev, NULL, objects_return, count_return);
	return 0;
}

/** \brief Return a list of objects in a revision which are attached
 *         to a certain object.
 *
 * If \a ob is \c NULL, return all objects in the revision which are
 * *not* attached.  The objects are returned in their actual order.
 * Objects attached to the returned objects are not returned.
 *
 * \a objects_return may be \c NULL to indicate that the caller is
 * only interested in the object count.  In this case, the object list
 * isn't created, and the function cannot run out of memory.
 * Otherwise, the same semantics apply as in \ref xorn_get_objects.
 * See there for a more detailed description.  */

int xorn_get_objects_attached_to(
	xorn_revision_t rev, xorn_object_t ob,
	xorn_object_t **objects_return, size_t *count_return)
{
	if (ob != NULL && rev->obstates.find(ob) == rev->obstates.end())
		return -1;
	std::map<xorn_object_t, std::vector<xorn_object_t> >::const_iterator i
		= rev->children.find(ob);
	if (objects_return == NULL) {
		*count_return =
			i == rev->children.end() ? 0 : i->second.size();
		return 0;
	}
	if (i == rev->children.end()) {
		*objects_return = NULL;
		*count_return = 0;
		return 0;
	}

	*objects_return = (xorn_object_t *) malloc(
		i->second.size() * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !i->second.empty())
		return -1;

	for (std::vector<xorn_object_t>::const_iterator j
		     = i->second.begin(); j != i->second.end(); ++j)
		(*objects_return)[(*count_return)++] = *j;
	return 0;
}

/** \brief Return a list of objects which are in a revision as well as
 *         in a selection.
 *
 * The objects are not necessarily returned in a meaningful order.
 *
 * The same semantics apply as in \ref xorn_get_objects.  See there
 * for a more detailed description.  */

int xorn_get_selected_objects(
	xorn_revision_t rev, xorn_selection_t sel,
	xorn_object_t **objects_return, size_t *count_return)
{
	*objects_return = (xorn_object_t *) malloc(
		std::min(rev->obstates.size(),
			 sel->size()) * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !rev->obstates.empty() && !sel->empty())
		return -1;

	xorn_object_t *ptr = set_intersection(
		iterate_keys(rev->obstates.begin()),
		iterate_keys(rev->obstates.end()),
		sel->begin(), sel->end(), *objects_return);

	*count_return = ptr - *objects_return;
	*objects_return = (xorn_object_t *) realloc(
		*objects_return, *count_return * sizeof(xorn_object_t));
	return 0;
}

/** \brief Return a list of objects which are in a revision but not in
 *         another.
 *
 * The returned list contains all objects in \a to_rev which are not
 * in \a from_rev.  They are not necessarily returned in a meaningful order.
 *
 * The same semantics apply as in \ref xorn_get_objects.  See there
 * for a more detailed description.  */

int xorn_get_added_objects(
	xorn_revision_t from_rev, xorn_revision_t to_rev,
	xorn_object_t **objects_return, size_t *count_return)
{
	*objects_return = (xorn_object_t *) malloc(
		to_rev->obstates.size() * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !to_rev->obstates.empty())
		return -1;

	xorn_object_t *ptr = set_difference(
		iterate_keys(to_rev->obstates.begin()),
		iterate_keys(to_rev->obstates.end()),
		iterate_keys(from_rev->obstates.begin()),
		iterate_keys(from_rev->obstates.end()), *objects_return);

	*count_return = ptr - *objects_return;
	*objects_return = (xorn_object_t *) realloc(
		*objects_return, *count_return * sizeof(xorn_object_t));
	return 0;
}

/** \brief Return a list of objects which are in a revision but not in
 *         another.
 *
 * The returned list contains all objects in \a from_rev which are not
 * in \a to_rev.  They are not necessarily returned in a meaningful order.
 *
 * The same semantics apply as in \ref xorn_get_objects.  See there
 * for a more detailed description.  */

int xorn_get_removed_objects(
	xorn_revision_t from_rev, xorn_revision_t to_rev,
	xorn_object_t **objects_return, size_t *count_return)
{
	*objects_return = (xorn_object_t *) malloc(
		from_rev->obstates.size() * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !from_rev->obstates.empty())
		return -1;

	xorn_object_t *ptr = set_difference(
		iterate_keys(from_rev->obstates.begin()),
		iterate_keys(from_rev->obstates.end()),
		iterate_keys(to_rev->obstates.begin()),
		iterate_keys(to_rev->obstates.end()), *objects_return);

	*count_return = ptr - *objects_return;
	*objects_return = (xorn_object_t *) realloc(
		*objects_return, *count_return * sizeof(xorn_object_t));
	return 0;
}

/** \brief Return a list of objects which exist in two revisions but
 *         have different type or data.
 *
 * The objects are not necessarily returned in a meaningful order.
 *
 * The same semantics apply as in \ref xorn_get_objects.  See there
 * for a more detailed description.  */

int xorn_get_modified_objects(
	xorn_revision_t from_rev, xorn_revision_t to_rev,
	xorn_object_t **objects_return, size_t *count_return)
{
	*objects_return = (xorn_object_t *) malloc(
		std::min(from_rev->obstates.size(),
			 to_rev->obstates.size()) * sizeof(xorn_object_t));
	*count_return = 0;
	if (*objects_return == NULL && !from_rev->obstates.empty()
				    && !to_rev->obstates.empty())
		return -1;

	std::map<xorn_object_t, obstate *>::const_iterator i
		= from_rev->obstates.begin();
	std::map<xorn_object_t, obstate *>::const_iterator j
		= to_rev->obstates.begin();

	while (i != from_rev->obstates.end() && j != to_rev->obstates.end())
		if (i->first < j->first)
			++i;
		else if (i->first > j->first)
			++j;
		else {
			if (i->second != j->second)
				(*objects_return)[(*count_return)++] =
					i->first;
			++i;
			++j;
		}

	*objects_return = (xorn_object_t *) realloc(
		*objects_return, *count_return * sizeof(xorn_object_t));
	return 0;
}
