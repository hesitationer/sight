/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ContainOneSkinTest.hpp"

#include <fwActivities/IValidator.hpp>
#include <fwActivities/IObjectValidator.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ModelSeries.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::validators::ut::ContainOneSkinTest );

namespace validators
{
namespace ut
{

//------------------------------------------------------------------------------

void ContainOneSkinTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ContainOneSkinTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ContainOneSkinTest::testValidator()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::validators::ModelSeries::ContainOneSkin");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
    ::fwData::Reconstruction::sptr rec1        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec2        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec3        = ::fwData::Reconstruction::New();
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst;


    {
        validation = objValidator->validate(rec1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Validator on ohter data should NOT be valid", false, validation.first);
    }
    {
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty model series should NOT be valid", false, validation.first);
    }
    {
        vecReconst.push_back(rec1);
        modelSeries->setReconstructionDB(vecReconst);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with one non-Skin reconstruction should NOT be valid",
                                     false, validation.first);
    }
    {
        rec1->setStructureType("Skin");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with one Skin reconstruction should be valid",
                                     true, validation.first);
    }
    {
        vecReconst.push_back(rec2);
        modelSeries->setReconstructionDB(vecReconst);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with two reconstructions (only one Skin) should be valid",
                                     true, validation.first);
    }
    {
        vecReconst.push_back(rec3);
        modelSeries->setReconstructionDB(vecReconst);
        validation = objValidator->validate(modelSeries);
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with three reconstructions (only one Skin) should be valid",
                                     true, validation.first);
    }
    {
        rec2->setStructureType("Skin");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with three reconstructions (two Skin) should NOT be valid",
                                     false, validation.first);
    }
    {
        rec1->setStructureType("Bones");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with three reconstructions (only one Skin) should be valid",
                                     true, validation.first);
    }
    {
        rec2->setStructureType("Tumor");
        validation = objValidator->validate(modelSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("ModelSeries with three reconstructions (no Skin) should NOT be valid",
                                     false, validation.first);
    }
}


//------------------------------------------------------------------------------

void ContainOneSkinTest::testValidatorWithVector()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::validators::ModelSeries::ContainOneSkin");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Vector::sptr vector = ::fwData::Vector::New();

    ::fwMedData::ModelSeries::sptr modelSeries1 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries3 = ::fwMedData::ModelSeries::New();
    ::fwData::Reconstruction::sptr rec11        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec12        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec21        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec22        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec31        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec32        = ::fwData::Reconstruction::New();
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst1;
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst2;
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst3;

    vecReconst1.push_back(rec11);
    vecReconst1.push_back(rec12);
    vecReconst2.push_back(rec21);
    vecReconst2.push_back(rec22);
    vecReconst3.push_back(rec31);
    vecReconst3.push_back(rec32);

    modelSeries1->setReconstructionDB(vecReconst1);
    modelSeries2->setReconstructionDB(vecReconst2);
    modelSeries3->setReconstructionDB(vecReconst3);

    {
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector series should be valid", true, validation.first);
    }
    {
        vector->getContainer().push_back(modelSeries1);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with one ModelSeries (without Skin) shoud NOT be valid",
                                     false, validation.first);
    }
    {
        rec11->setStructureType("Skin");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with one ModelSeries (with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec12->setStructureType("Skin");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with one ModelSeries (with two Skin) shoud be NOT valid",
                                     false, validation.first);
    }
    {
        rec12->setStructureType("Bones");
        vector->getContainer().push_back(modelSeries2);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with two ModelSeries (one without Skin) shoud be NOT valid",
                                     false, validation.first);
    }
    {
        rec21->setStructureType("Skin");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with two ModelSeries (each with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec22->setStructureType("Skin");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with two ModelSeries (one with one Skin and one with two skin) shoud NOT "
                                     "be valid", false,
                                     validation.first);
    }
    {
        rec22->setStructureType("Bone");
        vector->getContainer().push_back(modelSeries3);
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with three ModelSeries (two with one Skin and one without skin) shoud NOT "
                                     "be valid", false, validation.first);
    }
    {
        rec32->setStructureType("Skin");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with three ModelSeries (each with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with three ModelSeries (two with one Skin and one without skin) shoud NOT "
                                     "be valid", false, validation.first);
    }
}

//------------------------------------------------------------------------------

void ContainOneSkinTest::testValidatorWithComposite()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::validators::ModelSeries::ContainOneSkin");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    ::fwMedData::ModelSeries::sptr modelSeries1 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries3 = ::fwMedData::ModelSeries::New();
    ::fwData::Reconstruction::sptr rec11        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec12        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec21        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec22        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec31        = ::fwData::Reconstruction::New();
    ::fwData::Reconstruction::sptr rec32        = ::fwData::Reconstruction::New();
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst1;
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst2;
    ::fwMedData::ModelSeries::ReconstructionVectorType vecReconst3;

    vecReconst1.push_back(rec11);
    vecReconst1.push_back(rec12);
    vecReconst2.push_back(rec21);
    vecReconst2.push_back(rec22);
    vecReconst3.push_back(rec31);
    vecReconst3.push_back(rec32);

    modelSeries1->setReconstructionDB(vecReconst1);
    modelSeries2->setReconstructionDB(vecReconst2);
    modelSeries3->setReconstructionDB(vecReconst3);

    {
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Composite series should be valid", true, validation.first);
    }
    {
        (*composite)["model1"] = modelSeries1;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Vector with one ModelSeries (without Skin) shoud NOT be valid",
                                     false, validation.first);
    }
    {
        rec11->setStructureType("Skin");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with one ModelSeries (with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec12->setStructureType("Skin");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with one ModelSeries (with two Skin) shoud be NOT valid",
                                     false, validation.first);
    }
    {
        rec12->setStructureType("Bones");
        (*composite)["model2"] = modelSeries2;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with two ModelSeries (one without Skin) shoud be NOT valid",
                                     false, validation.first);
    }
    {
        rec21->setStructureType("Skin");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with two ModelSeries (each with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec22->setStructureType("Skin");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with two ModelSeries (one with one Skin and one with two skin) shoud "
                                     " NOT be valid", false, validation.first);
    }
    {
        rec22->setStructureType("Bone");
        (*composite)["model3"] = modelSeries3;
        validation             = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with three ModelSeries (two with one Skin and one without skin) shoud "
                                     "NOT be valid", false, validation.first);
    }
    {
        rec32->setStructureType("Skin");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with three ModelSeries (each with one Skin) shoud be valid",
                                     true, validation.first);
    }
    {
        rec21->setStructureType("Tumor");
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Composite with three ModelSeries (two with one Skin and one without skin) shoud "
                                     "NOT be valid", false, validation.first);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace validators

