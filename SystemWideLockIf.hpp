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

} // namespace kiss

#endif // SYSTEMWIDELOCKIF_H
