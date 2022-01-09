#include "SimpleIpc.hpp"
#include "SystemWideLockIf.hpp"

#include <atomic>
#include <iostream>
#include <string>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <cstring>

// TODO: add message produced callback (to signal other process/other core)

class DummySystemWideLock : public kiss::SystemWideLockIf
{
  public:
    static constexpr uint32_t UNLOCKED = 0xB0B0B0B0U;
    static constexpr uint32_t LOCKED = 0xFAFAFAFAU;

    DummySystemWideLock()
    {
        static kiss::SharedMem lockMem("/shm_lock", sizeof(std::atomic_uint32_t));
        lockAddr = new (lockMem.getAddress()) std::atomic_uint32_t;
        lockAddr->store(UNLOCKED);
    }

    virtual void lock()
    {
        bool exchangeDone = false;
        do
        {
            uint32_t exp = UNLOCKED;
            exchangeDone = lockAddr->compare_exchange_strong(exp, LOCKED);
        } while (!exchangeDone);
    }

    virtual void unlock()
    {
        bool exchangeDone = false;
        do
        {
            uint32_t exp = LOCKED;
            exchangeDone = lockAddr->compare_exchange_strong(exp, UNLOCKED);
        } while (!exchangeDone);
    }

  private:
    std::atomic_uint32_t *lockAddr;
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
