#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace TwitchBot
{
    /**
     * This interface is required for the MessageManager class inorder to
     * communicate to the twitch server. This represents a connection between
     * the client and the server.
     */
    class Connection
    {
        
        public:

            //Types

            /**
             * This is the type of function to call whenever a message is
             * received from the Twitch Server.
             *
             * @param message This is the message being received from the Twitch
             * server.
             */
            typedef std::function< void(const std::string& message) > MessageReceivedDelegate;

            /**
             * This is the type of function to call when the Twitch server
             * closes its end of the connection.
             */
            typedef std::function< void() > DisconnectedDelegate;

            // Methods

            /**
             * This method is is called to setup a callback to happen whenever
             * any message is received from the Twitch server or the user agent.
             *
             * @param[in] messageReceivedDelegate This is the function to call
             * whenever any message is received from the Twitch server or the
             * user agent.
             */
            virtual void SetMessageReceivedDelegate(MessageReceivedDelegate messageReceivedDelegate) = 0;


            /**
             * This method is is called to setup a callback to happen when the
             * Twitch server, closes its end of the connection.
             *
             * @param[in] disconnectedDelegate This is the function to call when
             * the Twitch server, closes its end of the connection.
             */
            virtual void SetDisconnectedDelegate(DisconnectedDelegate disconnectedDelegate) = 0;

            /**
             * This method is called to establish a connection to the Twitch
             * server.
             *
             * @return returns an indication of whether or not the connection
             * was succesful. This is a synchronous call; the connection will
             * either succeed or fail befor the method returns.
             */
            virtual bool Connect() = 0;

            /**
             * This method is called to release a connection from the Twitch
             * server. This is a synchronous call; the connection will be
             * disconnected before the method returns.
             */
            virtual bool Disconnect() = 0;

            /**
             * This method queues the given message to be sent to the Twitch
             * server. This is an asynchronous call; the message may or may not
             * be sent before the method returns
             *
             * @param[in] message This is the text to send to the Twitch server
             */
            virtual void Send(const std::string& message) = 0;

            /**
             * Trims the front and back white space in a string.
             *
             *@param[in] exampleString the string to trim
             */
            void TrimWhiteSpaceFrontBack(const std::string exampleString) = 0;

    };
}