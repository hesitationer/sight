/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_UT_SHADINGTEST_HPP__
#define __FWRENDEROGRE_UT_SHADINGTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <OGRE/OgreMaterial.h>

namespace fwRenderOgre
{
namespace helper
{

namespace ut
{

class ShadingTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ShadingTest );
CPPUNIT_TEST( isColorTechnique );
CPPUNIT_TEST( isPeelTechnique );
CPPUNIT_TEST( isGeometricTechnique );
CPPUNIT_TEST( isDepthOnlyTechnique );
CPPUNIT_TEST( getProgramSuffix );
CPPUNIT_TEST( getR2VBGeometryProgramName );
CPPUNIT_TEST( replaceProgramSuffix );
CPPUNIT_TEST( replaceProgramPrefix );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void isColorTechnique();
    void isPeelTechnique();
    void isGeometricTechnique();
    void isDepthOnlyTechnique();
    void getProgramSuffix();
    void getR2VBGeometryProgramName();
    void replaceProgramSuffix();
    void replaceProgramPrefix();

private:
    /// Ogre root necessary to setup up everything properly
    ::Ogre::Root* m_ogreRoot;
    /// Dummy material created at runtime
    ::Ogre::MaterialPtr m_material;
};

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre

#endif // __FWRENDEROGRE_UT_SHADINGTEST_HPP__
