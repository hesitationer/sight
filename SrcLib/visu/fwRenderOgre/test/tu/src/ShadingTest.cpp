/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ShadingTest.hpp"

#include <fwTest/helper/compare.hpp>
#include <fwTest/Data.hpp>

#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRenderOgre::helper::ut::ShadingTest );

namespace fwRenderOgre
{
namespace helper
{
namespace ut
{

//------------------------------------------------------------------------------

void ShadingTest::setUp()
{
    m_ogreRoot = Utils::getOgreRoot();

    auto& mgr = ::Ogre::MaterialManager::getSingleton();
    m_material = mgr.create("TestMaterial", ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    m_material->createTechnique();
}

//------------------------------------------------------------------------------

void ShadingTest::tearDown()
{
    m_ogreRoot = nullptr;
    Utils::destroyOgreRoot();
}

//------------------------------------------------------------------------------

void ShadingTest::isColorTechnique()
{
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isColorTechnique(*tech));

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isColorTechnique(*tech));

}

//------------------------------------------------------------------------------

void ShadingTest::isPeelTechnique()
{
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isPeelTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isPeelTechnique(*tech));
}

//------------------------------------------------------------------------------

void ShadingTest::isGeometricTechnique()
{
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isGeometricTechnique(*tech));
}
//------------------------------------------------------------------------------

void ShadingTest::isDepthOnlyTechnique()
{
    ::Ogre::Technique* tech = m_material->getTechnique(0);
    SLM_ASSERT("Technique is not set", tech);

    tech->setName("");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("depth");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("CelShadingDepthPeeling/depthMap");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/backDepth");
    CPPUNIT_ASSERT_EQUAL(true, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/occlusionMap");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/transmittanceBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("WeightedBlended/weightBlend");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));

    tech->setName("DepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("Celfalse, ShadingDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("DualDepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
    tech->setName("HybridTransparency/peelInit");
    CPPUNIT_ASSERT_EQUAL(false, Shading::isDepthOnlyTechnique(*tech));
}

//------------------------------------------------------------------------------

void ShadingTest::getProgramSuffix()
{
    std::string suffix;

    suffix = Shading::getProgramSuffix(::fwData::Material::AMBIENT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Ambient+VT"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::FLAT, false, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::FLAT, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Flat+VT"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::GOURAUD, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("Gouraud+DfsTex"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::GOURAUD, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("Gouraud+VT+DfsTex"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::PHONG, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT+DfsTex"), suffix);

    suffix = Shading::getProgramSuffix(::fwData::Material::PHONG, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("PixelLit+VT"), suffix);

}

//------------------------------------------------------------------------------

void ShadingTest::getR2VBGeometryProgramName()
{
    std::string prgName;

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TRIANGLE, true, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TRIANGLE, false, true, false);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Triangles+VT_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TETRA, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::TETRA, true, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Tetra+DfsTex+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::QUAD, false, true, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+VT+PPColor_GP"), prgName);

    prgName = Shading::getR2VBGeometryProgramName(::fwData::Mesh::QUAD, false, false, true);
    CPPUNIT_ASSERT_EQUAL(std::string("R2VB/Quad+PPColor_GP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::replaceProgramSuffix()
{
    std::string prgName;

    prgName = Shading::replaceProgramSuffix("Default/PixelLit+VT+DfsTex_VP", "Gouraud+DfsTex");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/Gouraud+DfsTex_VP"), prgName);

    prgName = Shading::replaceProgramSuffix("CelShadingDepthPeeling/peel_PixelLit+VT+DfsTex_FP", "Flat+VT");
    CPPUNIT_ASSERT_EQUAL(std::string("CelShadingDepthPeeling/peel_Flat+VT_FP"), prgName);

    prgName = Shading::replaceProgramSuffix("HybridTransparency/peel_Ambient+VT_FP", "Flat");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel_Flat_FP"), prgName);
}

//------------------------------------------------------------------------------

void ShadingTest::replaceProgramPrefix()
{
    std::string prgName;

    prgName = Shading::replaceProgramPrefix("Default/PixelLit_FP", "DepthPeeling/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("DepthPeeling/peel/PixelLit_FP"), prgName);

    prgName = Shading::replaceProgramPrefix("DepthPeeling/PeelInit/PixelLit_FP", "Default");
    CPPUNIT_ASSERT_EQUAL(std::string("Default/PixelLit_FP"), prgName);

    prgName = Shading::replaceProgramPrefix("Default/Flat+VT_FP", "HybridTransparency/peel");
    CPPUNIT_ASSERT_EQUAL(std::string("HybridTransparency/peel/Flat+VT_FP"), prgName);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace helper
} //namespace fwRenderOgre
