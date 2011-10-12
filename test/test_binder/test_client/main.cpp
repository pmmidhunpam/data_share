#include <sys/types.h>
#include <unistd.h>
#include <grp.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>

#include "TestService.h"

using namespace android;

int main(int argc, char** argv)
{
    printf("test client is starting .................................................\n");

    sp<ITestServer> testServer;
    const String16 name("testservice");
    int n;
    while ((n = getService(name, &testServer)) != NO_ERROR) {
	printf("getService error ret = %d\n", n);
        usleep(250000);
    }
    printf("ok let's invoke fun()\n");
    testServer->fun();
    return 0;
}

