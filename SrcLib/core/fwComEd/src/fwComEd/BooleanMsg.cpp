/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwComEd/BooleanMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::BooleanMsg, ::fwComEd::BooleanMsg );

namespace fwComEd
{

//-----------------------------------------------------------------------------

std::string BooleanMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";

//-----------------------------------------------------------------------------

BooleanMsg::BooleanMsg() throw()
{}

//-----------------------------------------------------------------------------

BooleanMsg::~BooleanMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd
