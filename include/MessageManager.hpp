#include <string>
#include <vector>
#include <functional>
#include <memory>

#include </home/criogenesis/Downloads/TwitchCppBot/include/Connection.hpp>

namespace TwitchBot
{
    /**
     * This class represents a MessageManager agent that connects to the chat,
     * sends messages to the chat, and reads the user input from the chat.
     */
    class MessageManager
    {
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
             * the agent completes logging out of the Twitch server.
             */
            typedef std::function < void() > LoggedOutDelegate;

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
            void SetSocketConnection(ConnectionFactory connectionFactory);
            
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
             * @brief This process starts the progress of logging out of the
             * Twitch server.
             */
            void LogOut();

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