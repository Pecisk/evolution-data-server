/* File:       e-gdbus-egdbuscalview.h
 *
 * GType name: EGdbusCalView
 * D-Bus name: org.gnome.evolution.dataserver.calendar.CalView
 *
 * Generated by GDBus Binding Tool 0.1. DO NOT EDIT.
 */

#ifndef __E_GDBUS_E_GDBUS_CAL_VIEW_H__
#define __E_GDBUS_E_GDBUS_CAL_VIEW_H__

#include <gio/gio.h>

#include "e-gdbus-typemappers.h"
G_BEGIN_DECLS

#define E_GDBUS_TYPE_CAL_VIEW         (e_gdbus_cal_view_get_type ())
#define E_GDBUS_CAL_VIEW(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), E_GDBUS_TYPE_CAL_VIEW, EGdbusCalView))
#define E_GDBUS_IS_CAL_VIEW(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), E_GDBUS_TYPE_CAL_VIEW))
#define E_GDBUS_CAL_VIEW_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE((o), E_GDBUS_TYPE_CAL_VIEW, EGdbusCalViewIface))

/**
 * EGdbusCalView:
 *
 * Opaque type representing a proxy or an exported object.
 */
typedef struct _EGdbusCalView EGdbusCalView; /* Dummy typedef */
/**
 * EGdbusCalViewIface:
 * @parent_iface: The parent interface.
 * @objects_added: Handler for the #EGdbusCalView::objects-added signal.
 * @objects_modified: Handler for the #EGdbusCalView::objects-modified signal.
 * @objects_removed: Handler for the #EGdbusCalView::objects-removed signal.
 * @progress: Handler for the #EGdbusCalView::progress signal.
 * @done: Handler for the #EGdbusCalView::done signal.
 * @handle_start: Handler for the #EGdbusCalView::handle-start signal.
 * @handle_stop: Handler for the #EGdbusCalView::handle-stop signal.
 *
 * Virtual table.
 */
typedef struct _EGdbusCalViewIface EGdbusCalViewIface;

GType e_gdbus_cal_view_get_type (void) G_GNUC_CONST;

/* ---------------------------------------------------------------------- */

typedef struct _EGdbusCalViewProxy EGdbusCalViewProxy;
typedef struct _EGdbusCalViewProxyClass EGdbusCalViewProxyClass;

/**
 * EGdbusCalViewProxyPrivate:
 *
 * The #EGdbusCalViewProxyPrivate structure contains only private data.
 */
typedef struct _EGdbusCalViewProxyPrivate EGdbusCalViewProxyPrivate;

/**
 * EGdbusCalViewProxy:
 *
 * The #EGdbusCalViewProxy structure contains only private data and
 * should only be accessed using the provided API.
 */
struct _EGdbusCalViewProxy
{
  GDBusProxy parent_instance;
  EGdbusCalViewProxyPrivate *priv;
};

/**
 * EGdbusCalViewProxyClass:
 *
 * Class structure for #EGdbusCalViewProxy.
 */
struct _EGdbusCalViewProxyClass
{
  GDBusProxyClass parent_class;
};

#define E_GDBUS_TYPE_CAL_VIEW_PROXY (e_gdbus_cal_view_proxy_get_type ())
GType e_gdbus_cal_view_proxy_get_type (void) G_GNUC_CONST;

void e_gdbus_cal_view_proxy_new (GDBusConnection     *connection,
                   GDBusProxyFlags      flags,
                   const gchar         *name,
                   const gchar         *object_path,
                   GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data);
EGdbusCalView *e_gdbus_cal_view_proxy_new_finish (GAsyncResult  *res,
                        GError       **error);
EGdbusCalView *e_gdbus_cal_view_proxy_new_sync (GDBusConnection     *connection,
                       GDBusProxyFlags      flags,
                       const gchar         *name,
                       const gchar         *object_path,
                       GCancellable        *cancellable,
                       GError             **error);

void e_gdbus_cal_view_proxy_new_for_bus (GBusType             bus_type,
                           GDBusProxyFlags      flags,
                           const gchar         *name,
                           const gchar         *object_path,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data);
EGdbusCalView *e_gdbus_cal_view_proxy_new_for_bus_finish (GAsyncResult  *res,
                                 GError       **error);
EGdbusCalView *e_gdbus_cal_view_proxy_new_for_bus_sync (GBusType             bus_type,
                               GDBusProxyFlags      flags,
                               const gchar         *name,
                               const gchar         *object_path,
                               GCancellable        *cancellable,
                               GError             **error);

/* ---------------------------------------------------------------------- */

