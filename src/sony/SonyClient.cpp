
#include "SonyClient.h"
#include "Logger.h"
#include <chrono>
#include <unistd.h> // UNIX ONLY


namespace MagicPodsCore
{
    SonyClient::SonyClient(std::string tag) : _tag{tag}
    {
    }

    void SonyClient::WaitResult(SonyBaseCmd &cmd, int timeOutSeconds)
    {
        //TODO: Check connection. If connection is disconnected we unable to send cmd

        //TODO: Subscribe to bytes from headphones cmd.ProcessResponse += 

        unsigned char prefix = Prefix;

        for (size_t i = 0; i <= 1; i++)
        {
            if (i == 1){
                prefix = prefix == 0? (unsigned char)1 : (unsigned char)0;
            }

            auto requestBytes = cmd.Request(prefix);
            //TODO: Send requestBytes to headphones
            
            auto timeout = std::chrono::system_clock::now() + std::chrono::seconds(timeOutSeconds);
            while (std::chrono::system_clock::now() < timeout)
            {
                usleep(10000); //10 ms UNIX only

                if (cmd.IsProcessed)
                    break;
            }

            if (cmd.IsProcessed)
                break;            
        }        
    }

    void SonyClient::ProcessResponse(const std::vector<unsigned char> &bytes)
    {
        if (bytes.size() >= 2 && bytes.size() != 9 && bytes[1] != 1)
        {

            // Next command should use inverse prefix.
            // If headphones send command with prefix 0,
            // command from our application must set prefix 1 and vice versa
            auto response = bytes[2] == 0 ? std::vector<unsigned char>{0x3e, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x3c} : std::vector<unsigned char>{0x3e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3c};
            Prefix = bytes[2] == 0 ? (unsigned char)1 : (unsigned char)0;
            LOG_DEBUG("%s New prefix %d", _tag.c_str(), Prefix);

            // Notifies the headphones that the command has been successfully received
            // Send verification response like 3e010000000000013c or 3e010100000000023c
            // TODO: Send response to Sony headphones
        }

        // TODO: Event bytes received from Sony headphones
    }
}