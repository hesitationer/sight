/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <OGRE/OgreMovableObject.h>

#include <string>
#include <vector>

namespace arData
{
class Camera;
}

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor from f4s Camera to Ogre Camera.
 *
 * @section Slots Slots
 * - \b updateTF3D(): Called when the Ogre transform matrix has been updated and updates the transform service
 * accordingly.
 * - \b calibrate(): Applies calibration information to Ogre camera.
 *
 * @section XML XML Configuration
 * @code{.xml}
 *  <service uid="cameraAdaptor" type="::visuOgreAdaptor::SCamera">
 *      <inout key="transform" uid="..." />
 *      <in key="calibration" uid="..." />
 *      <config layer="..." />
 *  </service>
 * @endcode
 * @subsection Configuration Configuration
 * - \b layer (mandatory): defines the camera's layer
 * @subsection Input Input
 * - \b transform [::fwData::TransformationMatrix3D]: transform matrix for the camera.
 * - \b calibration [::arData::Camera]: camera containing calibration information.
 */
class VISUOGREADAPTOR_CLASS_API SCamera : public ::fwRenderOgre::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro((SCamera)(::fwRenderOgre::IAdaptor))

    /// Constructor.
    VISUOGREADAPTOR_API SCamera() noexcept;

    /// Destructor. Does nothing.
    VISUOGREADAPTOR_API virtual ~SCamera() noexcept;

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT;
    /** @} */

    /// Updates Transformation Matrix.
    VISUOGREADAPTOR_API virtual void updateTF3D();

    /// Returns proposals to connect service slots to associated object signals.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Associated ogre camera getter.
    VISUOGREADAPTOR_API virtual ::Ogre::Camera* getCamera();

    /// Near clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setNearClipDistance(::Ogre::Real _nearClipDistance);

    /// Far clipping plane position setter.
    VISUOGREADAPTOR_API virtual void setFarClipDistance(::Ogre::Real _farClipDistance);

    /// Aspect ratio setter.
    VISUOGREADAPTOR_API virtual void setAspectRatio(::Ogre::Real _ratio);

protected:
    /// Configures the service.
    VISUOGREADAPTOR_API void configuring() override;

    /// Starting method. Does nothing.
    VISUOGREADAPTOR_API void starting() override;

    /// Stopping method.
    VISUOGREADAPTOR_API void stopping() override;

    /// Updating method. Does nothing.
    VISUOGREADAPTOR_API void updating() override;

private:

    /// Calibrate the camera parameters according to an arData::Camera.
    void calibrate();

    /// Ogre camera managed by this adaptor.
    ::Ogre::Camera* m_camera;

    /// Aspect ratio for the frustum viewport.
    ::Ogre::Real m_aspectRatio;

    /// camera used to calibrate ogre camera.
    CSPTR(::arData::Camera) m_calibration;

    /// Connection with the layer
    ::fwCom::helper::SigSlotConnection m_layerConnection;
};

//------------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::Camera* SCamera::getCamera()
{
    return m_camera;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
