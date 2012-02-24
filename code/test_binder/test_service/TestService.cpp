#include "TestService.h"

TestService::TestService() {
    printf("TestService constructor\n");
}

//具体接口的服务实现方法。这个方法在哪个线程执行由binder驱动来管理。
void TestService::fun(void) {
    printf("(%d/%d) TestService fun()\n", getpid(), gettid());
    sleep(20);
}

TestService*
TestService::instantiate() {
    TestService* p = new TestService();
    int n = defaultServiceManager()->addService(String16("testservice"), p);
    printf("TestService::instantiate() addService ret = %d\n", n);
    return p;
}
