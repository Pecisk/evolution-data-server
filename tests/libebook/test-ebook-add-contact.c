/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

#include <stdlib.h>

#include "ebook-test-utils.h"
#include "e-test-server-utils.h"

static ETestServerClosure book_closure =
	{ E_TEST_SERVER_DEPRECATED_ADDRESS_BOOK, NULL, 0 };

static void
test_add_contact_sync (ETestServerFixture *fixture,
                       gconstpointer user_data)
{
	EBook *book;
	EContact *contact;
	gchar *uid;

	book = E_TEST_SERVER_UTILS_SERVICE (fixture, EBook);
	uid = ebook_test_utils_book_add_contact_from_test_case_verify (
		book, "simple-1", &contact);

	test_print ("successfully added and retrieved contact '%s'\n", uid);
	g_object_unref (contact);
}

static void
test_add_contact_async (ETestServerFixture *fixture,
                        gconstpointer user_data)
{
	EBook *book;
	gchar *vcard;
	EContact *contact;

	book = E_TEST_SERVER_UTILS_SERVICE (fixture, EBook);

	vcard = ebook_test_utils_new_vcard_from_test_case ("simple-1");
	contact = e_contact_new_from_vcard (vcard);
	g_free (vcard);

	ebook_test_utils_book_async_add_contact (
		book, contact, ebook_test_utils_callback_quit, fixture->loop);

	g_main_loop_run (fixture->loop);
}

gint
main (gint argc,
      gchar **argv)
{
#if !GLIB_CHECK_VERSION (2, 35, 1)
	g_type_init ();
#endif
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base ("http://bugzilla.gnome.org/");

	g_test_add (
		"/EBook/AddContact/Sync",
		ETestServerFixture,
		&book_closure,
		e_test_server_utils_setup,
		test_add_contact_sync,
		e_test_server_utils_teardown);
	g_test_add (
		"/EBook/AddContact/Async",
		ETestServerFixture,
		&book_closure,
		e_test_server_utils_setup,
		test_add_contact_async,
		e_test_server_utils_teardown);

	return e_test_server_utils_run ();
}
