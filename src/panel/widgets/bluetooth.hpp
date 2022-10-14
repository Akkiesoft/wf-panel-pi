#ifndef WIDGETS_BLUETOOTH_HPP
#define WIDGETS_BLUETOOTH_HPP

#include "../widget.hpp"
#include <gtkmm/button.h>

extern "C" {
#include "bluetooth/bluetooth.h"
}

class WayfireBluetooth : public WayfireWidget
{
    std::unique_ptr <Gtk::Button> plugin;

    WfOption <int> icon_size {"panel/icon_size"};
    WfOption <std::string> bar_pos {"panel/position"};

    /* plugin */
    BluetoothPlugin data;
    BluetoothPlugin *bt;

  public:

    void init (Gtk::HBox *container) override;
    virtual ~WayfireBluetooth ();
    void icon_size_changed_cb (void);
    void bar_pos_changed_cb (void);
};

#endif /* end of include guard: WIDGETS_BLUETOOTH_HPP */