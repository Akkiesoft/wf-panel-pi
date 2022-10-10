#include <glibmm.h>
#include "netman.hpp"

void WayfireNetman::bar_pos_changed_cb (void)
{
    if ((std::string) bar_pos == "bottom") nm->bottom = TRUE;
    else nm->bottom = FALSE;
}

void WayfireNetman::icon_size_changed_cb (void)
{
    nm->icon_size = icon_size;
    netman_update_display (nm);
}

void WayfireNetman::init (Gtk::HBox *container)
{
    /* Create the button */
    plugin = std::make_unique <Gtk::Button> ();
    container->pack_start (*plugin, false, false);

    /* Setup structure */
    nm = &data;
    nm->plugin = (GtkWidget *)((*plugin).gobj());
    nm->icon_size = icon_size;

    /* Initialise the plugin */
    netman_init (nm);

    /* Setup icon size callback and force an update of the icon */
    icon_size.set_callback (sigc::mem_fun (*this, &WayfireNetman::icon_size_changed_cb));
    icon_size_changed_cb ();

    /* Setup bar position callback */
    bar_pos.set_callback (sigc::mem_fun (*this, &WayfireNetman::bar_pos_changed_cb));
    bar_pos_changed_cb ();
}

WayfireNetman::~WayfireNetman()
{
}
