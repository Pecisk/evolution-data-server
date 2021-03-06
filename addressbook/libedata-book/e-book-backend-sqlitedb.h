/*-*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-book-backend-sqlitedb.h
 *
 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
 *
 * Authors: Chenthill Palanisamy <pchenthill@novell.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#if !defined (__LIBEDATA_BOOK_H_INSIDE__) && !defined (LIBEDATA_BOOK_COMPILATION)
#error "Only <libedata-book/libedata-book.h> should be included directly."
#endif

#ifndef E_BOOK_BACKEND_SQLITEDB_H
#define E_BOOK_BACKEND_SQLITEDB_H

#include <libebook-contacts/libebook-contacts.h>

/* Standard GObject macros */
#define E_TYPE_BOOK_BACKEND_SQLITEDB \
	(e_book_backend_sqlitedb_get_type ())
#define E_BOOK_BACKEND_SQLITEDB(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), E_TYPE_BOOK_BACKEND_SQLITEDB, EBookBackendSqliteDB))
#define E_BOOK_BACKEND_SQLITEDB_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), E_TYPE_BOOK_BACKEND_SQLITEDB, EBookBackendSqliteDBClass))
#define E_IS_BOOK_BACKEND_SQLITEDB(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), E_TYPE_BOOK_BACKEND_SQLITEDB))
#define E_IS_BOOK_BACKEND_SQLITEDB_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), E_TYPE_BOOK_BACKEND_SQLITEDB))
#define E_BOOK_BACKEND_SQLITEDB_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), E_TYPE_BOOK_BACKEND_SQLITEDB, EBookBackendSqliteDBClass))

/**
 * E_BOOK_SDB_ERROR:
 *
 * Error domain for #EBookBackendSqliteDB operations.
 *
 * Since: 3.8
 **/
#define E_BOOK_SDB_ERROR (e_book_backend_sqlitedb_error_quark ())

G_BEGIN_DECLS

typedef struct _EBookBackendSqliteDB EBookBackendSqliteDB;
typedef struct _EBookBackendSqliteDBClass EBookBackendSqliteDBClass;
typedef struct _EBookBackendSqliteDBPrivate EBookBackendSqliteDBPrivate;

/**
 * EBookSDBError:
 * @E_BOOK_SDB_ERROR_CONSTRAINT: The error occurred due to an explicit constraint
 * @E_BOOK_SDB_ERROR_CONTACT_NOT_FOUND: A contact was not found by UID (this is different
 *                                      from a query that returns no results, which is not an error).
 * @E_BOOK_SDB_ERROR_OTHER: Another error occurred
 * @E_BOOK_SDB_ERROR_NOT_SUPPORTED: A query was not supported
 * @E_BOOK_SDB_ERROR_INVALID_QUERY: A query was invalid. This can happen if the sexp could not be parsed
 *                                  or if a phone number query contained non-phonenumber input.
 *
 * Defines the types of possible errors reported by the #EBookBackendSqliteDB
 */
typedef enum {
	E_BOOK_SDB_ERROR_CONSTRAINT,
	E_BOOK_SDB_ERROR_CONTACT_NOT_FOUND,
	E_BOOK_SDB_ERROR_OTHER,
	E_BOOK_SDB_ERROR_NOT_SUPPORTED,
	E_BOOK_SDB_ERROR_INVALID_QUERY
} EBookSDBError;

/**
 * EBookBackendSqliteDB:
 *
 * Contains only private data that should be read and manipulated using the
 * functions below.
 *
 * Since: 3.2
 **/
struct _EBookBackendSqliteDB {
	GObject parent;
	EBookBackendSqliteDBPrivate *priv;
};

struct _EBookBackendSqliteDBClass {
	GObjectClass parent_class;
};

/**
 * EbSdbSearchData:
 *
 * FIXME: Document me.
 *
 * Since: 3.2
 **/
typedef struct {
	gchar *vcard;
	gchar *uid;
	gchar *bdata;
} EbSdbSearchData;

GType		e_book_backend_sqlitedb_get_type
						(void) G_GNUC_CONST;
GQuark          e_book_backend_sqlitedb_error_quark
                                                (void);
EBookBackendSqliteDB *
		e_book_backend_sqlitedb_new	(const gchar *path,
						 const gchar *emailid,
						 const gchar *folderid,
						 const gchar *folder_name,
						 gboolean store_vcard,
						 GError **error);
EBookBackendSqliteDB *
		e_book_backend_sqlitedb_new_full
                                                (const gchar *path,
						 const gchar *emailid,
						 const gchar *folderid,
						 const gchar *folder_name,
						 gboolean store_vcard,
						 ESourceBackendSummarySetup *setup,
						 GError **error);
gboolean	e_book_backend_sqlitedb_lock_updates
						(EBookBackendSqliteDB *ebsdb,
						 GError **error);
gboolean	e_book_backend_sqlitedb_unlock_updates
						(EBookBackendSqliteDB *ebsdb,
						 gboolean do_commit,
						 GError **error);
gboolean	e_book_backend_sqlitedb_new_contact
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 EContact *contact,
						 gboolean replace_existing,
						 GError **error);
gboolean	e_book_backend_sqlitedb_new_contacts
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GSList *contacts,
						 gboolean replace_existing,
						 GError **error);
gboolean	e_book_backend_sqlitedb_remove_contact
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_remove_contacts
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GSList *uids,
						 GError **error);
gboolean	e_book_backend_sqlitedb_has_contact
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 gboolean *partial_content,
						 GError **error);
EContact *	e_book_backend_sqlitedb_get_contact
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 GHashTable *fields_of_interest,
						 gboolean *with_all_required_fields,
						 GError **error);
gchar *		e_book_backend_sqlitedb_get_vcard_string
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 GHashTable *fields_of_interest,
						 gboolean *with_all_required_fields,
						 GError **error);
GSList *	e_book_backend_sqlitedb_search	(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *sexp,
						 GHashTable *fields_of_interest,
						 gboolean *searched,
						 gboolean *with_all_required_fields,
						 GError **error);
GSList *	e_book_backend_sqlitedb_search_uids
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *sexp,
						 gboolean *searched,
						 GError **error);
GHashTable *	e_book_backend_sqlitedb_get_uids_and_rev
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_get_is_populated
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_is_populated
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 gboolean populated,
						 GError **error);
gboolean	e_book_backend_sqlitedb_get_revision
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 gchar **revision_out,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_revision
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *revision,
						 GError **error);
gchar *		e_book_backend_sqlitedb_get_sync_data
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_sync_data
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *sync_data,
						 GError **error);
gchar *		e_book_backend_sqlitedb_get_key_value
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *key,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_key_value
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *key,
						 const gchar *value,
						 GError **error);
gchar *		e_book_backend_sqlitedb_get_contact_bdata
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_contact_bdata
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 const gchar *uid,
						 const gchar *value,
						 GError **error);
gboolean	e_book_backend_sqlitedb_get_has_partial_content
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_set_has_partial_content
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 gboolean partial_content,
						 GError **error);
GSList *	e_book_backend_sqlitedb_get_partially_cached_ids
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_delete_addressbook
						(EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GError **error);
gboolean	e_book_backend_sqlitedb_remove	(EBookBackendSqliteDB *ebsdb,
						 GError **error);
void		e_book_backend_sqlitedb_search_data_free
						(EbSdbSearchData *s_data);
gboolean        e_book_backend_sqlitedb_check_summary_query
                                                (EBookBackendSqliteDB *ebsdb,
						 const gchar *query,
						 gboolean *with_list_attrs);
gboolean        e_book_backend_sqlitedb_check_summary_fields
                                                (EBookBackendSqliteDB *ebsdb,
						 GHashTable *fields_of_interest);

#ifndef EDS_DISABLE_DEPRECATED
gboolean	e_book_backend_sqlitedb_is_summary_query
						(const gchar *query);
gboolean	e_book_backend_sqlitedb_is_summary_fields
						(GHashTable *fields_of_interest);
gboolean	e_book_backend_sqlitedb_add_contact
                                                (EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 EContact *contact,
						 gboolean partial_content,
						 GError **error);
gboolean	e_book_backend_sqlitedb_add_contacts
                                                (EBookBackendSqliteDB *ebsdb,
						 const gchar *folderid,
						 GSList *contacts,
						 gboolean partial_content,
						 GError **error);
#endif

G_END_DECLS

#endif /* E_BOOK_BACKEND_SQLITEDB_H */
