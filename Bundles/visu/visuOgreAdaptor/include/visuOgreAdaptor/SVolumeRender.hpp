/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/TransferFunction.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>
#include <fwRenderOgre/vr/ImportanceDrivenVolumeRenderer.hpp>
#include <fwRenderOgre/vr/PreIntegrationTable.hpp>
#include <fwRenderOgre/vr/SATVolumeIllumination.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgrePolygon.h>
#include <OGRE/OgreTexture.h>

#include <array>
#include <vector>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor for volume rendering.
 *
 * @section Slots Slots
 * - \b newImage(): Called when a new image is loaded.
 * - \b updateSampling(int): Called when the sampling is changed and updates the volume renderer accordingly.
 * - \b updateOpacityCorrection(int): Called when the opacity correction factor is changed and updates the volume
 * renderer accordingly.
 * - \b updateAOFactor(double) : Called when the ambient occlusion factor is changed and computes the SAT.
 * - \b updateColorBleedingFactor(double) : Called when the color bleeding factor is changed and computes the SAT.
 * - \b updateSatSizeRatio(int) : Called when the SAT ratio is changed and computes it again with the new corresponding
 *      size.
 * - \b updateSatShellsNumber(int) : Called when the number of SAT shells is changed and compute the SAT.
 * - \b updateSatShellRadius(int) : Called when the SAT shell radius is changed and computes the SAT.
 * - \b updateSatConeAngle(int) : Called when the SAT cone angle is changed and computes the SAT.
 * - \b updateSatConeSamples(int) : Called when the SAT cone samples number is changed and computes the SAT.
 * - \b togglePreintegration(bool): Toggle pre-integration.
 * - \b toggleAmbientOcclusion(bool): Toggle ambient occlusion.
 * - \b toggleColorBleeding(bool): Toggle color bleeding.
 * - \b toggleShadows(bool): Toggle soft shadows.
 * - \b toggleWidgets(bool): Toggles widget visibility.
 * - \b resizeViewport(int, int): Called when the size of the viewport changes.
 * - \b setFocalDistance(int): Called to modify focal length (only useful for stereoscopic 3D).
 * - \b setStereoMode(int): Called to modify 3D stereoscopic 3D mode.
 * - \b setBoolParameter(bool, string): Calls a bool parameter slot according to the given key.
 * - \b setIntParameter(int, string): Calls an int parameter slot according to the given key.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 * - \b setEnumParameter(string, string): Calls a double parameter slot according to the given key.
 * - \b setColorParameter(array<uint8_t, 4>, string): Calls a color parameter slot according to the given key.
 * - \b updateVisibility(bool): show or hide the volume.
 * - \b updateClippingBox(): updates the cropping widget from the clipping matrix.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVolumeRender" >
        <inout key="image" uid="..." autoConnect="yes" />
        <inout key="tf" uid="..." optional="yes" />
        <inout key="clippingMatrix" uid="..." />
        <config layer="default"
                samples="1024" preintegration="yes" ao="no" colorBleeding="no" shadows="no"
                satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                aoFactor="0.5" colorBleedingFactor="0.5" autoresetcamera="yes"/>
    </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: input volume data.
 * @subsection In-Out In-Out
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 * - \b mask [::fwData::Image] (optional): segmented data.
 * - \b clippingMatrix [::fwData::TransformationMatrix3D]: matrix used to clip the volume.
 * @subsection Configuration Configuration
 * - \b layer (mandatory): id of the layer where this adaptor applies.
 * - \b samples (optional, default=512): maximum number of samples per ray or number of slices.
 * - \b preintegration (optional, yes/no, default=no): use pre-integration.
 * - \b widgets (optional, yes/no, default=yes): display VR widgets.
 * - \b ao (optional, true/false, default=false): Ambient occlusion usage.
 * - \b colorBleeding (optional, true/false, default=false): Color bleeding usage.
 * - \b shadows (optional, true/false, default=false): Soft shadows usage.
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 *      associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination from the
 *      SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 * - \b aoFactor (optional, double, default=1.0): factor used to weight the ambient occlusion.
 * - \b colorBleedingFactor (optional, double, default=1.0): factor used to weight the color bleeding.
 * - \b autoresetcamera (optional, yes/no, default=yes): reset the camera at image update to view the whole volume.
 */
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro( (SVolumeRender)(::fwRenderOgre::IAdaptor) )

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEW_IMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEW_MASK_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAMPLING_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_AO_FACTOR_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_COLOR_BLEEDING_FACTOR_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SIZE_RATIO_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SHELLS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_SHELL_RADIUS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_CONE_ANGLE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SAT_CONE_SAMPLES_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_PREINTEGRATION_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_AMBIENT_OCCLUSION_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_COLOR_BLEEDING_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_SHADOWS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_WIDGETS_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_RESIZE_VIEWPORT_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_FOCAL_DISTANCE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_STEREO_MODE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CLIPPING_BOX_SLOT;
    /** @} */

    /// Volume rendering effects.
    typedef enum
    {
        VR_AMBIENT_OCCLUSION,
        VR_COLOR_BLEEDING,
        VR_SHADOWS
    } VREffectType;

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() noexcept;

