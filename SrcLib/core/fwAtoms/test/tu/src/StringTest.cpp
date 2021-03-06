/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "StringTest.hpp"

#include <fwAtoms/String.hpp>

#include <limits>



CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::StringTest );

namespace fwAtoms
{
namespace ut
{

void StringTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void StringTest::tearDown()
{
    // Clean up after the test run.
}


void StringTest::conversion()
{
    const std::string VALUES[] = {"", "azerty"};
    fwAtoms::String::sptr metaString;

    for ( std::string str: VALUES )
    {
        metaString = ::fwAtoms::String::New(str);

        CPPUNIT_ASSERT_EQUAL(str, metaString->getString());
    }
}

}  // namespace ut
}  // namespace fwAtoms

