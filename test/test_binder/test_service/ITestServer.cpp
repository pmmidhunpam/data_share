#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Timers.h>

#include <binder/Parcel.h>
#include <binder/IInterface.h>

#include "ITestServer.h"
using namespace android;
enum {
    FUN = IBinder::FIRST_CALL_TRANSACTION,
};

class BpTestServer : public BpInterface<ITestServer>
{
public:
    BpTestServer(const sp<IBinder>& impl)
        : BpInterface<ITestServer>(impl)
    {
	printf("BpTestServer constructor\n");
    }

    virtual void fun(void) {
	printf("BpTestServer fun()\n");
        Parcel data, reply;
        data.writeInterfaceToken(ITestServer::getInterfaceDescriptor());
        remote()->transact(FUN, data, &reply);
    }
};

IMPLEMENT_META_INTERFACE(TestServer, "android.test.TestServer");

status_t BnTestServer::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch(code) {
        case FUN: {
            CHECK_INTERFACE(ITestServer, data, reply);
            fun();
	    return NO_ERROR;
        } break;
    }
    return BBinder::onTransact(code, data, reply, flags);
}
