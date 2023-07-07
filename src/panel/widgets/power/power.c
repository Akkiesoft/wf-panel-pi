/*
Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <glib/gi18n.h>
#include <fcntl.h>
#include "power.h"

#define VMON_INTERVAL 15000
#define VMON_PATH "/sys/devices/platform/soc/soc:firmware/raspberrypi-hwmon/hwmon/hwmon1/in0_lcrit_alarm"

/* Plug-in global data */

/* Prototypes */

static gboolean is_pi (void);
static void update_icon (PowerPlugin *pt);
static gboolean vtimer_event (PowerPlugin *pt);


static gboolean is_pi (void)
{
    if (system ("raspi-config nonint is_pi") == 0)
        return TRUE;
    else
        return FALSE;
}

/* Read the current charge state and update the icon accordingly */

static void update_icon (PowerPlugin *pt)
{
    set_taskbar_icon (pt->tray_icon, "under-volt", pt->icon_size);
    if (!pt->show_icon)
    {
        gtk_widget_set_sensitive (pt->plugin, FALSE);
        gtk_widget_hide (pt->plugin);
    }
    else
    {
        gtk_widget_set_sensitive (pt->plugin, TRUE);
        gtk_widget_show (pt->plugin);
        gtk_widget_set_tooltip_text (pt->tray_icon, "Low voltage has been detected");
    }
}

static gboolean vtimer_event (PowerPlugin *pt)
{
    FILE *fp = fopen (VMON_PATH, "rb");
    if (fp)
    {
        int val = fgetc (fp);
        fclose (fp);
        if (val == '1')
        {
            lxpanel_critical (_("Low voltage warning\nPlease check your power supply"));
            pt->show_icon = TRUE;
            update_icon (pt);
        }
    }
    return TRUE;
}

/* Plugin functions */

/* Handler for system config changed message from panel */
void power_update_display (PowerPlugin *pt)
{
    update_icon (pt);
}

void power_init (PowerPlugin *pt)
{
    setlocale (LC_ALL, "");
    bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);

    /* Allocate icon as a child of top level */
    pt->tray_icon = gtk_image_new ();
    gtk_container_add (GTK_CONTAINER (pt->plugin), pt->tray_icon);

    pt->ispi = is_pi ();
    pt->show_icon = FALSE;

    /* Start timed events to monitor low voltage warnings */
    if (pt->ispi) pt->vtimer = g_timeout_add (VMON_INTERVAL, (GSourceFunc) vtimer_event, (gpointer) pt);
    else pt->vtimer = 0;

    update_icon (pt);

    /* Show the widget and return */
    gtk_widget_show_all (pt->plugin);
}

void power_destructor (gpointer user_data)
{
    PowerPlugin *pt = (PowerPlugin *) user_data;

    /* Disconnect the timer. */
    if (pt->vtimer) g_source_remove (pt->vtimer);
}

