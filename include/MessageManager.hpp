#include <string>
#include <vector>
#include <functional>
#include <memory>

#include </home/criogenesis/Downloads/TwitchCppBot/include/Connection.hpp>
#include </home/criogenesis/Downloads/TwitchCppBot/include/TimeKeeper.hpp>

namespace TwitchBot
{
    /**
     * This class represents a MessageManager agent that connects to the chat,
     * sends messages to the chat, and reads the user input from the chat.
     */
    class MessageManager
    {
        // Types
        public:
            /**
             * @brief This is the type of function used by the class to create
             * new connections to the twitch server.
             */
            typedef std::function< std::shared_ptr< Connection >() > ConnectionFactory;

            /**
             * @brief This is the type of function used to notify the user when
             * the agent has successfully logged into the Twitch server.
             */
            typedef std::function < void() > LoggedInDelegate;

            /**
             * @brief This is the type of function used to notify the user when
             * the agent completes logging out of the Twitch server, or when the
             * connection is closed, or when it could not be established in the
             * first place.
             */
            typedef std::function < void() > LoggedOutDelegate;

            typedef std::function < void(const std::string& channel, const std::string& user)) > JoinDelegate;

        // Lifecycle Management
        public:
            ~MessageManager() noexcept;
            MessageManager(const MessageManager& other) = delete;
            MessageManager(MessageManager&&) noexcept = delete;
            MessageManager& operator=(const MessageManager& other) = delete;
            MessageManager& operator=(MessageManager&&) noexcept = delete;

        // Beginning of Public Methods
        public:
            /**
             * Default constructor
             */
            MessageManager();

            /**
             * @brief This method will provide a connectionFactory object with
             * the ability to connect to the Twitch server.
             *
             * @param[in] connectionFactory This is the method to call in order
             * to connect to the Twitch server.
             */
            void SetConnectionFactory(ConnectionFactory connectionFactory);

            /**
             * @brief This method will provide a means of measuring elapsed time
             * periods.
             *
             * @param[in] timeKeeper This is the object used to measure elapsed
             * time periods.
             */
            void SetTimeKeeper(std::shared_ptr< TimeKeeper > timeKeeper);
            
            /**
             * @brief This method is is called to setup a callback to happen
             * when the user agent successfully logs into the Twitch server.
             *
             * @param[in] loggedInDelegate This is the function to call when the
             * user agent successfully logs into the Twitch server.
             */
            void SetLoggedInDelegate(LoggedInDelegate loggedInDelegate);

            /**
             * @brief This method is is called to setup a callback to happen
             * when the user agent completes logging out the Twitch server.
             *
             * @param[in] loggedOutDelegate This is the function to call when
             * the user agent complets logging out the Twitch server.
             */
            void SetLoggedOutDelegate(LoggedOutDelegate loggedOutDelegate);


            void SetJoinDelegate(JoinDelegate joinDelegate);

            /**
             * @brief This method starts the process of logging into the Twitch
             * server.
             *
             * @param[in] nickname This is the nickname associated to the twitch
             * user account.
             *
             * @param[in] token This is the oauth token associated to the user
             * account used for authentication with the Twitch server.
             */
            void LogIn(
                const std::string& nickname,
                const std::string& token
            );

            /**
             * this process starts the progress of logging out of the Twitch
             * server.
             *
             * @param[in] farewell this is the message sent back to the Twitch
             * server just before the connection is closed.
             */
            void LogOut(const std::string& farewell);


            /**
             * This method starts the process of joining the Twitch channel
             *
             * @param[in] channel This is the name of the channel to join.
             */
            void Join(const std::string& channel);

        private:
            /**
             * A struct that contains the private properties of the instance.
             * This is defined within the implementation and declared here to
             * ensure that it is scoped within the class.
             */
            struct Impl;

            /**
             * This contains the private properties of the instance.
             */
            std::unique_ptr< Impl > impl_;

    };
}