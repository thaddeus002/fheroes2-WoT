
#include "dialog.h"
#include "network.h"
#include "interface_network.h"

namespace Interface {

bool NetworkGui::ProcessStateChange(const NetworkEvent &ev)
{
    switch(ev.NewState)
    {
        case ST_ERROR:
            Dialog::Message("Error", ev.ErrorMessage, Font::BIG, Dialog::OK);
            return false;

        case ST_DISCONNECTED:
            Dialog::Message("Error", "Disconnected from server", Font::BIG, Dialog::OK);
            return false;
        default:
            break;
    }

    return true;
}

};
