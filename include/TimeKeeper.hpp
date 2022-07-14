
namespace TwitchBot
{

    /**
     * This represents the time Keeping requirements of the Twitch::Server. To
     * integrate the Twitch::Server into a larger program, implement this
     * interface in terms of the actual server time.
     */
    class TimeKeeper
    {
        public:
        // Methods

        /**
         * This method returns the current server time, in seconds.
         *
         * @return The current server time, in seconds.
         */
        virtual double GetCurrentTime() = 0;
    };
}