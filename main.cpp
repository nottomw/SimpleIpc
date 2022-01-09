#include "SimpleIpc.hpp"
#include "SystemWideLockIf.hpp"

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// TODO: add message produced callback (to signall other process/other core)

class DummySystemWideLock : public kiss::SystemWideLockIf
{
  public:
    virtual void lock()
    {
    }

    virtual void unlock()
    {
    }
};

int main(int argc, char **)
{
    kiss::SharedMem metaMem("/shm_meta", 2048);
    kiss::SharedMem mem("shm_data", 5120);
    DummySystemWideLock systemLock;

    if (argc <= 1)
    {
        printf("CONSUMER\n");
        kiss::SimpleIpc ipcConsumer(metaMem, mem, systemLock);

        while (true)
        {
            printf("Click to receive...\n");
            const int d = getchar();
            (void)d;

            uint8_t buffer[1024] = {0};
            auto res = ipcConsumer.receiveMessage(&buffer[0], 1024);
            if (res == kiss::SimpleIpc::Result::OK)
            {
                printf("Message received: %s\n", buffer);
            }
            else
            {
                printf("No new messages: %d\n", (uint32_t)res);
            }
        }
    }
    else
    {
        printf("PRODUCER\n");
        kiss::SimpleIpc ipcProducer(metaMem, mem, systemLock);

        while (true)
        {
            std::string userInput;
            std::cin >> userInput;

            std::cout << "Sending: " << userInput << std::endl;

            userInput += '\0';

            uint8_t *userInputTest = (uint8_t *)userInput.c_str();

            auto sendRes = ipcProducer.sendMessage(userInputTest, userInput.size());
            if (sendRes != kiss::SimpleIpc::Result::OK)
            {
                std::cout << "send error" << std::endl;
            }
            else
            {
                std::cout << "done" << std::endl;
            }
        }
    }

    return 0;
}
