#ifndef SYSTEMWIDELOCKIF_H
#define SYSTEMWIDELOCKIF_H

namespace kiss
{

class SystemWideLockIf
{
  public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

// Can be used when std::lock_guard/unique_lock/... not availabe
struct LockScoped
{
  public:
    LockScoped() = delete;
    LockScoped(const LockScoped &) = delete;
    LockScoped(LockScoped &&) = delete;
    LockScoped &operator=(const LockScoped &) = delete;
    LockScoped &operator=(LockScoped &&) = delete;

    LockScoped(SystemWideLockIf &lock) : mLock(lock)
    {
        mLock.lock();
    }

    ~LockScoped()
    {
        mLock.unlock();
    }

  private:
    SystemWideLockIf &mLock;
};

} // namespace kiss

#endif // SYSTEMWIDELOCKIF_H
