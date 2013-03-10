#ifndef H2INTERFACE_NETWORK_H
#define H2INTERFACE_NETWORK_H

class NetworkEvent;

namespace Interface
{
    class NetworkGui {
    public:
        static bool ProcessStateChange(const NetworkEvent&);
    };
}

#endif
