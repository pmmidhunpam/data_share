#ifndef _ITESTSERVER_H_
#define _ITESTSERVER_H_

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/RefBase.h>

#include <binder/IInterface.h>

using namespace android;

class ITestServer : public IInterface
{
public:
  DECLARE_META_INTERFACE(TestServer);

  virtual void fun(void) = 0;
};

class BnTestServer : public BnInterface<ITestServer>
{
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};
#endif
