#include <string>
#include <vector>
#include <memory>


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
            /**
            * This method is called to establish a connection to the Twitch
            * server.
            *
            * @return returns an indication of whether or not the connection was
            * succesful. This is a synchronous call; the connection will either
            * succeed or fail befor the method returns.
            */
            virtual bool Connect() = 0;

            /**
            * This method queues the given message to be sent to the Twitch
            * server. This is an asynchronous call; the message may or may not
            * be sent before the method returns
            *
            * @param[in] message This is the text to send to the Twitch server
            */
            virtual void Send(const std::string& message) = 0;
        

    };
}