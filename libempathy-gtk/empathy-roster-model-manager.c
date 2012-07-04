/*
 * empathy-roster-model-manager.c
 *
 * Implementation of EmpathyRosterModel using EmpathyIndividualManager as
 * source.
 *
 * Copyright (C) 2012 Collabora Ltd. <http://www.collabora.co.uk/>
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

#include "config.h"

#include "empathy-roster-model-manager.h"

#include "empathy-roster-model.h"

static void roster_model_iface_init (EmpathyRosterModelInterface *iface);

G_DEFINE_TYPE_WITH_CODE (EmpathyRosterModelManager,
    empathy_roster_model_manager,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (EMPATHY_TYPE_ROSTER_MODEL, roster_model_iface_init))

enum
{
  PROP_MANAGER = 1,
  N_PROPS
};

/*
enum
{
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];
*/

struct _EmpathyRosterModelManagerPriv
{
  EmpathyIndividualManager *manager;
};

static void
members_changed_cb (EmpathyIndividualManager *manager,
    const gchar *message,
    GList *added,
    GList *removed,
    TpChannelGroupChangeReason reason,
    EmpathyRosterModelManager *self)
{
  GList *l;

  for (l = added; l != NULL; l = g_list_next (l))
    {
      empathy_roster_model_fire_individual_added (EMPATHY_ROSTER_MODEL (self),
          l->data);
    }

  for (l = removed; l != NULL; l = g_list_next (l))
    {
      empathy_roster_model_fire_individual_removed (EMPATHY_ROSTER_MODEL (self),
          l->data);
    }
}

static void
groups_changed_cb (EmpathyIndividualManager *manager,
    FolksIndividual *individual,
    const gchar *group,
    gboolean is_member,
    EmpathyRosterModelManager *self)
{
  empathy_roster_model_fire_groups_changed (EMPATHY_ROSTER_MODEL (self),
      individual,
      group,
      is_member);
}

static void
empathy_roster_model_manager_get_property (GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
  EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (object);

  switch (property_id)
    {
      case PROP_MANAGER:
        g_value_set_object (value, self->priv->manager);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
empathy_roster_model_manager_set_property (GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec)
{
  EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (object);

  switch (property_id)
    {
      case PROP_MANAGER:
        g_assert (self->priv->manager == NULL); /* construct only */
        self->priv->manager = g_value_dup_object (value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
empathy_roster_model_manager_constructed (GObject *object)
{
  EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_roster_model_manager_parent_class)->constructed;

  if (chain_up != NULL)
    chain_up (object);

  g_assert (EMPATHY_IS_INDIVIDUAL_MANAGER (self->priv->manager));

  tp_g_signal_connect_object (self->priv->manager, "members-changed",
      G_CALLBACK (members_changed_cb), self, 0);
  tp_g_signal_connect_object (self->priv->manager, "groups-changed",
      G_CALLBACK (groups_changed_cb), self, 0);
}

static void
empathy_roster_model_manager_dispose (GObject *object)
{
  EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_roster_model_manager_parent_class)->dispose;

  g_clear_object (&self->priv->manager);

  if (chain_up != NULL)
    chain_up (object);
}

static void
empathy_roster_model_manager_finalize (GObject *object)
{
  //EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (object);
  void (*chain_up) (GObject *) =
      ((GObjectClass *) empathy_roster_model_manager_parent_class)->finalize;

  if (chain_up != NULL)
    chain_up (object);
}

static void
empathy_roster_model_manager_class_init (
    EmpathyRosterModelManagerClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);
  GParamSpec *spec;

  oclass->get_property = empathy_roster_model_manager_get_property;
  oclass->set_property = empathy_roster_model_manager_set_property;
  oclass->constructed = empathy_roster_model_manager_constructed;
  oclass->dispose = empathy_roster_model_manager_dispose;
  oclass->finalize = empathy_roster_model_manager_finalize;

  spec = g_param_spec_object ("manager", "Manager",
      "EmpathyIndividualManager",
      EMPATHY_TYPE_INDIVIDUAL_MANAGER,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (oclass, PROP_MANAGER, spec);

  g_type_class_add_private (klass, sizeof (EmpathyRosterModelManagerPriv));
}

static void
empathy_roster_model_manager_init (EmpathyRosterModelManager *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self,
      EMPATHY_TYPE_ROSTER_MODEL_MANAGER, EmpathyRosterModelManagerPriv);
}

EmpathyRosterModelManager *
empathy_roster_model_manager_new (EmpathyIndividualManager *manager)
{
  g_return_val_if_fail (EMPATHY_IS_INDIVIDUAL_MANAGER (manager), NULL);

  return g_object_new (EMPATHY_TYPE_ROSTER_MODEL_MANAGER,
      "manager", manager,
      NULL);
}

static GList *
empathy_roster_model_manager_get_individuals (EmpathyRosterModel *model)
{
  EmpathyRosterModelManager *self = EMPATHY_ROSTER_MODEL_MANAGER (model);

  return empathy_individual_manager_get_members (self->priv->manager);
}

static void
roster_model_iface_init (EmpathyRosterModelInterface *iface)
{
  iface->get_individuals = empathy_roster_model_manager_get_individuals;
}