/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
 *
 * Authors: Michael Zucchi <notzed@ximian.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <camel/camel-file-utils.h>
#include "camel-imapx-utils.h"
#include "camel-imapx-store-summary.h"

#define d(...) camel_imapx_debug(debug, '?', __VA_ARGS__)

/* Version 0: Original IMAPX file format. */
#define CAMEL_IMAPX_STORE_SUMMARY_VERSION_0 (0)

/* Version 1: (3.10) Store the hierarchy separator. */
#define CAMEL_IMAPX_STORE_SUMMARY_VERSION_1 (1);

#define CAMEL_IMAPX_STORE_SUMMARY_VERSION (1)

G_DEFINE_TYPE (
	CamelIMAPXStoreSummary,
	camel_imapx_store_summary,
	CAMEL_TYPE_STORE_SUMMARY)


static CamelIMAPXNamespaceList *
namespace_load (CamelStoreSummary *s,
                FILE *in)
{
	CamelIMAPXStoreNamespace *ns, *tail;
	CamelIMAPXNamespaceList *nsl;
	guint32 i, j;
	gint32 n;

	nsl = g_malloc0 (sizeof (CamelIMAPXNamespaceList));
	nsl->personal = NULL;
	nsl->shared = NULL;
	nsl->other = NULL;

	for (j = 0; j < 3; j++) {
		switch (j) {
		case 0:
			tail = (CamelIMAPXStoreNamespace *) &nsl->personal;
			break;
		case 1:
			tail = (CamelIMAPXStoreNamespace *) &nsl->shared;
			break;
		case 2:
			tail = (CamelIMAPXStoreNamespace *) &nsl->other;
			break;
		}

		if (camel_file_util_decode_fixed_int32 (in, &n) == -1)
			goto fail;

		for (i = 0; i < n; i++) {
			guint32 sep;
			gchar *prefix;
			gchar *unused;

			if (camel_file_util_decode_string (in, &prefix) == -1)
				goto fail;

			/* XXX This string is just a duplicate of 'prefix',
			 *     retained only for backward-compatibility. */
			if (camel_file_util_decode_string (in, &unused) == -1) {
				g_free (prefix);
				goto fail;
			}

			g_free (unused);

			if (camel_file_util_decode_uint32 (in, &sep) == -1) {
				g_free (prefix);
				goto fail;
			}

			tail->next = ns = g_malloc (sizeof (CamelIMAPXStoreNamespace));
			ns->sep = sep;
			ns->prefix = prefix;
			ns->next = NULL;
			tail = ns;
		}
	}

	return nsl;

fail:
	camel_imapx_namespace_list_clear (nsl);

	return NULL;
}

static gint
namespace_save (CamelStoreSummary *s,
                FILE *out,
                CamelIMAPXNamespaceList *nsl)
{
	CamelIMAPXStoreNamespace *ns, *cur = NULL;
	guint32 i, n;

	for (i = 0; i < 3; i++) {
		switch (i) {
		case 0:
			cur = nsl->personal;
			break;
		case 1:
			cur = nsl->shared;
			break;
		case 2:
			cur = nsl->other;
			break;
		}

		for (ns = cur, n = 0; ns; n++)
			ns = ns->next;

		if (camel_file_util_encode_fixed_int32 (out, n) == -1)
			return -1;

		ns = cur;
		while (ns != NULL) {
			if (camel_file_util_encode_string (out, ns->prefix) == -1)
				return -1;

			/* XXX This redundancy is for backward-compatibility. */
			if (camel_file_util_encode_string (out, ns->prefix) == -1)
				return -1;

			if (camel_file_util_encode_uint32 (out, ns->sep) == -1)
				return -1;

			ns = ns->next;
		}
	}

	return 0;
}

