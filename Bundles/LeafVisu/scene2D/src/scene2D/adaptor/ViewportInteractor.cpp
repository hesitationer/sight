/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/ViewportInteractor.hpp"

#include <fwData/Composite.hpp>

#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::ViewportInteractor,  ::fwData::Composite );

namespace scene2D
{
namespace adaptor
{

//-----------------------------------------------------------------------------

ViewportInteractor::ViewportInteractor() noexcept :
    m_viewportIsTranslated(false)
{
}

//-----------------------------------------------------------------------------

ViewportInteractor::~ViewportInteractor() noexcept
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::configuring()
{
    this->IAdaptor::configuring();
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doStart()
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doStop()
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doUpdate()
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doSwap()
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::processInteraction( ::fwRenderQt::data::Event& _event )
{
    if ( _event.getType() == ::fwRenderQt::data::Event::MouseWheelUp
         && _event.getModifier() == ::fwRenderQt::data::Event::ShiftModifier )
    {
        this->zoom(true);
    }
    else if ( _event.getType() == ::fwRenderQt::data::Event::MouseWheelDown
              && _event.getModifier() == ::fwRenderQt::data::Event::ShiftModifier )
    {
        this->zoom(false);
    }
    else if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress
              && _event.getButton() == ::fwRenderQt::data::Event::LeftButton
              && _event.getModifier() == ::fwRenderQt::data::Event::ShiftModifier )
    {
        m_viewportIsTranslated = true;
        m_lastCoordEvent       = _event.getCoord();
    }
    else if ( m_viewportIsTranslated )
    {
        if ( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
        {
            ::fwRenderQt::data::Coord coord                  = _event.getCoord();
            ::fwRenderQt::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

            float dx     = coord.getX() - m_lastCoordEvent.getX();
            float xTrans = dx * sceneViewport->getWidth() / (float) this->getScene2DRender()->getView()->width();

            float dy     = coord.getY() - m_lastCoordEvent.getY();
            float yTrans = dy * sceneViewport->getHeight() / (float) this->getScene2DRender()->getView()->height();

            sceneViewport->setX( sceneViewport->getX() - xTrans );
            sceneViewport->setY( sceneViewport->getY() - yTrans );
            this->getScene2DRender()->getView()->updateFromViewport();

            m_lastCoordEvent = coord;
        }
        else if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
        {
            m_viewportIsTranslated = false;
        }
    }
}

//-----------------------------------------------------------------------------

void ViewportInteractor::zoom( bool zoomIn )
{
    ::fwRenderQt::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

    float zoomPercent = 10.f / 100.0f;
    float y           = sceneViewport->getY();
    float x           = sceneViewport->getX();
    float width       = sceneViewport->getWidth();
    float height      = sceneViewport->getHeight();
    float centerX     = x + width/2.0f;
    float centerY     = y + height/2.0f;

    float newWidth;
    float newHeight;
    if ( zoomIn )
    {
        newWidth  = width * zoomPercent;
        newHeight = height * zoomPercent;
    }
    else
    {
        newWidth  = -width * zoomPercent;
        newHeight = -height * zoomPercent;
    }

    newWidth  += width;
    newHeight += height;

    x = centerX - newWidth / 2;
    y = centerY - newHeight / 2;

    width  = newWidth;
    height = newHeight;

    // Set viewport
    sceneViewport->setX( x );
    sceneViewport->setY( y );
    sceneViewport->setWidth( width );
    sceneViewport->setHeight( height );
    this->getScene2DRender()->getView()->updateFromViewport();
}

//-----------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D

