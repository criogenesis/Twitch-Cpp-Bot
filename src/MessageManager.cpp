

#include </home/criogenesis/Downloads/TwitchCppBot/include/MessageManager.hpp>

namespace TwitchBot 
{
    
    /**
    * This contains the private properties of a MessageManager instance.
    */
    struct MessageManager::Impl{

    };

    MessageManager::~MessageManager() = default;

    MessageManager::MessageManager()
        : impl_ (new Impl())
    {

    }
}
