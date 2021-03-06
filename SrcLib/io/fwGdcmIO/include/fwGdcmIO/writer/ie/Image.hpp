/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <fwData/Image.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Image Information Entity class
 */
class FWGDCMIO_CLASS_API Image : public ::fwGdcmIO::writer::ie::InformationEntity< ::fwData::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] image Image data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Image(const SPTR(::gdcm::Writer)& writer,
                       const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                       const ::fwData::Image::csptr& image,
                       const ::fwLog::Logger::sptr& logger = nullptr,
                       ProgressCallback progress           = nullptr,
                       CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Image();

    /**
     * @brief Write General Image Module tags
     * @see PS 3.3 C.7.6.1
     */
    FWGDCMIO_API virtual void writeGeneralImageModule();

    /**
     * @brief Write General Image Module specific tags
     * @see PS 3.3 C.7.6.1
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeGeneralImageModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     */
    FWGDCMIO_API virtual void writeImagePlaneModule();

    /**
     * @brief Write Image Plane Module tags
     * @see PS 3.3 C.7.6.2
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeImagePlaneModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write Image Pixel Module tags
     * @see PS 3.3 C.7.6.3
     */
    FWGDCMIO_API virtual void writeImagePixelModule();

    /**
     * @brief Write Image Pixel Module specific tags
     * @see PS 3.3 C.7.6.3
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeImagePixelModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write VOI LUT Module tags
     * @see PS 3.3 C.11.2
     */
    FWGDCMIO_API virtual void writeVOILUTModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    FWGDCMIO_API virtual void writeSOPCommonModule();

    /**
     * @brief Write SOP Common Module specific tags
     * @see PS 3.3 C.12.1
     * @param[in] instanceNumber Instance number
     */
    FWGDCMIO_API virtual void writeSOPCommonModuleSpecificTags(unsigned int instanceNumber);

    /**
     * @brief Write CT Image Module tags
     * @see PS 3.3 C.8.2.1
     */
    FWGDCMIO_API virtual void writeCTImageModule();

    /**
     * @brief Write MR Image Module tags
     * @see PS 3.3 C.8.3.1
     */
    FWGDCMIO_API virtual void writeMRImageModule();

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
