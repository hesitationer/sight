/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/policy/AlwaysSplitPolicy.hpp"

namespace fwXML
{

bool AlwaysSplitPolicy::split(::boost::shared_ptr< ::fwTools::Object > obj)
{
    return true;
}

} //namespace fwXML