typedef struct _EGdbusCalViewStub EGdbusCalViewStub;
typedef struct _EGdbusCalViewStubClass EGdbusCalViewStubClass;

/**
 * EGdbusCalViewStubPrivate:
 *
 * The #EGdbusCalViewStubPrivate structure contains only private data.
 */
typedef struct _EGdbusCalViewStubPrivate EGdbusCalViewStubPrivate;

/**
 * EGdbusCalViewStub:
 *
 * The #EGdbusCalViewStub structure contains only private data and
 * should only be accessed using the provided API.
 */
struct _EGdbusCalViewStub
{
  GObject parent_instance;
  EGdbusCalViewStubPrivate *priv;
};

/**
 * EGdbusCalViewStubClass:
 *
 * Class structure for #EGdbusCalViewStub.
 */
struct _EGdbusCalViewStubClass
{
  GObjectClass parent_class;
};

#define E_GDBUS_TYPE_CAL_VIEW_STUB (e_gdbus_cal_view_stub_get_type ())
GType e_gdbus_cal_view_stub_get_type (void) G_GNUC_CONST;

EGdbusCalView *e_gdbus_cal_view_stub_new (void);

guint e_gdbus_cal_view_register_object (EGdbusCalView *object,
                    GDBusConnection *connection,
                    const gchar *object_path,
                    GError **error);

void e_gdbus_cal_view_drain_notify (EGdbusCalView *object);

const GDBusInterfaceInfo *e_gdbus_cal_view_interface_info (void) G_GNUC_CONST;

struct _EGdbusCalViewIface
{
  GTypeInterface parent_iface;

  /* Signal handlers for receiving D-Bus signals: */
  void (*objects_added) (
        EGdbusCalView *object,
        const gchar * const *arg_objects);
  void (*objects_modified) (
        EGdbusCalView *object,
        const gchar * const *arg_objects);
  void (*objects_removed) (
        EGdbusCalView *object,
        const gchar * const *arg_ids);
  void (*progress) (
        EGdbusCalView *object,
        const gchar *arg_message,
        guint arg_progress);
  void (*done) (
        EGdbusCalView *object,
        guint arg_status,
        const gchar *arg_message);

  /* Signal handlers for handling D-Bus method calls: */
  gboolean (*handle_start) (
        EGdbusCalView *object,
        GDBusMethodInvocation *invocation);
  gboolean (*handle_stop) (
        EGdbusCalView *object,
        GDBusMethodInvocation *invocation);
};

/* C Bindings for properties */

/* D-Bus Methods */
void e_gdbus_cal_view_call_start (
        EGdbusCalView *proxy,
        GCancellable *cancellable,
        GAsyncReadyCallback callback,
        gpointer user_data);

gboolean e_gdbus_cal_view_call_start_finish (
        EGdbusCalView *proxy,
        GAsyncResult *res,
        GError **error);

gboolean e_gdbus_cal_view_call_start_sync (
        EGdbusCalView *proxy,
        GCancellable *cancellable,
        GError **error);

void e_gdbus_cal_view_call_stop (
        EGdbusCalView *proxy,
        GCancellable *cancellable,
        GAsyncReadyCallback callback,
        gpointer user_data);

gboolean e_gdbus_cal_view_call_stop_finish (
        EGdbusCalView *proxy,
        GAsyncResult *res,
        GError **error);

gboolean e_gdbus_cal_view_call_stop_sync (
        EGdbusCalView *proxy,
        GCancellable *cancellable,
        GError **error);

/* D-Bus Methods Completion Helpers */
void e_gdbus_cal_view_complete_start (
        EGdbusCalView *object,
        GDBusMethodInvocation *invocation);

void e_gdbus_cal_view_complete_stop (
        EGdbusCalView *object,
        GDBusMethodInvocation *invocation);

/* D-Bus Signal Emission Helpers */
void e_gdbus_cal_view_emit_objects_added (
        EGdbusCalView *object,
        const gchar * const *arg_objects);

void e_gdbus_cal_view_emit_objects_modified (
        EGdbusCalView *object,
        const gchar * const *arg_objects);

void e_gdbus_cal_view_emit_objects_removed (
        EGdbusCalView *object,
        const gchar * const *arg_ids);

void e_gdbus_cal_view_emit_progress (
        EGdbusCalView *object,
        const gchar *arg_message,
        guint arg_progress);

void e_gdbus_cal_view_emit_done (
        EGdbusCalView *object,
        guint arg_status,
        const gchar *arg_message);

G_END_DECLS

#endif /* __E_GDBUS_E_GDBUS_CAL_VIEW_H__ */