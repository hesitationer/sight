/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Point.hpp>

#include <fwCamp/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Point))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("coord", &::fwData::Point::m_vCoord);
}
