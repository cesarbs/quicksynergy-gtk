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
#include "intl.h"

#include <unistd.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "logo/qslogo.h"
#include "callbacks.h"
#include "synergy_config.h"
#include "ui.h"

GdkPixbuf *make_logo() {
    return gdk_pixbuf_new_from_inline(-1, qslogo, FALSE, NULL);
}

GtkWidget *screen_entry_new(char **textp, const char *position) {
    GtkWidget *entry;

    entry = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(entry), *textp);
    gtk_entry_set_alignment(GTK_ENTRY(entry), 0.5);
    gtk_entry_set_width_chars(GTK_ENTRY(entry), 10);

    g_signal_connect(G_OBJECT(entry), "focus-in-event",
        G_CALLBACK(entry_focus_in_event), (gpointer) position);

    g_signal_connect(G_OBJECT(entry), "focus-out-event",
        G_CALLBACK(entry_focus_out_event), (gpointer) position);

    g_signal_connect(G_OBJECT(entry), "changed",
        G_CALLBACK(entry_changed_cb), (gpointer) textp);

    return entry;
}

GtkWidget *make_server_tab(qs_state_t *state) {
    GtkWidget *table;
    GtkWidget *vbox;
    GtkWidget *image;
    GtkWidget *label;
    GtkWidget *above_entry;
    GtkWidget *below_entry;
    GtkWidget *left_entry;
    GtkWidget *right_entry;
    char hostname[64];

    /* build the table that will hold the server layout widgets */
    table = gtk_table_new(3, 3, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(table), 12);

    /* text entries for server configuration */
    above_entry = screen_entry_new(&state->above, _("Above"));
    below_entry = screen_entry_new(&state->below, _("Below"));
    left_entry = screen_entry_new(&state->left, _("Left"));
    right_entry = screen_entry_new(&state->right, _("Right"));

    /* attach entries to table */
    gtk_table_attach_defaults(GTK_TABLE(table), above_entry, 1, 2, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(table), below_entry, 1, 2, 2, 3);
    gtk_table_attach_defaults(GTK_TABLE(table), left_entry, 0, 1, 1, 2);
    gtk_table_attach_defaults(GTK_TABLE(table), right_entry, 2, 3, 1, 2);

    /* vbox which will hold the center image and label */
    vbox = gtk_vbox_new(FALSE, 6);
    gtk_table_attach_defaults(GTK_TABLE(table), vbox, 1, 2, 1, 2);

    /* image to be displayed in the center of the main window */
    image = gtk_image_new_from_stock(GTK_STOCK_HOME, GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

    /* label with computer's hostname to be placed under the center image */
    if(!gethostname(hostname, sizeof(hostname))) {
        label = gtk_label_new(hostname);
        gtk_box_pack_start(GTK_BOX(vbox), label,  FALSE, FALSE, 0);
    }

    return table;
}

GtkWidget *make_client_tab(qs_state_t *state) {
    GtkWidget *vbox, *vbox1;
    GtkWidget *label;
    GtkWidget *entry;

    /* client's vbox */
    vbox1 = gtk_vbox_new(FALSE, 18);
    gtk_container_set_border_width(GTK_CONTAINER(vbox1), 12);

    /* hostname */
    vbox = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox1), vbox, FALSE, FALSE, 0);

    label = gtk_label_new(_("Server hostname/IP address:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    /* entry for placing the server's hostname/IP address */
    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), state->hostname);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    /* update state information when text changes */
    g_signal_connect(G_OBJECT(entry), "changed",
        G_CALLBACK(entry_changed_cb), (gpointer) &state->hostname);

    return vbox1;
}

GtkWidget *make_settings_tab(qs_state_t *state) {
    GtkWidget *vbox;
    GtkWidget *vbox1;
    GtkWidget *hbox;
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *check_button;
    GtkWidget *entry;

    /* general vbox */
    vbox = gtk_vbox_new(FALSE, 18);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);

    /* share settings vbox */
    vbox1 = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), vbox1, TRUE, TRUE, 0);

    /* share settings label */
    label = gtk_label_new(_("<b>Share</b>"));
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox1), label, FALSE, FALSE, 0);

    /* items hbox */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox, TRUE, TRUE, 0);

    /* dummy label for spacing */
    label = gtk_label_new("  ");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    /* items vbox */
    vbox1 = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 0);

    /* ssh tunnel configuration */
    check_button = gtk_check_button_new_with_label(
        _("Secure connection with SSH tunnel"));
    gtk_box_pack_start(GTK_BOX(vbox1), check_button, FALSE, FALSE, 0);
/*
    g_signal_connect(G_OBJECT(entry), "changed",
        G_CALLBACK(entry_changed_cb), (gpointer) &state->client_name);
*/
    /* use settings vbox */
    vbox1 = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), vbox1, TRUE, TRUE, 0);

    /* share settings label */
    label = gtk_label_new(_("<b>Use</b>"));
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox1), label, FALSE, FALSE, 0);

    /* items hbox */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox, TRUE, TRUE, 0);

    /* dummy label for spacing */
    label = gtk_label_new("  ");
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    /* items vbox */
    vbox1 = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(hbox), vbox1, TRUE, TRUE, 0);

    /* screen name */
    hbox = gtk_hbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox, FALSE, FALSE, 0);

    label = gtk_label_new(_("Screen name:"));
    gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), state->client_name);
    gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(entry), "changed",
        G_CALLBACK(entry_changed_cb), (gpointer) &state->client_name);

    return vbox;
}

