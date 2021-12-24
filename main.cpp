#include "SimpleIpc.hpp"

#include <iostream>
#include <string>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

int main(int argc, char **)
{
    kiss::SharedMem mem(128);

    if (argc <= 1)
    {
        printf("CONSUMER\n");
        kiss::SimpleIpc ipcConsumer(mem);

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
        kiss::SimpleIpc ipcProducer(mem);

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

int mainOld()
{
    // TODO: sender-receiver process

    kiss::SharedMem mem(128);
    kiss::SimpleIpc ipcProducer(mem);

    const std::vector<std::string> testStrVec{
        "test str one", "test str two", "test str three", "test str four", "test str five",
    };

    for (auto &it : testStrVec)
    {
        auto &ipcMsg = ipcProducer.messageBufferFetch(it.size() + 1);

        auto *buffer = ipcMsg.getBuffer();
        (void)memcpy(buffer, it.c_str(), it.size() + 1);

        ipcProducer.sendMessage(ipcMsg);
    }

    kiss::SimpleIpc ipcConsumer(mem);
    for (const auto &it : testStrVec)
    {
        (void)it;

        kiss::IpcMessage *msg = ipcConsumer.receiveStart();

        printf("Message received: %s\n", msg->getBuffer());

        ipcConsumer.receiveEnd(msg);
    }

    // ----------------------

    for (uint32_t i = 0; i < 20; ++i)
    {
        std::string testStr{"987654321"}; // 9 chrs + nullbyte
        const uint32_t strSize = testStr.size() + 1;

        {
            auto &ipcMsg = ipcProducer.messageBufferFetch(strSize);

            auto *buffer = ipcMsg.getBuffer();
            (void)memcpy(buffer, testStr.c_str(), strSize);

            ipcProducer.sendMessage(ipcMsg);
            printf("[%d] Message sent: %s\n", i, ipcMsg.getBuffer());
        }
        {
            auto &ipcMsg = ipcProducer.messageBufferFetch(strSize);

            auto *buffer = ipcMsg.getBuffer();
            (void)memcpy(buffer, testStr.c_str(), strSize);

            ipcProducer.sendMessage(ipcMsg);
            printf("[%d] Message sent: %s\n", i, ipcMsg.getBuffer());
        }
        // -------------

        kiss::IpcMessage *msg = ipcConsumer.receiveStart();

        if (msg == nullptr)
            printf("[%d] Message received: nullptr\n", i);
        else
            printf("[%d] Message received: %s\n", i, msg->getBuffer());

        ipcConsumer.receiveEnd(msg);
    }

    return 0;
}
