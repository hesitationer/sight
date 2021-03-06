/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlProtocol/MessageFactory.hpp"

#include "igtlProtocol/RawMessage.hpp"

#include <igtl/igtlImageMessage.h>
#include <igtl/igtlPointMessage.h>
#include <igtl/igtlPolyDataMessage.h>
#include <igtl/igtlPositionMessage.h>
#include <igtl/igtlStringMessage.h>
#include <igtl/igtlTrackingDataMessage.h>
#include <igtl/igtlTransformMessage.h>

namespace igtlProtocol
{

MessageFactory::CreatorContainer MessageFactory::s_creators = MessageFactory::initFactory();

//------------------------------------------------------------------------------

MessageFactory::CreatorContainer MessageFactory::initFactory()
{
    MessageFactory::CreatorContainer container;

    container["TRANSFORM"] = &MessageMaker< ::igtl::TransformMessage, false >::createMessage;
    container["IMAGE"]     = &MessageMaker< ::igtl::ImageMessage, false >::createMessage;
    container["POINT"]     = &MessageMaker< ::igtl::PointMessage, false >::createMessage;
    container["STRING"]    = &MessageMaker< ::igtl::StringMessage, false >::createMessage;
    container["POSITION"]  = &MessageMaker< ::igtl::PositionMessage, false >::createMessage;
    container["POLYDATA"]  = &MessageMaker< ::igtl::PolyDataMessage, false>::createMessage;
    container["TDATA"]     = &MessageMaker< ::igtl::TrackingDataMessage, false>::createMessage;
    container["STT_TDATA"] = &MessageMaker< ::igtl::StartTrackingDataMessage, false>::createMessage;
    container["STP_TDATA"] = &MessageMaker< ::igtl::StopTrackingDataMessage, false>::createMessage;
    container["ATOMS"]     = std::bind(&MessageMaker< RawMessage, true >::createMessage, "ATOMS");
    container["INTEGER"]   = std::bind(&MessageMaker< RawMessage, true >::createMessage, "INTEGER");
    container["FLOAT"]     = std::bind(&MessageMaker< RawMessage, true >::createMessage, "FLOAT");
    return container;
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer MessageFactory::create(const std::string& type)
{
    CreatorContainer::const_iterator it;

    if ((it = MessageFactory::s_creators.find(type)) != MessageFactory::s_creators.end())
    {
        return it->second();
    }
    throw ::igtlProtocol::exception::Conversion("Cannot find a creator for type " + type);
}

}//namespace igtlProtocol
