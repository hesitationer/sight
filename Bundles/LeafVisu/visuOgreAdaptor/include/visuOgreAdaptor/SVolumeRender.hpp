/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/TransferFunction.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>
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
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="volumeRender" type="::visuOgreAdaptor::SVolumeRender">
        <in key="image" uid="imageUID" autoConnect="yes" />
        <in key="mask" uid="maskUID" autoConnect="yes" />
        <inout key="TF" uid="TFSelections" />
        <config renderer="default"
                 preintegration="yes" mode="slice" ao="no" colorBleeding="no" shadows="no"
                 satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                 aoFactor="0.5" colorBleedingFactor="0.5"
                 selectedTFKey="SelectedTF" />
    </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: input volume data.
 * - \b mask [::fwData::Image]: segmented data.
 * @subsection In-Out In-Out:
 * - \b TF [::fwData::Composite]: Selected transfer function.
 * With :
 * - \b renderer (optional): defines the renderer displaying the volume.
 * - \b preintegration (optional, yes/no, default=no): use pre-integration.
 * - \b widgets (optional, yes/no, default=yes): display VR widgets.
 * - \b mode (optional, slice/raycasting, default=raycasting): Rendering mode.
 * - \b selectedTFKey (mandatory): TF key.
 * - \b tfSelectionFwID (mandatory): TF selection.
 * Only if the raycasting render mode is activated :
 * - \b ao (optional, yes/no, default=no): Ambient occlusion usage.
 * - \b colorBleeding (optional, yes/no, default=no): Color bleeding usage.
 * - \b shadows (optional, yes/no, default=no): Soft shadows usage.
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 *      associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination from the
 *      SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 * - \b aoFactor (optional, double, default=1.0): factor used to weight the ambient occlusion.
 * - \b colorBleedingFactor (optional, double, default=1.0): factor used to weight the color bleeding.
 */
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable,
                                                public ::fwDataTools::helper::MedicalImageAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro( (SVolumeRender)(::fwRenderOgre::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEW_IMAGE_SLOT;
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
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_MODE3D_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() throw();

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    /// Starts the service initializes scene objects.
    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );

    /// Cleans up memory.
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

    /// Configures the service
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    /// Slot called on TF update.
    VISUOGREADAPTOR_API virtual void updatingTFPoints();

    /// Slot called on TF window update.
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connects fwData::Image::s_MODIFIED_SIG to this::s_NEWIMAGE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

private:

    void newImage();
    void updateSampling(int nbSamples);
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

    /// Creates widgets and connects its slots to interactor signals.
    void initWidgets();

    /// Computes the volume illumination and applies it to the ray tracing renderer
    void updateVolumeIllumination();

    /// Rendering mode.
    enum
    {
        VR_MODE_SLICE,
        VR_MODE_RAY_TRACING
    } m_renderingMode;

    /// Renders the volume.
    ::fwRenderOgre::vr::IVolumeRenderer* m_volumeRenderer;

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction m_gpuTF;

    /// Pre-integration table.
    ::fwRenderOgre::vr::PreIntegrationTable m_preIntegrationTable;

    /// This object's scene manager.
    ::Ogre::SceneManager* m_sceneManager;

    /// This object's scene node.
    ::Ogre::SceneNode* m_volumeSceneNode;

    /// Camera used for rendering.
    ::Ogre::Camera* m_camera;

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widgets;

    /// Sampling rate.
    uint16_t m_nbSlices;

    /// Use pre-integration.
    bool m_preIntegratedRendering;

    /// Sets usage of ambient occlusion.
    bool m_ambientOcclusion;

    /// Sets usage of color bleeding.
    bool m_colorBleeding;

    /// Sets usage of soft shadows.
    bool m_shadows;

    /// Toggles widget visibility.
    bool m_widgetVisibilty;

    /// Illumination volume used to render shadows and ambient occlusion.
    ::fwRenderOgre::vr::SATVolumeIllumination* m_illum;

    /// Ratio used to determine the size of the SAT regarding of the associated image size.
    float m_satSizeRatio;

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_satShells;

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_satShellRadius;

    /// Angle used to define the soft shadows cones.
    float m_satConeAngle;

    /// Number of samples along the soft shadows cones.
    int m_satConeSamples;

    /// Factor parameter used to weight the ambient occlusion.
    double m_aoFactor;

    /// Factor parameter used to weight the color bleeding.
    double m_colorBleedingFactor;

    /// Handle connections between the layer and the volume renderer.
    ::fwCom::helper::SigSlotConnection m_volumeConnection;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
