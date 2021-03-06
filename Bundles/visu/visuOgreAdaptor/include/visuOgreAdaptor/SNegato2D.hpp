/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to display a 2D negato.
 * *
 * @section Slots Slots
 * - \b newImage() : update the image display to show the new content.
 * - \b sliceType(int, int) : update image slice index .
 * - \b sliceIndex(int, int, int) : update image slice type.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SNegato2D">
            <inout key="image" uid="..." />
            <inout key="tf" uid="..." optional="yes" />
            <config layer="default" sliceIndex="axial" filtering="none" tfalpha="true" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): id of the layer where this adaptor applies.
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato
 * - \b tfalpha (optional, true/false, default=false): if true, the alpha channel of the transfer function is used
 */
class VISUOGREADAPTOR_CLASS_API SNegato2D : public ::fwRenderOgre::IAdaptor
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    fwCoreServiceClassDefinitionsMacro( (SNegato2D)(::fwRenderOgre::IAdaptor) )

    /// Constructor.
    VISUOGREADAPTOR_API SNegato2D() noexcept;
    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SNegato2D() noexcept;
    /// Sets the filtering type
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

protected:

    /// Configures the service
    VISUOGREADAPTOR_API virtual void configuring() override;
    /// Instanciates the texture, material, pass and texture unit state
    /// Sets the connection between attached data and the receive slot
    VISUOGREADAPTOR_API virtual void starting() override;
    /// Disconnects the attached data from the receive slot
    VISUOGREADAPTOR_API virtual void stopping() override;
    /// Requests a rendering of the scene.
    VISUOGREADAPTOR_API virtual void updating() override;
    /// Select the current tf
    VISUOGREADAPTOR_API void swapping(const KeyType& key) override;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Slot: update the displayed transfer function
    VISUOGREADAPTOR_API virtual void updateTF();

private:

    /// Slot: update image buffer
    void newImage();

    /// Slot: update image slice type
    void changeSliceType(int _from, int _to);

    /// Slot: update image slice index
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /// updates image slice index for the current fragment program.
    void updateShaderSliceIndexParameter();

    /// Makes the plane processes his mesh
    void createPlane(const ::fwData::Image::SpacingType& _spacing);

    /// Adapts the camera to the width and height of the displayed plane.
    void updateCameraWindowBounds();

    /// Ogre texture which will be displayed on the negato
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// The plane on which we will apply our texture
    ::fwRenderOgre::Plane* m_plane;

    bool m_enableAlpha {false};

    /// The scene node allowing to move the entire negato
    ::Ogre::SceneNode* m_negatoSceneNode;

    /// Defines the filtering type for this negato
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering;

    /// Stores the current slice index for each axis.
    std::vector<float> m_currentSliceIndex;

    /// Connections.
    ::fwCom::Connection m_connection;

    /// Image orientation
    OrientationMode m_orientation;

    ::fwDataTools::helper::TransferFunction m_helperTF;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato2D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------
} // visuOgreAdaptor
