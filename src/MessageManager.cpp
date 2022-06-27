
#include <condition_variable>
#include <mutex>
#include <thread>
#include </home/criogenesis/Downloads/TwitchCppBot/include/MessageManager.hpp>

namespace TwitchBot 
{
    
    /**
     * This contains the private properties of a MessageManager instance.
     */
    struct MessageManager::Impl{
        /** 
         * This is the method to call in order to connect to the Twitch server.
         */
        ConnectionFactory connectionFactory;
        /**
         * This is the function to call when the user agent successfully logs
         * into the Twitch server.
         */
        LoggedInDelegate loggedInDelegate;

        /**
         * This is used to synchronize access to the object.
         */
        std::mutex mutex;

        /**
         * This is used to signal the worker thread to wake up.
         */
        std::condition_variable wakeWorker;

        /**
         * This flag indicates whether or not the worker should be stopped.
         */
        bool stopWorker = false;

        /**
         * This is used to preform background tasks for the object.
         */
        std::thread worker;
    };

    MessageManager::~MessageManager()
    {
        std::lock_guard< decltype(impl_->mutex) > lock(impl_->mutex);
    }

    MessageManager::MessageManager()
        : impl_ (new Impl())
    {

    }
    void MessageManager::SetSocketConnection(ConnectionFactory connectionFactory)
    {
        impl_ ->connectionFactory = connectionFactory;
    }

    void MessageManager::SetLoggedInDelegate(LoggedInDelegate loggedInDelegate)
    {
        impl_ ->loggedInDelegate = loggedInDelegate;
    }

    void MessageManager::LogIn(const std::string& nickname, const std::string& token)
    {

    }
    void MessageManager::LogOut()
    {

    }
}
