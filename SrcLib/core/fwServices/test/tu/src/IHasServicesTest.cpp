/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "IHasServicesTest.hpp"

#include <fwServices/IHasServices.hpp>

#include <fwData/Boolean.hpp>

#include <TestService.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::IHasServicesTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void IHasServicesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IHasServicesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

static const std::string srvType = "::fwServices::ut::TestSrvAutoconnect";

struct TestIHasServices : public ::fwServices::IHasServices
{
    //------------------------------------------------------------------------------

    void testConnection()
    {
        ::fwData::Boolean::sptr data1 = ::fwData::Boolean::New();
        ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();
        ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();

        auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        {
            auto testService = this->registerService< ::fwServices::ut::TestSrvAutoconnect >(srvType);
            testService->registerInOut(data1, "data1", true);
            testService->registerInOut(data2, "data2", false);
            testService->registerInOut(data3, "data3", true);
            testService->start();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());
            sig2->emit();
            CPPUNIT_ASSERT(!testService->getReceived());
            sig3->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        {
            auto testService = this->registerService< ::fwServices::ut::TestSrvAutoconnect >(srvType);
            testService->registerInOut(data1, "data1", true);
            testService->registerInOut(data2, "data2", true);
            testService->registerInOut(data3, "data3");
            testService->start();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());
            sig3->emit();
            CPPUNIT_ASSERT(!testService->getReceived());
            sig2->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        // The destructor of ::fwServices::IHasServices would assert if unregister is not done properly
        // So if the test passes, that means we are ok with the unregistering
        this->unregisterServices();
    }

    //------------------------------------------------------------------------------

    void testRegistration()
    {
        ::fwData::Boolean::sptr data1 = ::fwData::Boolean::New();

        {
            ::fwServices::IService::wptr refService1;
            {
                auto testService1 = this->registerService(srvType);
                testService1->registerInOut(data1, "data1", true);
                testService1->start();
                refService1 = testService1;
            }

            ::fwServices::IService::wptr refService2;
            {
                auto testService2 = this->registerService(srvType);
                testService2->start();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            // The destructor of ::fwServices::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(refService1.lock()->getID());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterService(refService2.lock()->getID());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            ::fwServices::IService::wptr refService1;
            {
                auto testService1 = this->registerService(srvType);
                testService1->registerInOut(data1, "data1", true);
                testService1->start();
                refService1 = testService1;
            }

            ::fwServices::IService::wptr refService2;
            {
                auto testService2 = this->registerService(srvType);
                testService2->start();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            // The destructor of ::fwServices::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(refService1.lock());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterService(refService2.lock());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            ::fwServices::IService::wptr refService1;
            {
                auto testService1 = this->registerService(srvType);
                testService1->registerInOut(data1, "data1", true);
                testService1->start();
                refService1 = testService1;
            }

            ::fwServices::IService::wptr refService2;
            {
                auto testService2 = this->registerService("::fwServices::ut::TestServiceImplementation");
                testService2->start();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            // The destructor of ::fwServices::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("::fwServices::ut::TestSrvAutoconnect");
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterServices("::fwServices::ut::TestServiceImplementation");
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            auto testService1 = this->registerService< ::fwServices::ut::TestSrvAutoconnect >(srvType);
            testService1->registerInOut(data1, "data1", true);
            testService1->start();

            auto testService2 = this->registerService< ::fwServices::ut::TestSrvAutoconnect >(srvType);
            testService2->start();

            auto testService3 = this->registerService< ::fwServices::ut::TestSrvAutoconnect >(srvType);
            testService3->registerInOut(data1, "data1", true);
            testService3->start();

            // The destructor of ::fwServices::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("::fwServices::ut::TestSrvAutoconnect");
        }
    }
};

//------------------------------------------------------------------------------

void IHasServicesTest::testRegistration()
{
    TestIHasServices testHelper;
    testHelper.testRegistration();
}

//------------------------------------------------------------------------------

void IHasServicesTest::testConnection()
{
    TestIHasServices testHelper;
    testHelper.testConnection();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
