#ifndef _ITESTMANAGER_H_
#define _ITESTMANAGER_H_ 

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

class TestService : 
	public BnTestServer
{
public:
    static void instantiate();
    TestService();
    virtual void fun(void);
};

#endif
