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
    kiss::SharedMem mem(128);
    DummySystemWideLock systemLock;

    if (argc <= 1)
    {
        printf("CONSUMER\n");
        kiss::SimpleIpc ipcConsumer(mem, systemLock);

        while (true)
        {
            printf("Click to receive...\n");
            const int d = getchar();
            (void)d;

            kiss::IpcMessage *msg = ipcConsumer.receiveStart();
            if (msg != nullptr)
            {
                printf("Message received: %s\n", msg->getBuffer());
                ipcConsumer.receiveEnd(msg);
            }
            else
            {
                printf("No new messages\n");
            }
        }
    }
    else
    {
        printf("PRODUCER\n");
        kiss::SimpleIpc ipcProducer(mem, systemLock);

        while (true)
        {
            std::string userInput;
            std::cin >> userInput;

            std::cout << "Sending: " << userInput << std::endl;

            auto &ipcMsg = ipcProducer.messageBufferFetch(userInput.size() + 1);
            auto *buf = ipcMsg.getBuffer();
            (void)memcpy(buf, userInput.c_str(), userInput.size() + 1);
            ipcProducer.sendMessage(ipcMsg);

            std::cout << "done" << std::endl;
        }
    }

    return 0;
}
