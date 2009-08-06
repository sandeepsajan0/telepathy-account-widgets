/*
 * empathy-account.h - Header for EmpathyAccount
 * Copyright (C) 2009 Collabora Ltd.
 * @author Sjoerd Simons <sjoerd.simons@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __EMPATHY_ACCOUNT_H__
#define __EMPATHY_ACCOUNT_H__

#include <glib-object.h>
#include <gio/gio.h>

#include <telepathy-glib/connection.h>

G_BEGIN_DECLS

typedef struct _EmpathyAccount EmpathyAccount;
typedef struct _EmpathyAccountClass EmpathyAccountClass;

struct _EmpathyAccountClass {
    GObjectClass parent_class;
};

struct _EmpathyAccount {
    GObject parent;
    gpointer priv;
};

GType empathy_account_get_type (void);

/* TYPE MACROS */
#define EMPATHY_TYPE_ACCOUNT (empathy_account_get_type ())
#define EMPATHY_ACCOUNT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), EMPATHY_TYPE_ACCOUNT, EmpathyAccount))
#define EMPATHY_ACCOUNT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), EMPATHY_TYPE_ACCOUNT, EmpathyAccountClass))
#define EMPATHY_IS_ACCOUNT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMPATHY_TYPE_ACCOUNT))
#define EMPATHY_IS_ACCOUNT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), EMPATHY_TYPE_ACCOUNT))
#define EMPATHY_ACCOUNT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), EMPATHY_TYPE_ACCOUNT, \
    EmpathyAccountClass))

gboolean empathy_account_is_just_connected (EmpathyAccount *account);
TpConnection *empathy_account_get_connection (EmpathyAccount *account);
TpConnection *empathy_account_get_connection_for_path (EmpathyAccount *account,
  const gchar *path);
const gchar *empathy_account_get_unique_name (EmpathyAccount *account);
const gchar *empathy_account_get_display_name (EmpathyAccount *account);

const gchar *empathy_account_get_connection_manager (EmpathyAccount *account);
const gchar *empathy_account_get_protocol (EmpathyAccount *account);
const gchar *empathy_account_get_icon_name (EmpathyAccount *account);

void empathy_account_set_enabled_async (EmpathyAccount *account,
    gboolean enabled, GAsyncReadyCallback callback, gpointer user_data);
gboolean empathy_account_set_enabled_finish (EmpathyAccount *account,
    GAsyncResult *result, GError **error);

gboolean empathy_account_is_enabled (EmpathyAccount *account);

gboolean empathy_account_is_valid (EmpathyAccount *account);
gboolean empathy_account_is_ready (EmpathyAccount *account);

void empathy_account_update_settings_async (EmpathyAccount *account,
  GHashTable *parameters, const gchar **unset_parameters,
  GAsyncReadyCallback callback, gpointer user_data);

gboolean empathy_account_update_settings_finish (EmpathyAccount *account,
  GAsyncResult *result, GError **error);

void empathy_account_remove_async (EmpathyAccount *account,
  GAsyncReadyCallback callback, gpointer user_data);
gboolean empathy_account_remove_finish (EmpathyAccount *account,
  GAsyncResult *result, GError **error);

void empathy_account_set_display_name_async (EmpathyAccount *account,
    const gchar *display_name, GAsyncReadyCallback callback,
    gpointer user_data);
gboolean empathy_account_set_display_name_finish (EmpathyAccount *account,
    GAsyncResult *result, GError **error);

EmpathyAccount *empathy_account_new (TpDBusDaemon *bus_daemon,
    const gchar *unique_name);

void empathy_account_request_presence (EmpathyAccount *account,
  TpConnectionPresenceType type, const gchar *status, const gchar *message);

const GHashTable *empathy_account_get_parameters (EmpathyAccount *account);


G_END_DECLS

#endif /* #ifndef __EMPATHY_ACCOUNT_H__*/
