/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/evtloop.h>

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwWX/convert.hpp>
#include <fwWX/wxMainFrame.hpp>

#include "fwGuiWx/layoutManager/FrameLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IFrameLayoutManager,
        ::fwGui::FrameLayoutManager,
         ::fwGui::layoutManager::IFrameLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );


namespace fwGui
{

//-----------------------------------------------------------------------------

const std::map< ::fwGui::layoutManager::IFrameLayoutManager::Style, long> FrameLayoutManager::FWSTYLE_TO_WXSTYLE =
        ::boost::assign::map_list_of(::fwGui::layoutManager::IFrameLayoutManager::DEFAULT,wxDEFAULT_FRAME_STYLE)
                                    (::fwGui::layoutManager::IFrameLayoutManager::STAY_ON_TOP, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);

//-----------------------------------------------------------------------------

FrameLayoutManager::FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

FrameLayoutManager::~FrameLayoutManager()
{}

//-----------------------------------------------------------------------------

void FrameLayoutManager::createFrame()
{
    SLM_TRACE_FUNC();
    FrameInfo frameInfo = this->getFrameInfo();

    wxEventLoopBase* eventLoop = wxEventLoop::GetActive();
    if (!eventLoop)
    {
        wxEventLoop::SetActive(new wxEventLoop() );
    }
    // wxWidget initialization
    wxInitAllImageHandlers();

    if(!wxTheApp->GetTopWindow())
    {
        m_wxFrame = new ::fwWX::wxMainFrame(wxTheApp->GetTopWindow(),
                wxNewId(),
                ::fwWX::std2wx(frameInfo.m_name),
                 wxDefaultPosition,
                 wxDefaultSize,
                 FWSTYLE_TO_WXSTYLE.find(frameInfo.m_style)->second
        );
        wxTheApp->SetTopWindow( m_wxFrame ) ;
    }
    else
    {
        m_wxFrame = new wxFrame(wxTheApp->GetTopWindow(),
                wxNewId(),
                ::fwWX::std2wx(frameInfo.m_name),
                 wxDefaultPosition,
                 wxSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second),
                 FWSTYLE_TO_WXSTYLE.find(frameInfo.m_style)->second
        );
    }

    m_wxFrame->SetMinSize(wxSize(frameInfo.m_minSize.first, frameInfo.m_minSize.second));

    if(!frameInfo.m_iconPath.empty())
    {
        wxIcon icon( ::fwWX::std2wx(frameInfo.m_iconPath.native_file_string()), wxBITMAP_TYPE_ICO );
        OSLM_ASSERT("Sorry, unable to create an icon instance from " << frameInfo.m_iconPath.native_file_string(), icon.Ok());
        m_wxFrame->SetIcon( icon );
    }
    m_wxFrame->Bind( wxEVT_CLOSE_WINDOW, &FrameLayoutManager::onCloseFrame, this,  m_wxFrame->GetId());
    m_wxFrame->Show();
    m_wxFrame->Refresh();

    ::fwGuiWx::container::WxContainer::NewSptr frameContainer;
    frameContainer->setWxContainer(m_wxFrame);
    m_frame = frameContainer;
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::destroyFrame()
{
    m_wxFrame->Show(false);
    m_wxFrame->Unbind( wxEVT_CLOSE_WINDOW, &FrameLayoutManager::onCloseFrame, this,  m_wxFrame->GetId());

    if ( wxTheApp->GetTopWindow() != m_wxFrame )
    {
        m_wxFrame->Reparent(NULL);
    }

    m_frame->destroyContainer();

    m_frame.reset();
}

//-----------------------------------------------------------------------------

void FrameLayoutManager::onCloseFrame(wxCloseEvent& event)
{
    SLM_TRACE_FUNC();
    this->m_closeCallback();
}

//-----------------------------------------------------------------------------

} // namespace fwGui