static void
imapx_store_summary_finalize (GObject *object)
{
	CamelIMAPXStoreSummary *summary;

	summary = CAMEL_IMAPX_STORE_SUMMARY (object);

	camel_imapx_namespace_list_clear (summary->namespaces);

	/* Chain up to parent's finalize() method. */
	G_OBJECT_CLASS (camel_imapx_store_summary_parent_class)->
		finalize (object);
}

static gint
imapx_store_summary_summary_header_load (CamelStoreSummary *s,
                                         FILE *in)
{
	CamelIMAPXStoreSummary *is = (CamelIMAPXStoreSummary *) s;
	CamelStoreSummaryClass *store_summary_class;
	gint32 version, unused;

	camel_imapx_namespace_list_clear (is->namespaces);

	store_summary_class =
		CAMEL_STORE_SUMMARY_CLASS (
		camel_imapx_store_summary_parent_class);

	/* Chain up to parent's summary_header_load() method. */
	if (store_summary_class->summary_header_load (s, in) == -1)
		return -1;

	if (camel_file_util_decode_fixed_int32 (in, &version) == -1)
		return -1;

	is->version = version;

	if (version < CAMEL_IMAPX_STORE_SUMMARY_VERSION) {
		g_warning (
			"Unable to load store summary: "
			"Expected version (%d), got (%d)",
			CAMEL_IMAPX_STORE_SUMMARY_VERSION,
			version);
		return -1;
	}

	/* note file format can be expanded to contain more namespaces, but only 1 at the moment */
	if (camel_file_util_decode_fixed_int32 (in, &unused) == -1)
		return -1;

	/* namespaces */
	if ((is->namespaces = namespace_load (s, in)) == NULL)
		return -1;

	return 0;
}

static gint
imapx_store_summary_summary_header_save (CamelStoreSummary *s,
                                         FILE *out)
{
	CamelIMAPXStoreSummary *is = (CamelIMAPXStoreSummary *) s;
	CamelStoreSummaryClass *store_summary_class;

	store_summary_class =
		CAMEL_STORE_SUMMARY_CLASS (
		camel_imapx_store_summary_parent_class);

	/* Chain up to parent's summary_header_save() method. */
	if (store_summary_class->summary_header_save (s, out) == -1)
		return -1;

	/* always write as latest version */
	if (camel_file_util_encode_fixed_int32 (out, CAMEL_IMAPX_STORE_SUMMARY_VERSION) == -1)
		return -1;

	if (camel_file_util_encode_fixed_int32 (out, 0) == -1)
		return -1;

	if (is->namespaces && namespace_save (s, out, is->namespaces) == -1)
		return -1;

	return 0;
}

static CamelStoreInfo *
imapx_store_summary_store_info_load (CamelStoreSummary *s,
                                     FILE *in)
{
	CamelStoreSummaryClass *store_summary_class;
	CamelStoreInfo *si;
	gchar *mailbox_name = NULL;
	gchar *separator = NULL;

	store_summary_class =
		CAMEL_STORE_SUMMARY_CLASS (
		camel_imapx_store_summary_parent_class);

	/* Chain up to parent's store_info_load() method. */
	si = store_summary_class->store_info_load (s, in);
	if (si == NULL)
		return NULL;

	if (camel_file_util_decode_string (in, &separator) == -1) {
		camel_store_summary_info_unref (s, si);
		return NULL;
	}

	if (camel_file_util_decode_string (in, &mailbox_name) == -1) {
		camel_store_summary_info_unref (s, si);
		return NULL;
	}

	/* NB: this is done again for compatability */
	if (camel_imapx_mailbox_is_inbox (mailbox_name))
		si->flags |=
			CAMEL_FOLDER_SYSTEM |
			CAMEL_FOLDER_TYPE_INBOX;

	((CamelIMAPXStoreInfo *) si)->mailbox_name = mailbox_name;
	((CamelIMAPXStoreInfo *) si)->separator = *separator;

	g_free (separator);

	return si;
}

static gint
imapx_store_summary_store_info_save (CamelStoreSummary *s,
                                     FILE *out,
                                     CamelStoreInfo *si)
{
	CamelStoreSummaryClass *store_summary_class;
	gchar separator[] = { '\0', '\0' };
	const gchar *mailbox_name;

	store_summary_class =
		CAMEL_STORE_SUMMARY_CLASS (
		camel_imapx_store_summary_parent_class);

	mailbox_name = ((CamelIMAPXStoreInfo *) si)->mailbox_name;
	separator[0] = ((CamelIMAPXStoreInfo *) si)->separator;

	/* Chain up to parent's store_info_save() method. */
	if (store_summary_class->store_info_save (s, out, si) == -1)
		return -1;

	if (camel_file_util_encode_string (out, separator) == -1)
		return -1;

	if (camel_file_util_encode_string (out, mailbox_name) == -1)
		return -1;

	return 0;
}

static void
imapx_store_summary_store_info_free (CamelStoreSummary *s,
                                     CamelStoreInfo *si)
{
	CamelStoreSummaryClass *store_summary_class;

	store_summary_class =
		CAMEL_STORE_SUMMARY_CLASS (
		camel_imapx_store_summary_parent_class);

	g_free (((CamelIMAPXStoreInfo *) si)->mailbox_name);

	/* Chain up to parent's store_info_free() method. */
	store_summary_class->store_info_free (s, si);
}

static void
camel_imapx_store_summary_class_init (CamelIMAPXStoreSummaryClass *class)
{
	GObjectClass *object_class;
	CamelStoreSummaryClass *store_summary_class;

	object_class = G_OBJECT_CLASS (class);
	object_class->finalize = imapx_store_summary_finalize;

	store_summary_class = CAMEL_STORE_SUMMARY_CLASS (class);
	store_summary_class->summary_header_load =imapx_store_summary_summary_header_load;
	store_summary_class->summary_header_save = imapx_store_summary_summary_header_save;
	store_summary_class->store_info_load = imapx_store_summary_store_info_load;
	store_summary_class->store_info_save = imapx_store_summary_store_info_save;
	store_summary_class->store_info_free = imapx_store_summary_store_info_free;
}

static void
camel_imapx_store_summary_init (CamelIMAPXStoreSummary *s)
{
	((CamelStoreSummary *) s)->store_info_size = sizeof (CamelIMAPXStoreInfo);
	s->version = CAMEL_IMAPX_STORE_SUMMARY_VERSION;
}

/**
 * camel_imapx_store_summary_new:
 *
 * Create a new CamelIMAPXStoreSummary object.
 *
 * Returns: A new CamelIMAPXStoreSummary widget.
 **/
CamelIMAPXStoreSummary *
camel_imapx_store_summary_new (void)
{
	return g_object_new (CAMEL_TYPE_IMAPX_STORE_SUMMARY, NULL);
}

/**
 * camel_imapx_store_summary_mailbox:
 * @s:
 * @mailbox_name:
 *
 * Retrieve a summary item by mailbox name.
 *
 * A referenced to the summary item is returned, which may be
 * ref'd or free'd as appropriate.
 *
 * Returns: The summary item, or NULL if the @mailbox_name
 * is not available.
 * It must be freed using camel_store_summary_info_unref().
 **/
CamelIMAPXStoreInfo *
camel_imapx_store_summary_mailbox (CamelIMAPXStoreSummary *s,
                                   const gchar *mailbox_name)
{
	CamelStoreInfo *match = NULL;
	GPtrArray *array;
	gboolean find_inbox;
	guint ii;

	find_inbox = camel_imapx_mailbox_is_inbox (mailbox_name);

	array = camel_store_summary_array (CAMEL_STORE_SUMMARY (s));

	for (ii = 0; ii < array->len; ii++) {
		CamelIMAPXStoreInfo *info;
		gboolean is_inbox;

		info = g_ptr_array_index (array, ii);
		is_inbox = camel_imapx_mailbox_is_inbox (info->mailbox_name);

		if (find_inbox && is_inbox) {
			match = camel_store_summary_info_ref (
				CAMEL_STORE_SUMMARY (s),
				(CamelStoreInfo *) info);
			break;
		}

		if (g_str_equal (info->mailbox_name, mailbox_name)) {
			match = camel_store_summary_info_ref (
				CAMEL_STORE_SUMMARY (s),
				(CamelStoreInfo *) info);
			break;
		}
	}

	camel_store_summary_array_free (CAMEL_STORE_SUMMARY (s), array);

	return (CamelIMAPXStoreInfo *) match;
}

gchar *
camel_imapx_store_summary_path_to_mailbox (CamelIMAPXStoreSummary *s,
                                           const gchar *path,
                                           gchar dir_sep)
{
	CamelStoreSummary *store_summary;
	gchar *mailbox_name;
	gchar *subpath, *last = NULL;
	CamelStoreInfo *si;
	CamelIMAPXStoreNamespace *ns;
	const gchar *si_mailbox_name = NULL;

	store_summary = CAMEL_STORE_SUMMARY (s);

	/* check to see if we have a subpath of path already defined */
	subpath = alloca (strlen (path) + 1);
	strcpy (subpath, path);
	do {
		si = camel_store_summary_path (store_summary, subpath);
		if (si == NULL) {
			last = strrchr (subpath, '/');
			if (last != NULL)
				*last = 0;
		}
	} while (si == NULL && last != NULL);

	if (si != NULL)
		si_mailbox_name = ((CamelIMAPXStoreInfo *) si)->mailbox_name;

	/* path is already present, use the raw version we have */
	if (si != NULL && strlen (subpath) == strlen (path)) {
		mailbox_name = g_strdup (si_mailbox_name);
		camel_store_summary_info_unref (store_summary, si);
		return mailbox_name;
	}

	ns = camel_imapx_store_summary_namespace_find_by_path (s, path);

	if (si != NULL)
		mailbox_name = g_strdup (path + strlen (subpath));
	else if (ns != NULL)
		mailbox_name = g_strdup (path + strlen (ns->prefix));
	else
		mailbox_name = g_strdup (path);

	if (dir_sep != '/') {
		gchar *cp = mailbox_name;

		while (*cp != '\0') {
			if (*cp == '/')
				*cp = dir_sep;
			else if (*cp == dir_sep)
				*cp = '/';
			cp++;
		}
	}

	/* merge old path part if required */
	if (si != NULL) {
		gchar *temp;

		temp = g_strdup_printf ("%s%s", si_mailbox_name, mailbox_name);
		g_free (mailbox_name);
		camel_store_summary_info_unref (store_summary, si);
		mailbox_name = temp;

	} else if (ns != NULL) {
		gchar *temp;

		temp = g_strdup_printf ("%s%s", ns->prefix, mailbox_name);
		g_free (mailbox_name);
		mailbox_name = temp;
	}

	return mailbox_name;
}

