/*
 *
 * 定义ITestServer服务接口。
 * 定义BnTestServer服务实现类。TestService会继承了该类实现具体的接口方法。
 * 在cpp文件中定义了BpTestServer服务代理类，供binder客户端使用。
 *
 * */
#ifndef _ITESTSERVER_H_
#define _ITESTSERVER_H_

#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/RefBase.h>

#include <binder/IInterface.h>

using namespace android;

//服务接口
class ITestServer : public IInterface
{
public:
    //这个宏声明了descriptor成员，asInterface方法。
    DECLARE_META_INTERFACE(TestServer);

    virtual void fun(void) = 0;
};

//服务实现类。TestService会继承了该类实现具体的接口方法。
class BnTestServer : public BnInterface<ITestServer>
{
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};
#endif
