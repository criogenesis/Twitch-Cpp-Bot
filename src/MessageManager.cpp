
#include <condition_variable>
#include <mutex>
#include <deque>
#include <thread>
#include </home/criogenesis/Downloads/TwitchCppBot/include/MessageManager.hpp>

namespace
{
    /**
     * @brief This is the required line terminator for lines of text sent to or
     * from the Twitch server.
     *
     */
    const std::string CRLF = "\r\n";

    /**
     * These are the states in which the MessageManager class can be.
     */
    enum class State
    {
        /**
         * The client has either not logged in, or has logged out. There is no
         * active connection.
         */
        NotLoggedIn,

        /**
         * The client has completely logged into the server with an active
         * connection.
         */
        LoggedIn
    };

    /**
     * These are the types of actions that the MessageManager worker can
     * perform.
     */
    enum class ActionType
    {
        /**
         * Establish a new connection to Twitch chat, and use the new connection
         * to log in.
         */
        LogIn,

        /**
         * LogOut of Twitch chat, and close thee active connection.
         */
        LogOut
    };

    /**
     * This is used to convey all actions by the MessageManager class worker to
     * perform, including the perameters.
     */
    struct Action
    {
        /**
         * This is the type of action to perform.
         */
        ActionType type;

        /**
         * This is used with the LogIn action, to provide the nickname to be
           used
         * in the chat session.
        * /
        std::string nickname;
        /**
         * This is used with the LogIn action, to provide the client with OAuth
         * token to to be used to authenticate with the server.
         */
        std::string token;

        /**
         * This is used with multiple actions, to provide the client with some
         * text to be sent to the server
         */
        std::string message;
    };
}

namespace TwitchBot 
{
    
    /**
     * This contains the private properties of a MessageManager instance.
     */
    struct MessageManager::Impl
    {
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
         * This is the function to call when the user agent completely logs out
         * of the Twitch server.
         */
        LoggedOutDelegate loggedOutDelegate;

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

        /**
         * These are the actions to be performed by the worker thread.
         */
        std::deque< Action > actions;

        /**
         * This is the interface to the current connection to the Twitch server,
         * if we are connected
         */
        std::shared_ptr< Connection > connection;

        //Methods

        /**
         * This method signals the worker thread to stop.
         */
        void StopWorker()
        {
            std::lock_guard< decltype(mutex) > lock(mutex);
            stopWorker = true;
            wakeWorker.notify_one();
        }

        /**
         * This runs its own thread and performs background tasks for the
         * object.
         */
        void Worker()
        {
            std::unique_lock< decltype(mutex) > lock(mutex);
            while(!stopWorker)
            {
                while(!actions.empty())
                {
                    const auto nextAction = actions.front();
                    actions.pop_front();
                    switch (nextAction.type)
                    {
                        case ActionType::LogIn:
                        {
                            if(connection != nullptr)
                            {
                                // TODO: Write code here in case LogIn is asked
                                // and we're already logged in.
                            }

                            connection = connectionFactory();
                            if(connection->Connect())
                            {
                                connection->Send("Pass oauth:", nextAction.token + CRLF);
                                connection->Send("NICK", nextAction.nickname + CRLF);
                            }
                            else
                            {
                                // TODO: Need to do something if this fails
                            }
                            
                            break;
                        }
                        case ActionType::LogOut: 
                        {
                            if(connection != nullptr)
                            {
                                connection->Send("QUIT :", nextAction.message + CRLF);
                                
                                //Close the connection (after data is
                                //transmitted)
                                {
                                    
                                }
                            }
                            break;
                        }

                        // Potentially place diagnostic actions inside this
                        // function for the future.
                        //
                        // Example: "You gave me an action that I do not
                        // understand etc."
                        defafult: 
                        {
                            break;
                        }
                    }
                }
                wakeWorker.wait
                (
                    lock,
                    [this]
                    {
                        return
                        (
                            stopWorker 
                            ||
                            !actions.empty()
                        );
                    }
                );
            
            }
        }
    };
    
    MessageManager::~MessageManager()
    {
        impl_->StopWorker();
        impl_->worker.join();
    }

    MessageManager::MessageManager()
        : impl_ (new Impl())
    {
        impl_->worker = std::thread(&Impl::Worker, impl_.get());
    }

    void MessageManager::SetSocketConnection(ConnectionFactory connectionFactory)
    {
        impl_ ->connectionFactory = connectionFactory;
    }

    void MessageManager::SetLoggedInDelegate(LoggedInDelegate loggedInDelegate)
    {
        impl_ ->loggedInDelegate = loggedInDelegate;
    }

    void MessageManager::SetLoggedOutDelegate(LoggedOutDelegate loggedOutDelegate)
    {
        impl_ ->loggedOutDelegate = loggedOutDelegate;
    }

    void MessageManager::LogIn(const std::string& nickname, const std::string& token)
    {
        std::lock_guard< decltype(impl_->mutex) > lock(impl_->mutex);
        Action action;
        action.nickname = nickname;
        action.token = token;
        action.type = ActionType::LogIn;
        impl_->actions.push_back(action);
        impl_->wakeWorker.notify_one();

    }

    void MessageManager::LogOut()
    {
        std::lock_guard< decltype(impl_->mutex) > lock(impl_->mutex);
        Action action;
        action.type = ActionType::LogOut;
        impl_->actions.push_back(action);
        impl_->wakeWorker.notify_one();
    }
}
