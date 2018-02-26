/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/MeshPickerInteractor.hpp"

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

#include <QtCore/QEvent>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::MeshPickerInteractor );

namespace fwRenderOgre
{

namespace interactor
{

MeshPickerInteractor::MeshPickerInteractor() noexcept :
    m_control(false)
{
}

//------------------------------------------------------------------------------

MeshPickerInteractor::~MeshPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::resizeEvent(int x, int y)
{
    m_width  = x;
    m_height = y;
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonPressEvent(MouseButton button, int x, int y)
{
    if(m_picker->hasSceneManager())
    {
        if(m_control && m_picker->executeRaySceneQuery(x, y, m_width, m_height, m_queryFlags))
        {
            ::Ogre::Vector3 click = m_picker->getIntersectionInWorldSpace();

            ::fwData::Point::sptr point                = fwData::Point::New();
            ::fwData::Point::PointCoordArrayType cords =
            {{static_cast<double>(click.x), static_cast<double>(click.y), static_cast<double>(click.z)}};
            point->setCoord(cords);

            if(button == Qt::LeftButton)
            {
                m_sigAddPoint->asyncEmit(::fwData::Object::dynamicCast(point));
            }
            else
            {
                m_sigRemovePoint->asyncEmit(::fwData::Object::dynamicCast(point));
            }
        }
    }
    else
    {
        SLM_WARN("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonReleaseEvent(MouseButton, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::keyPressEvent(int k)
{
    if(k == Qt::Key_Control)
    {
        m_control = true;
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::keyReleaseEvent(int)
{
    m_control = false;
}

//------------------------------------------------------------------------------

} //namespace interactor
} //namespace fwRenderOgre
