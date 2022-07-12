#include <chrono>
#include <condition_variable>
#include <mutex>
#include <deque>
#include <queue>
#include <thread>
#include <vector>
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
     * This is the maximum amount of time to wait for the Twitch server to
     * provide the Message Of The Day (MOTD), confirming a successful log-in,
     * before timing out.
     */
    const std::chrono::seconds LOG_IN_TIMEOUT_PERIOD(1);

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
        LogOut,

        /**
         * Process all messages received from the Twitch server.
         */
        ProcessMessageRecieved
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
           used in the chat session.
         */
        std::string nickname;

        /**
         * This is used with the LogIn action, to provide the client with OAuth
         * token to to be used to authenticate with the server.
         */
        std::string token;

        /**
         * This is used with multiple actions, to provide the client with some
         * text to be sent to the server.
         */
        std::string message;
    };

    /**
     * This contains all the information parsed from a single message from the
     * Twitch server.
     *
     */
    struct Message
    {
        /**
         * If this is not an empty string, the message included is a prefix
         * which is stored here without the leading colon (:) character.
         */
        std::string prefix;

        /**
         * This is the command portion of the message, which may be a 3 digit
         * code, or an IRC command.
         *
         * If it's empty, the message was invalid or there was no message.
         */
        std::string command;

        /**
         * These are the parameters(If any), provided within the message.
         */
        std::vector< std::string > parameters;
    };

    /** 
     * This represents a condition that the worker is awaiting, which might time
     * out. 
     */
    struct TimeoutConditions
    {
        /**
         * This is the type of action which prompted the wait condition.
         */
        ActionType type;

        /**
         * This is the time, according to the time keeper,  at which the
         * condition will be considered to have timed out.
         */
         double expiration = 0.0;
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

        //Methods
        
        /**
         * This method extracts the next message received from the Twitch
         * server.
         *
         * @param[in,out] dataReceived All incoming data in the form of a buffer
         * to receive the characters coming in from the Twitch server, until a
         * complete line has been received, removed from the buffer, and
         * handeled.
         *
         * @param[out] message this is where to store the next message received.
         *
         * @return an indication of whether or not a complete line was extracted
         * is returned.
         */
        std::string GetNextMessage(std::string& dataReceived, Message& message))
        {
            // "dataReceived.find" will attempt to find the indeces of the where
            // CRLF would start in "dataReceived" and set it equal to "lineEnd"
            // (probably being returned back as an int)
            const auto lineEnd = dataReceived.find(CRLF);
            
            // We simply return out of the function if it fails to find the CRLF
            if (lineEnd == std::string::npos)
            {
                return false;
            }

            // "line" should now contain the revelant data without the CRLF
            const auto line = dataReceived.substr(0, lineEnd);

            // Remove the line from the buffer
            dataReceived = dataReceived.substr(lineEnd + CRLF.length());

            // Unpack the message from the line
            size_t offset = 0;
            int state = 0;
            message = Message();
            while (offset < line.length())
            {
                switch (state)
                {
                    // First character of the line could be ':', which singals a
                    // prefix or is the first charater of the command.
                    case 0:
                    {
                        if (line[offset] == ':')
                        {
                            state = 1;
                        }
                        else
                        {

                        }
                    } break;
                    
                    // Prefix
                    case 1:
                    {
                        if (line[offset] == ' ')
                        {
                            state = 2;
                        }
                        else
                        {
                            message.prefix += line[offset];
                        }

                    } break;

                    // First character of command
                    case 2:
                    {
                        if (line[offset] != ' ')
                        {
                            state = 3;
                            message.command += line[offset];
                        }

                    } break;
                    
                    // Command
                    case 3:
                    {
                        if (line[offset] == ' ')
                        {
                            state = 4;
                        }
                        else
                        {
                            message.command += line[offset];
                        }
                    } break;

                    // First character of parameter
                    case 4:
                    {
                        if (line[offset] == ':')
                        {
                            state = 6;
                            message.parameters.push_back("");
                        }
                        else if (line[offset] != ' ')
                        {
                            state = 5;
                            message.parameters.push_back(line.substr(offset, 1));
                        }

                    } break;
                    
                    // Parameter (not last, or last having no spaces)
                    case 5:
                    {
                        if (line[offset] == ' ')
                        {
                            state = 4;
                        }
                        else
                        {
                            message.parameter.back() += line[offset];
                        }
                    } break;

                    // Last Parameter (May include spaces)
                    case 6:
                    {
                        message.parameter.back() += line[offset];
                    } break;
                }
                ++offset;
            }

            // Invalid end states
            if ((state == 0) || (state == 1) || (state == 2))
            {
                message.command.clear();
                // If logging facility is being implemented in the future, an
                // error would be logged here for an invalid message.
            }
            return true;
        }

        /**
         * This method is called to whenever any message is received from the
         * Twitch server for the user agent.
         *
         * @param[in] rawText This is the raw text received from the Twitch
         * server.
         */
        void MessageReceived(const std::string& rawText)
        {
            std::lock_guard< decltype(mutex) > lock(mutex);
            Action action;
            action.type = ActionType::ProcessMessageRecieved;
            action.message = rawText;
            impl_->actions.push_back(action);
            impl_->wakeWorker.notify_one();
        }

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
            // This is the interface to the current connection to the Twitch
            // server, if we are connected.
            std::shared_ptr< Connection > connection;

            // All incoming data in the form of a buffer to receive the
            // characters coming in from the Twitch server, until a complete
            // line has been received, removed from the buffer, and handeled.
            std::string dataReceived;

            // This flag indiciates whether or not the client has finished
            // logging into the Twitch server (we've received the MOTD from the
            // server).
            bool loggedIn = false;


            // This holds onto any conditions that the worker is awaiting, which
            // might time out.
            std::priority_queue< TimeoutConditions > timeoutConditions;
            
            std::unique_lock< decltype(mutex) > lock(mutex);
            while(!stopWorker)
            {
                while(!actions.empty())
                {
                    const auto nextAction = actions.front();
                    actions.pop_front();
                    lock.unlock();
                    switch (nextAction.type)
                    {
                        case ActionType::LogIn:
                        {
                            if(connection != nullptr)
                            {
                                break;
                            }

                            connection = connectionFactory();
                            connection->SetMessageReceivedDelegate
                            (
                                std::bind(&Impl::MessageReceived, this, std::placeholders::_1)
                            );
                            if(connection->Connect())
                            {
                                connection->Send("Pass oauth:", nextAction.token + CRLF);
                                connection->Send("NICK", nextAction.nickname + CRLF);

                                if(timeKeeper != nullptr)
                                {
                                    TimeoutConditions timeoutConditions;
                                    timeoutConditions.type = ActionType::LogIn;
                                    timeoutConditions.expiration = timeKeeper->GetCurrentTime() + LOG_IN_TIMEOUT_PERIOD;
                                }
                                if(loggedInDelegate != nullptr)
                                {
                                    loggedInDelegate();
                                }
                            }
                            else
                            {
                                if(loggedOutDelegate != nullptr)
                                {
                                    loggedOutDelegate();
                                }
                            }
                            
                            
                        } break;

                        case ActionType::LogOut: 
                        {
                            if(connection != nullptr)
                            {
                                connection->Send("QUIT :", nextAction.message + CRLF);
                                connection->Disconnect();
                                //Close the connection (after data is
                                //transmitted)
                                {
                                    
                                }
                                if(loggedOutDelegate != nullptr)
                                {
                                    loggedOutDelegate();
                                }
                            }
                            
                        } break;

                        case ActionType::ProcessMessageRecieved:
                        {
                            dataReceived += nextAction.message;
                            Message message;
                            while(GetNextMessage(dataRecieved, message))
                            {
                                if (message.command.empty())
                                {
                                    continue;
                                }
                                if (message.command == "376") // RPL_ENDOFMOTD (RFC_1459)
                                {
                                    if(!loggedIn)
                                    {
                                        loggedIn = true;
                                        if (loggedInDelegate != nullptr)
                                        {
                                            loggedInDelegate();
                                        }
                                    }
                                }
                            }
                        } break;

                        // Potentially place diagnostic actions inside this
                        // function for the future.
                        //
                        // Example: "You gave me an action that I do not
                        // understand etc."
                        defafult: 
                        {
                            
                        } break;
                    }
                    lock.lock();
                }

                if (!actions.empty() && actions.front().delay)
                {
                    wakeWorker.wait_for
                    (
                        lock,
                        std::chrono::miliseconds(50),
                        [this]
                        {
                            return stopWorker;
                        }
                    );
                }
                else
                {
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
        action.type = ActionType::LogIn;
        action.nickname = nickname;
        action.token = token;
        impl_->actions.push_back(action);
        impl_->wakeWorker.notify_one();

    }

    void MessageManager::LogOut(const std::string& farewell)
    {
        std::lock_guard< decltype(impl_->mutex) > lock(impl_->mutex);
        Action action;
        action.type = ActionType::LogOut;
        action.message = farewell;
        impl_->actions.push_back(action);
        impl_->wakeWorker.notify_one();
    }
}
