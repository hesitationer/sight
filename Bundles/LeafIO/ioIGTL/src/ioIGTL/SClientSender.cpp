/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SClientSender.hpp"
#include "ioIGTL/helper/preferences.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Failed.hpp>

#include <functional>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioIGTL::SClientSender);

namespace ioIGTL
{

const ::fwCom::Slots::SlotKeyType SClientSender::s_START_SENDING_SLOT = "startSending";
const ::fwCom::Slots::SlotKeyType SClientSender::s_STOP_SENDING_SLOT  = "stopSending";

//-----------------------------------------------------------------------------

SClientSender::SClientSender() :
    m_deviceName("F4S")
{
    newSlot(s_START_SENDING_SLOT, &SClientSender::startSending, this);
    newSlot(s_STOP_SENDING_SLOT, &SClientSender::stopSending, this);
}

//-----------------------------------------------------------------------------

SClientSender::~SClientSender()
{
}

//-----------------------------------------------------------------------------

void SClientSender::configuring()
{
    SLM_ASSERT("Configuration not found", m_configuration != nullptr);

    std::vector < ::fwRuntime::ConfigurationElement::sptr > deviceNames = m_configuration->find("deviceName");
    if(!deviceNames.empty())
    {
        for(const auto& dn : deviceNames)
        {
            m_client.addAuthorizedDevice(dn->getValue());
        }
        m_client.setFilteringByDeviceName(true);
    }

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > inoutCfgs = m_configuration->find("inout");

    SLM_ASSERT("Missing 'in group=\"objects\"'", inoutCfgs[0]->getAttributeValue("group") == "objects");

    std::vector< ConfigurationType > objectsCfgs = inoutCfgs[0]->find("key");
    for(ConfigurationType cfg : objectsCfgs)
    {
        if (cfg->hasAttribute("name"))
        {
            std::string deviceName = cfg->getAttributeValue("name");
            m_deviceNames.push_back(deviceName);
            m_client.addAuthorizedDevice(deviceName);
        }
        else
        {
            m_deviceNames.push_back(m_deviceName);
        }
    }

    if (m_configuration->findConfigurationElement("server"))
    {
        const std::string serverInfo = m_configuration->findConfigurationElement("server")->getValue();
        SLM_INFO("OpenIGTLinkListener::configure server: " + serverInfo);
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw ::fwTools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientSender::runClient()
{
    // 1. Connection
    try
    {
        const std::uint16_t port   = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
        const std::string hostname = ::ioIGTL::helper::getPreferenceKey<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
        return;
    }
}

//-----------------------------------------------------------------------------

void SClientSender::starting()
{
}

//-----------------------------------------------------------------------------

void SClientSender::stopping()
{
    this->::ioNetwork::INetworkSender::stopping();
    this->stopSending();
}

//-----------------------------------------------------------------------------

void SClientSender::startSending()
{
    if(!m_client.isConnected())
    {
        m_clientFuture = std::async(std::launch::async, std::bind(&SClientSender::runClient, this));
    }
}

//-----------------------------------------------------------------------------

void SClientSender::stopSending()
{
    if(m_client.isConnected())
    {
        try
        {
            m_client.disconnect();
            m_clientFuture.wait();
        }
        catch (::fwCore::Exception& e)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Error", e.what());
        }
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const ::fwData::Object::sptr& obj)
{
    if (m_client.isConnected())
    {
        if (!m_deviceName.empty())
        {
            m_client.setDeviceNameOut(m_deviceName);
        }
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const ::fwData::Object::sptr& obj, const size_t index)
{
    if (m_client.isConnected())
    {
        if (!m_deviceNames[index].empty())
        {
            m_client.setDeviceNameOut(m_deviceNames[index]);
        }
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL
