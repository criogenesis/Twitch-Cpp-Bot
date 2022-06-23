#include <string>
#include <vector>
#include <memory>


namespace TwitchBot
{
    /**
     * This class represents a MessageManager agent that connects to the chat, 
     * sends messages to the chat,
     * and reads the user input from the chat.
     */
    class MessageManager
    {
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

        private:
            /**
             * A struct that contains the private properties of the
             * instance. This is defined within the implementation and declared
             * here to ensure that it is scoped within the class.
             */
            struct Impl;

            /**
             * This contains the private properties of the instance.
             */
            std::unique_ptr< Impl > impl_;

    };
}