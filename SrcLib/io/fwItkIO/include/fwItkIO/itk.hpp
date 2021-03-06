/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWITKIO_ITK_HPP__
#define __FWITKIO_ITK_HPP__


#include <fwData/Image.hpp>

#include "fwItkIO/config.hpp"

namespace fwItkIO
{

/**
 * @brief Allocate a new data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so ::fwData::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * ::fwData::Image does not manage the image buffer.
 */
template< class ITKIMAGE>
::fwData::Image::sptr dataImageFactory( typename ITKIMAGE::Pointer itkImage, bool bufferManagerIsDataImage = true );

/**
 * @brief Update a data Image from an itk one.
 *
 * If bufferManagerIsDataImage = true, itkImage releases buffer management
 * and so ::fwData::Image manages it (in this case, itkImage must be the
 * owner of this buffer). if bufferManagerIsDataImage = false, the created
 * ::fwData::Image does not manage the image buffer.
 */
template< class ITKIMAGE>
void dataImageFactory( typename ITKIMAGE::Pointer itkImage, ::fwData::Image::sptr _dataImage,
                       bool bufferManagerIsDataImage = true );

/**
 * @brief Create an ITK image from an ::fwData::Image.
 *
 * If bufferManagerIsDataImage = true, imageData conserve his buffer management
 * and so the created itkImage not manages it. If bufferManagerIsDataImage = false,
 * the created itkImage manage his image buffer and thus imageData releases his
 * buffer (in this case, imageData must be the owner of this buffer).
 *
 * @pre an assertion check if ITKIMAGE::PixelType correspond to imageData->getPixelType
 * @pre an assertion check if ITKIMAGE dimension correspond to imageData dimension
 */
template< class ITKIMAGE>
typename ITKIMAGE::Pointer itkImageFactory( ::fwData::Image::csptr imageData, bool bufferManagerIsDataImage = true );

}

#include "fwItkIO/itk.hxx"

#endif // __FWITKIO_ITK_HPP__
