/*  QuickSynergy -- a GUI for synergy
 *  Copyright (C) 2006, 2007, 2008, 2009 Cesar L. B. Silveira, Otavio C. Cordeiro
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib.h>
#include "synergy_config.h"
#include "ui.h"
#include "intl.h"

qs_state_t *load_config() {
    const char *home_dir = getenv("HOME");
    qs_state_t *state;
    GKeyFile *key_file;

    chdir(home_dir);

    key_file = g_key_file_new();
    g_key_file_load_from_file(key_file, QS_CONF_DIR QS_CONF_FILE,
        G_KEY_FILE_NONE, NULL);

    state = (qs_state_t *) malloc(sizeof(qs_state_t));

    state->data.above =
        (g_key_file_has_key(key_file, "Share", "Above", NULL)   ?
         g_key_file_get_value(key_file, "Share", "Above", NULL) :
         _("Above"));

    state->data.below =
        (g_key_file_has_key(key_file, "Share", "Below", NULL)   ?
         g_key_file_get_value(key_file, "Share", "Below", NULL) :
         _("Below"));

    state->data.left =
        (g_key_file_has_key(key_file, "Share", "Left", NULL) ?
         g_key_file_get_value(key_file, "Share", "Left", NULL) :
         _("Left"));

    state->data.right =
        (g_key_file_has_key(key_file, "Share", "Right", NULL) ?
         g_key_file_get_value(key_file, "Share", "Right", NULL) :
         _("Right"));

    state->data.req_tunnel =
        (g_key_file_has_key(key_file, "Require", "Tunnel", NULL) ?
         g_key_file_get_boolean(key_file, "Require", "Tunnel", NULL) :
         0);

    state->data.hostname =
        (g_key_file_has_key(key_file, "Use", "Hostname", NULL) ?
         g_key_file_get_value(key_file, "Use", "Hostname", NULL) :
         "");

    state->data.client_name =
        (g_key_file_has_key(key_file, "Use", "ClientName", NULL) ?
         g_key_file_get_value(key_file, "Use", "ClientName", NULL) :
         "");

    state->data.use_socks =
        (g_key_file_has_key(key_file, "Use", "SOCKS", NULL) ?
         g_key_file_get_boolean(key_file, "Use", "SOCKS", NULL) :
         0);

    state->ui.current_page =
        (g_key_file_has_key(key_file, "Settings", "LastPage", NULL) ?
         g_key_file_get_integer(key_file, "Settings", "LastPage", NULL) :
         0);

    state->proc.running = 0;

    g_key_file_free(key_file);

    return state;
}

void save_config(qs_state_t *state) {
    const char *home_dir = getenv("HOME");
    GKeyFile *key_file;
    gsize length;
    gchar *data;

    chdir(home_dir);

    mkdir(QS_CONF_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    key_file = g_key_file_new();

    if(g_strcmp0(state->data.above, _("Above"))) {
        g_key_file_set_value(key_file, "Share", "Above", state->data.above);
    }

    if(g_strcmp0(state->data.below, _("Below"))) {
        g_key_file_set_value(key_file, "Share", "Below", state->data.below);
    }

    if(g_strcmp0(state->data.left, _("Left"))) {
        g_key_file_set_value(key_file, "Share", "Left", state->data.left);
    }

    if(g_strcmp0(state->data.right, _("Right"))) {
        g_key_file_set_value(key_file, "Share", "Right", state->data.right);
    }

    g_key_file_set_boolean(key_file, "Require", "Tunnel",
                           state->data.req_tunnel);

    g_key_file_set_value(key_file, "Use", "Hostname", state->data.hostname);

    g_key_file_set_value(key_file, "Use", "ClientName",
                         state->data.client_name);

    g_key_file_set_boolean(key_file, "Use", "SOCKS",
                           state->data.use_socks);

    g_key_file_set_integer(key_file, "Settings", "LastPage",
                           state->ui.current_page);

    data = g_key_file_to_data(key_file, &length, NULL);
    g_file_set_contents(QS_CONF_DIR QS_CONF_FILE, data, length, NULL);

    g_key_file_free(key_file);
}

void save_synergy_config(qs_state_t *state) {
    const char *home_dir = getenv("HOME");
    char hostname[64];
    FILE *f;

    chdir(home_dir);

    mkdir(QS_CONF_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    f = fopen(QS_CONF_DIR QS_SYNERGY_CONF_FILE, "w");

    gethostname(hostname, 64);

    fprintf(f, "section: screens\n");
    fprintf(f, "\t%s:\n", hostname);

    if(strcmp(state->data.above, _("Above"))) {
        fprintf(f, "\t%s:\n", state->data.above);
    }

    if(strcmp(state->data.below, _("Below"))) {
        fprintf(f, "\t%s:\n", state->data.below);
    }

    if(strcmp(state->data.left, _("Left"))) {
        fprintf(f, "\t%s:\n", state->data.left);
    }

    if(strcmp(state->data.right, _("Right"))) {
        fprintf(f, "\t%s:\n", state->data.right);
    }

    fprintf(f, "end\n");

    /* server links */
    fprintf(f, "section: links\n");
    fprintf(f, "\t%s:\n", hostname);

    if(strcmp(state->data.above, _("Above"))) {
        fprintf(f, "\t\tup = %s\n", state->data.above);
    }

    if(strcmp(state->data.below, _("Below"))) {
        fprintf(f, "\t\tdown = %s\n", state->data.below);
    }

    if(strcmp(state->data.left, _("Left"))) {
        fprintf(f, "\t\tleft = %s\n", state->data.left);
    }

    if(strcmp(state->data.right, _("Right"))) {
        fprintf(f, "\t\tright = %s\n", state->data.right);
    }

    /* client links */
    if(strcmp(state->data.above, _("Above"))) {
        fprintf(f, "\t%s:\n", state->data.above);
        fprintf(f, "\t\tdown = %s\n", hostname);
    }

    if(strcmp(state->data.below, _("Below"))) {
        fprintf(f, "\t%s:\n", state->data.below);
        fprintf(f, "\t\tup = %s\n", hostname);
    }

    if(strcmp(state->data.left, _("Left"))) {
        fprintf(f, "\t%s:\n", state->data.left);
        fprintf(f, "\t\tright = %s\n", hostname);
    }

    if(strcmp(state->data.right, _("Right"))) {
        fprintf(f, "\t%s:\n", state->data.right);
        fprintf(f, "\t\tleft = %s\n", hostname);
    }

    fprintf(f, "end\n");

    fclose(f);
}
