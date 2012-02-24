/*
 * 
 * 定义了TestService继承BnTestServer，具体服务的实现类。
 * android一般框架都会用xxservice的类继承Bnxx的类。在xxservice中实现具体的服务。降低代码的耦合度。
 *
 * */

#ifndef _ITESTSERVICE_H_
#define _ITESTSERVICE_H_ 

#include <stdint.h>
#include <sys/types.h>

#include <utils/Vector.h>
#include <utils/SortedVector.h>
#include <utils/KeyedVector.h>
#include <utils/threads.h>
#include <utils/RefBase.h>

#include <binder/Permission.h>
#include <binder/IServiceManager.h>


#include "ITestServer.h"

class TestService : public BnTestServer
{
public:
    //android一般都用这个静态方法注册binder服务到servicemanager
    static TestService* instantiate();

    TestService();

    //实现接口服务的具体方法。
    virtual void fun(void);
};

#endif