CamelIMAPXStoreInfo *
camel_imapx_store_summary_add_from_mailbox (CamelIMAPXStoreSummary *s,
                                            const gchar *mailbox,
                                            gchar dir_sep)
{
	CamelIMAPXStoreInfo *info;
	gchar *pathu8, *prefix;
	gint len;
	gchar *mailbox_copy;
	CamelIMAPXStoreNamespace *ns;

	d ("adding mailbox '%s' '%c'\n", mailbox, dir_sep);

	len = strlen (mailbox);
	mailbox_copy = alloca (len + 1);
	strcpy (mailbox_copy, mailbox);
	if (mailbox_copy[len - 1] == dir_sep)
		mailbox_copy[len - 1] = 0;

	info = camel_imapx_store_summary_mailbox (s, mailbox_copy);
	if (info) {
		camel_store_summary_info_unref ((CamelStoreSummary *) s, (CamelStoreInfo *) info);
		d ("  already there\n");
		return info;
	}

	ns = camel_imapx_store_summary_namespace_find_by_mailbox (s, mailbox_copy);
	if (ns) {
		d ("(found namespace for '%s' ns '%s') ", mailbox_copy, ns->prefix);
		dir_sep = ns->sep;
		len = strlen (ns->prefix);
		if (len >= strlen (mailbox_copy)) {
			pathu8 = g_strdup (ns->prefix);
		} else {
			if (mailbox_copy[len] == ns->sep)
				len++;

			prefix = camel_imapx_mailbox_to_folder_path (
				mailbox_copy + len, ns->sep);
			if (*ns->prefix) {
				pathu8 = g_strdup_printf ("%s/%s", ns->prefix, prefix);
				g_free (prefix);
			} else {
				pathu8 = prefix;
			}
		}
		d (" (pathu8 = '%s')", pathu8);
	} else {
		d ("(Cannot find namespace for '%s')\n", mailbox_copy);
		pathu8 = camel_imapx_mailbox_to_folder_path (
			mailbox_copy, dir_sep);
	}

	info = (CamelIMAPXStoreInfo *) camel_store_summary_add_from_path ((CamelStoreSummary *) s, pathu8);
	if (info) {
		d ("  '%s' -> '%s'\n", pathu8, mailbox_copy);
		info->mailbox_name = g_strdup (mailbox_copy);
		info->separator = dir_sep;

		if (camel_imapx_mailbox_is_inbox (mailbox_copy))
			info->info.flags |=
				CAMEL_FOLDER_SYSTEM |
				CAMEL_FOLDER_TYPE_INBOX;
	} else {
		d ("  failed\n");
	}

	g_free (pathu8);

	return info;
}

/* should this be const? */
/* TODO: deprecate/merge this function with path_to_mailbox */
gchar *
camel_imapx_store_summary_mailbox_from_path (CamelIMAPXStoreSummary *s,
                                             const gchar *path)
{
	CamelIMAPXStoreNamespace *ns;
	gchar *mailbox = NULL;

	ns = camel_imapx_store_summary_namespace_find_by_path (s, path);
	if (ns)
		mailbox = camel_imapx_store_summary_path_to_mailbox (
			s, path, ns->sep);

	d ("looking up path %s -> %s\n", path, mailbox ? mailbox:"not found");

	return mailbox;
}

CamelIMAPXStoreNamespace *
camel_imapx_store_summary_namespace_find_by_path (CamelIMAPXStoreSummary *s,
                                                  const gchar *path)
{
	gint len;
	CamelIMAPXStoreNamespace *ns;

	/* NB: this currently only compares against 1 namespace, in future compare against others */
	/* CHEN TODO */
	ns = s->namespaces->personal;
	while (ns) {
		len = strlen (ns->prefix);
		if (len == 0
		    || (strncmp (ns->prefix, path, len) == 0
			&& (path[len] == '/' || path[len] == 0)))
			break;
		ns = NULL;
	}

	/* have a default? */
	return ns;
}

CamelIMAPXStoreNamespace *
camel_imapx_store_summary_namespace_find_by_mailbox (CamelIMAPXStoreSummary *s,
                                                     const gchar *mailbox)
{
	gint len = 0;
	CamelIMAPXStoreNamespace *ns;

	/* NB: this currently only compares against 1 namespace, in future compare against others */
	/* CHEN TODO */
	ns = s->namespaces->personal;
	while (ns) {
		if (ns->prefix)
			len = strlen (ns->prefix);
		if (len == 0
		    || (strncmp (ns->prefix, mailbox, len) == 0
			&& (mailbox[len] == ns->sep || mailbox[len] == 0)))
			break;
		ns = NULL;
	}

	/* have a default? */
	return ns;
}
