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
    printf("test server is starting .................................................\n");
    TestService *p = TestService::instantiate();
    printf("p = %d\n", p);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}