protected:

    /// Starts the service initializes scene objects.
    VISUOGREADAPTOR_API virtual void starting() override;

    /// Cleans up memory.
    VISUOGREADAPTOR_API virtual void stopping() override;

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void updating() override;

    /// Configures the service
    VISUOGREADAPTOR_API virtual void configuring() override;

    /// Retrieves the current transfer function
    VISUOGREADAPTOR_API void swapping(const KeyType& key) override;

    /// Slot: update the displayed transfer function
    VISUOGREADAPTOR_API virtual void updateTF();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connects fwData::Image::s_MODIFIED_SIG to this::s_NEWIMAGE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    void newImage();
    void newMask();
    void updateImage();
    void updateSampling(int nbSamples);
    void updateOpacityCorrection(int opacityCorrection);
    void updateAOFactor(double aoFactor);
    void updateColorBleedingFactor(double colorBleedingFactor);
    void updateSatSizeRatio(int sizeRatio);
    void updateSatShellsNumber(int shellsNumber);
    void updateSatShellRadius(int shellRadius);
    void updateSatConeAngle(int coneAngle);
    void updateSatConeSamples(int nbConeSamples);
    void togglePreintegration(bool preintegration);
    void toggleAmbientOcclusion(bool ambientOcclusion);
    void toggleColorBleeding(bool colorBleeding);
    void toggleShadows(bool shadows);
    void toggleWidgets(bool visible);
    void resizeViewport(int w, int h);
    void setFocalDistance(int focalDistance);
    void setStereoMode(::fwRenderOgre::Layer::StereoModeType mode);
    void setBoolParameter(bool val, std::string key);
    void setIntParameter(int val, std::string key);
    void setDoubleParameter(double val, std::string key);
    void setEnumParameter(std::string val, std::string key);
    void setColorParameter(std::array<uint8_t, 4> color, std::string key);
    void setImageSpacing();

    /// Slot: Sets the volume to be visible or not.
    void updateVisibility(bool visibility);

    /// Creates widgets and connects its slots to interactor signals.
    void createWidget();

    /// Removes the widgets from the interactor and deletes it.
    void destroyWidget();

    /// Computes the volume illumination and applies it to the ray tracing renderer
    void updateVolumeIllumination();

    /// Updates or creates the illumination volume according to the given VR effect.
    void toggleVREffect(VREffectType vrEffect);

    /// Updates the clipping box position from the inout clipping matrix.
    void updateClippingBox();
    ::fwDataTools::helper::TransferFunction m_helperTF;

    /// Updates the inout clipping matrix from the clipping box positions.
    void updateClippingTM3D();

    /// Renders the volume.
    ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* m_volumeRenderer { nullptr };

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    ::Ogre::TexturePtr m_maskTexture;

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction::sptr m_gpuTF;

    /// Pre-integration table.
    ::fwRenderOgre::vr::PreIntegrationTable m_preIntegrationTable;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// This object's scene node.
    ::Ogre::SceneNode* m_volumeSceneNode { nullptr };

    /// Camera used for rendering.
    ::Ogre::Camera* m_camera { nullptr };

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widget;

    /// Sampling rate.
    std::uint16_t m_nbSamples { 512 };

    /// Use pre-integration.
    bool m_preIntegratedRendering { false };

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion { false };

    /// Sets usage of color bleeding.
    bool m_colorBleeding { false };

    /// Sets usage of soft shadows.
    bool m_shadows { false };

    /// Toggles widget visibility.
    bool m_widgetVisibilty { true };

    /// Illumination volume used to render shadows and ambient occlusion.
    std::shared_ptr< ::fwRenderOgre::vr::SATVolumeIllumination> m_illum;

    /// Ratio used to determine the size of the SAT regarding of the associated image size.
    float m_satSizeRatio {0.25f };

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_satShells { 4 };

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_satShellRadius { 4 };

    /// Angle used to define the soft shadows cones.
    float m_satConeAngle { 0.1f };

    /// Number of samples along the soft shadows cones.
    int m_satConeSamples { 50 };

    /// Factor parameter used to weight the ambient occlusion.
    double m_aoFactor { 1. };

    /// Factor parameter used to weight the color bleeding.
    double m_colorBleedingFactor { 1. };

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { true };

    /// Default IDVR method
    std::string m_IDVRMethod { "None" };

    /// Handle connections between the layer and the volume renderer.
    ::fwCom::helper::SigSlotConnection m_volumeConnection;
};

} // visuOgreAdaptor
