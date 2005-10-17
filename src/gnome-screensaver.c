/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 8; tab-width: 8 -*-
 *
 * Copyright (C) 2004-2005 William Jon McCann <mccann@jhu.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Authors: William Jon McCann <mccann@jhu.edu>
 *
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gnome-screensaver.h"
#include "gs-monitor.h"

void
gnome_screensaver_quit (void)
{
        gtk_main_quit ();
}

int
main (int    argc,
      char **argv)
{
        GSMonitor          *monitor;
        GError             *error = NULL;
        static gboolean     show_version = FALSE;
        static gboolean     no_daemon    = FALSE;
        static GOptionEntry entries []   = {
                { "version", 0, 0, G_OPTION_ARG_NONE, &show_version,
                  N_("Version of this application"), NULL },
                { "no-daemon", 0, 0, G_OPTION_ARG_NONE, &no_daemon,
                  N_("Don't become a daemon"), NULL },
                { NULL }
        };

#ifdef ENABLE_NLS
        bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
# ifdef HAVE_BIND_TEXTDOMAIN_CODESET
        bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
# endif 
        textdomain (GETTEXT_PACKAGE);
#endif 

        if (! gtk_init_with_args (&argc, &argv, NULL, entries, NULL, &error)) {
                if (error) {
                        g_warning ("%s", error->message);
                        g_error_free (error);
                } else {
                        g_warning ("Unable to initialize GTK+");
                }
                exit (1);
        }

        if (show_version) {
                g_print ("%s %s\n", argv [0], VERSION);
                exit (1);
        }

        monitor = gs_monitor_new ();

        if (! monitor)
                exit (1);

        error = NULL;
        if (! gs_monitor_start (monitor, &error)) {
                if (error) {
                        g_warning ("%s", error->message);
                        g_error_free (error);
                } else {
                        g_warning ("Unable to start screensaver");
                }
                exit (1);
        }

        /* Don't close stdout and stderr for now */
        if (! no_daemon && daemon (0, 1))
                g_error ("Could not daemonize: %s", g_strerror (errno));

        gtk_main ();

        g_object_unref (monitor);

	return 0;
}
