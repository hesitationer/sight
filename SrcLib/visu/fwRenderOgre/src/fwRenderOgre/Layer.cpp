/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Layer.hpp"

#include "fwRenderOgre/compositor/Core.hpp"
#include "fwRenderOgre/helper/Camera.hpp"
#include "fwRenderOgre/ILight.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwDataTools/Color.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <fwThread/Worker.hpp>

#include <boost/tokenizer.hpp>

#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRectangle2D.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

#include <stack>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Layer::s_INIT_LAYER_SIG           = "layerInitialized";
const ::fwCom::Signals::SignalKeyType Layer::s_RESIZE_LAYER_SIG         = "layerResized";
const ::fwCom::Signals::SignalKeyType Layer::s_STEREO_MODE_CHANGED_SIG  = "StereoModeChanged";
const ::fwCom::Signals::SignalKeyType Layer::s_CAMERA_UPDATED_SIG       = "CameraUpdated";
const ::fwCom::Signals::SignalKeyType Layer::s_CAMERA_RANGE_UPDATED_SIG = "CameraRangeUpdated";

const ::fwCom::Slots::SlotKeyType Layer::s_INTERACTION_SLOT    = "interaction";
const ::fwCom::Slots::SlotKeyType Layer::s_RESET_CAMERA_SLOT   = "resetCamera";
const ::fwCom::Slots::SlotKeyType Layer::s_USE_CELSHADING_SLOT = "useCelShading";

//-----------------------------------------------------------------------------

const std::string Layer::DEFAULT_CAMERA_NAME        = "DefaultCam";
const std::string Layer::DEFAULT_LIGHT_NAME         = "DefaultLight";
const std::string Layer::s_DEFAULT_CAMERA_NODE_NAME = "CameraNode";

//-----------------------------------------------------------------------------

static const std::map<Layer::StereoModeType, std::string> s_stereoCompositorMap = {
    { Layer::StereoModeType::AUTOSTEREO_5, "AutoStereo5" },
    { Layer::StereoModeType::AUTOSTEREO_8, "AutoStereo8" },
    { Layer::StereoModeType::STEREO, "Stereo" },
    { Layer::StereoModeType::NONE, "" }
};

//-----------------------------------------------------------------------------

struct Layer::LayerCameraListener : public ::Ogre::Camera::Listener
{
    Layer* m_layer;
    int m_frameId;
    ::fwRenderOgre::interactor::IMovementInteractor::sptr m_interactor;

    //------------------------------------------------------------------------------

    LayerCameraListener(Layer* renderer, ::fwRenderOgre::interactor::IMovementInteractor::sptr interactor) :
        m_layer(renderer),
        m_frameId(0),
        m_interactor(interactor)
    {
    }

    //------------------------------------------------------------------------------

    virtual void cameraPreRenderScene(::Ogre::Camera* _camera)
    {
        SLM_ASSERT("Layer is not set", m_layer );

        if(m_layer->getStereoMode() != Layer::StereoModeType::NONE)
        {
            // Set the focal length using the point of interest of the interactor
            // This works well for the trackball but this would need to be adjusted for an another interactor type
            // For a FPS camera style for instance, we would fix the focal length once and for all according
            // to the scale of the world
            float focalLength = std::max(0.001f, std::abs(m_interactor->getLookAtZ()));
            _camera->setFocalLength(focalLength);

            const int frameId = m_layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frameId != m_frameId)
            {
                auto& gpuProgramMgr = ::Ogre::GpuProgramManager::getSingleton();

                for(std::uint8_t i = 0; i < m_layer->getNumberOfCameras(); ++i)
                {
                    ::Ogre::Matrix4 projMat = m_layer->getCameraProjMat(i);

                    projMat[1][0] = -projMat[1][0];
                    projMat[1][1] = -projMat[1][1];
                    projMat[1][2] = -projMat[1][2];
                    projMat[1][3] = -projMat[1][3];

                    const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();
                    {
                        const std::string projParamName = "ProjectionMatrixParam/"+std::to_string(i);
                        auto it                         = sharedParameterMap.find(projParamName);
                        if(it != sharedParameterMap.end())
                        {
                            it->second->setNamedConstant("u_proj", projMat);
                        }
                    }
                    {
                        const std::string projParamName = "InverseProjectionMatrixParam/"+std::to_string(i);
                        auto it                         = sharedParameterMap.find(projParamName);
                        if(it != sharedParameterMap.end())
                        {
                            it->second->setNamedConstant("u_invProj", projMat.inverse());
                        }
                    }
                }

                m_frameId = frameId;
            }
        }
    }

    //------------------------------------------------------------------------------
};

Layer::Layer() :
    m_sceneManager(nullptr),
    m_renderTarget(nullptr),
    m_viewport(nullptr),
    m_stereoMode(StereoModeType::NONE),
    m_rawCompositorChain(""),
    m_coreCompositor(nullptr),
    m_transparencyTechnique(::fwRenderOgre::compositor::DEFAULT),
    m_numPeels(8),
    m_depth(1),
    m_topColor("#333333"),
    m_bottomColor("#333333"),
    m_topScale(0.f),
    m_bottomScale(1.f),
    m_camera(nullptr),
    m_hasCoreCompositor(false),
    m_hasCompositorChain(false),
    m_sceneCreated(false),
    m_hasDefaultLight(true),
    m_cameraListener(nullptr)
{
    newSignal<InitLayerSignalType>(s_INIT_LAYER_SIG);
    newSignal<ResizeLayerSignalType>(s_RESIZE_LAYER_SIG);
    newSignal<StereoModeChangedSignalType>(s_STEREO_MODE_CHANGED_SIG);
    newSignal<CameraUpdatedSignalType>(s_CAMERA_UPDATED_SIG);
    newSignal<CameraUpdatedSignalType>(s_CAMERA_RANGE_UPDATED_SIG);

    newSlot(s_INTERACTION_SLOT, &Layer::interaction, this);
    newSlot(s_RESET_CAMERA_SLOT, &Layer::resetCameraCoordinates, this);
}

//-----------------------------------------------------------------------------

Layer::~Layer()
{
    if(m_camera && m_cameraListener)
    {
        m_camera->removeListener(m_cameraListener);
        delete m_cameraListener;
        m_cameraListener = nullptr;
    }

    if(m_autostereoListener)
    {
        ::Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    if(m_sceneManager)
    {
        ::fwRenderOgre::Utils::getOgreRoot()->destroySceneManager(m_sceneManager);
        m_sceneManager = nullptr;
    }
    m_camera = nullptr;
}

//-----------------------------------------------------------------------------

void Layer::setRenderTarget(::Ogre::RenderTarget* _renderTarget)
{
    m_renderTarget = _renderTarget;
}

//-----------------------------------------------------------------------------

void Layer::setID(const std::string& id)
{
    auto renderService = m_renderService.lock();
    SLM_ASSERT("Render service must be set before calling setID().", renderService);
    m_id = id;
}

//-----------------------------------------------------------------------------

const std::string Layer::getName() const
{
    return m_sceneManager->getName();
}

//-----------------------------------------------------------------------------

const std::string& Layer::getLayerID() const
{
    return m_id;
}

//-----------------------------------------------------------------------------

::Ogre::SceneManager* Layer::getSceneManager() const
{
    return m_sceneManager;
}

//-----------------------------------------------------------------------------

void Layer::createScene()
{
    namespace fwc = ::fwRenderOgre::compositor;

    auto root = ::fwRenderOgre::Utils::getOgreRoot();
    m_sceneManager = root->createSceneManager("DefaultSceneManager", m_renderService.lock()->getID() + "_" + m_id);
    m_sceneManager->addRenderQueueListener( ::fwRenderOgre::Utils::getOverlaySystem() );

    SLM_ASSERT("Scene manager must be initialized", m_sceneManager);
    SLM_ASSERT("Render window must be initialized", m_renderTarget);

    m_sceneManager->setAmbientLight(::Ogre::ColourValue(0.8f, 0.8f, 0.8f));

    // Create the camera
    m_camera = m_sceneManager->createCamera(Layer::DEFAULT_CAMERA_NAME);
    m_camera->setNearClipDistance(1);

    m_viewport = m_renderTarget->addViewport(m_camera, m_depth);

    m_compositorChainManager = fwc::ChainManager::uptr(new fwc::ChainManager(m_viewport));

    if (m_depth != 0)
    {
        m_viewport->setClearEveryFrame(true, ::Ogre::FBT_DEPTH);
    } // Set the background material
    else
    {
        // FIXME : background isn't shown when using compositor with a clear pass
        // We must blend the input previous in each compositor
        ::Ogre::MaterialPtr defaultMaterial = ::Ogre::MaterialManager::getSingleton().getByName("DefaultBackground");
        ::Ogre::MaterialPtr material        = ::Ogre::MaterialManager::getSingleton().create(
            this->getName() + "backgroundMat", ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        defaultMaterial.get()->copyDetailsTo(material);

        std::uint8_t color[4];
        ::Ogre::Pass* pass                           = material->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();

        ::fwDataTools::Color::hexaStringToRGBA(m_topColor, color);
        ::Ogre::ColourValue ogreTopColor(color[0] / 255.f,
                                         color[1] / 255.f,
                                         color[2] / 255.f,
                                         1.f);
        params->setNamedConstant("topColour", ogreTopColor);

        ::fwDataTools::Color::hexaStringToRGBA(m_bottomColor, color);
        ::Ogre::ColourValue ogreBotColor(color[0] / 255.f,
                                         color[1] / 255.f,
                                         color[2] / 255.f,
                                         1.f);
        params->setNamedConstant("bottomColour", ogreBotColor);

        params->setNamedConstant("topScale", m_topScale);
        params->setNamedConstant("bottomScale", m_bottomScale);

        // Create background rectangle covering the whole screen
        ::Ogre::Rectangle2D* rect = new ::Ogre::Rectangle2D();
        rect->setCorners(-1.0, 1.0, 1.0, -1.0);
        rect->setMaterial(material);

        // Render the background before everything else
        rect->setRenderQueueGroup(::Ogre::RENDER_QUEUE_BACKGROUND);

        // Use infinite AAB to always stay visible
        ::Ogre::AxisAlignedBox aabInf;
        aabInf.setInfinite();
        rect->setBoundingBox(aabInf);

        // Attach background to the scene
        ::Ogre::SceneNode* node = m_sceneManager->getRootSceneNode()->createChildSceneNode("Background");
        node->attachObject(rect);
    }

    // Alter the camera aspect ratio to match the viewport
    m_camera->setAspectRatio(Ogre::Real(m_viewport->getActualWidth()) / ::Ogre::Real(m_viewport->getActualHeight()));

    // Creating Camera Scene Node
    ::Ogre::SceneNode* cameraNode = m_sceneManager->getRootSceneNode()->createChildSceneNode(
        Layer::s_DEFAULT_CAMERA_NODE_NAME);
    cameraNode->setPosition(::Ogre::Vector3(0, 0, 5));
    cameraNode->lookAt(::Ogre::Vector3(0, 0, 1), ::Ogre::Node::TS_WORLD);

    cameraNode->attachObject(m_camera);

    if(m_hasDefaultLight)
    {
        m_defaultLightTransform     = ::fwData::TransformationMatrix3D::New();
        m_defaultLightDiffuseColor  = ::fwData::Color::New();
        m_defaultLightSpecularColor = ::fwData::Color::New();

        m_lightAdaptor = ::fwRenderOgre::ILight::createLightAdaptor(m_defaultLightTransform,
                                                                    m_defaultLightDiffuseColor,
                                                                    m_defaultLightSpecularColor);
        m_lightAdaptor->setName(Layer::DEFAULT_LIGHT_NAME);
        m_lightAdaptor->setType(::Ogre::Light::LT_DIRECTIONAL);
        m_lightAdaptor->setParentTransformName(cameraNode->getName());
        m_lightAdaptor->setLayerID(this->getLayerID());
        m_lightAdaptor->setRenderService(m_renderService.lock());
        m_lightAdaptor->start();
    }

    // If there is any interactor adaptor in xml, m_moveInteractor will be overwritten by InteractorStyle adaptor
    ::fwRenderOgre::interactor::IMovementInteractor::sptr interactor =
        ::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(
            ::fwRenderOgre::interactorFactory::New("::fwRenderOgre::interactor::TrackballInteractor",
                                                   m_sceneManager->getName()));

    this->setMoveInteractor(interactor);

    m_cameraListener = new LayerCameraListener(this, interactor);
    m_camera->addListener(m_cameraListener);

    // Setup transparency compositors
    if(m_hasCoreCompositor)
    {
        this->setupCore();
    }

    m_compositorChainManager->setOgreViewport(m_viewport);

    // Setup custom compositors and autostereo
    {
        ::boost::char_separator<char> sep(";");
        ::boost::tokenizer< ::boost::char_separator<char> > tok(m_rawCompositorChain, sep);
        std::vector< fwc::ChainManager::CompositorIdType> compositorChain;

        for(const auto& it : tok)
        {
            compositorChain.push_back(it);
        }
        if(m_stereoMode != StereoModeType::NONE)
        {
            compositorChain.push_back(s_stereoCompositorMap.at(m_stereoMode));

            m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(this->getNumberOfCameras());
            ::Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
        }

        m_compositorChainManager->setCompositorChain(compositorChain, m_id, m_renderService.lock());

        m_compositorChainManager->addAvailableCompositor("Stereo");
        m_compositorChainManager->addAvailableCompositor("AutoStereo5");
        m_compositorChainManager->addAvailableCompositor("AutoStereo8");
    }

    m_sceneCreated = true;

    this->signal<InitLayerSignalType>(s_INIT_LAYER_SIG)->asyncEmit(this->getSptr());
}

// ----------------------------------------------------------------------------

void Layer::destroyScene()
{
    if(m_lightAdaptor)
    {
        ::fwRenderOgre::ILight::destroyLightAdaptor(m_lightAdaptor);
        m_lightAdaptor.reset();
    }
}

// ----------------------------------------------------------------------------

void Layer::addAvailableCompositor(std::string compositorName)
{
    m_compositorChainManager->addAvailableCompositor(compositorName);
}

// ----------------------------------------------------------------------------

void Layer::updateCompositorState(std::string compositorName, bool isEnabled)
{
    auto renderService = m_renderService.lock();

    renderService->makeCurrent();
    m_compositorChainManager->updateCompositorState(compositorName, isEnabled, m_id, m_renderService.lock());
    renderService->requestRender();
}

// ----------------------------------------------------------------------------

void Layer::interaction(::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo info)
{
    this->getRenderService()->makeCurrent();

    switch(info.interactionType)
    {
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::MOUSEMOVE:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->mouseMoveEvent(info.button, info.x, info.y, info.dx, info.dy);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::WHEELMOVE:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->wheelEvent(info.delta, info.x, info.y);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::RESIZE:
        {
            auto sig = this->signal<ResizeLayerSignalType>(s_RESIZE_LAYER_SIG);
            sig->asyncEmit(info.x, info.y);

            for(auto& interactor : m_interactors)
            {
                interactor.lock()->resizeEvent(info.x, info.y);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::KEYPRESS:
        {

            for(auto& interactor : m_interactors)
            {
                interactor.lock()->keyPressEvent(info.key);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::KEYRELEASE:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->keyReleaseEvent(info.key);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::BUTTONRELEASE:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->buttonReleaseEvent(info.button, info.x, info.y);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::BUTTONPRESS:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->buttonPressEvent(info.button, info.x, info.y);
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::FOCUSIN:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->focusInEvent();
            }
            break;
        }
        case ::fwRenderOgre::IRenderWindowInteractorManager::InteractionInfo::FOCUSOUT:
        {
            for(auto& interactor : m_interactors)
            {
                interactor.lock()->focusOutEvent();
            }
            break;
        }
    }
    this->signal<CameraUpdatedSignalType>(s_CAMERA_UPDATED_SIG)->asyncEmit();
}

// ----------------------------------------------------------------------------

int Layer::getDepth() const
{
    return m_depth;
}

// ----------------------------------------------------------------------------

void Layer::setDepth(int depth)
{
    m_depth = depth;
}

// ----------------------------------------------------------------------------

void Layer::setWorker( const ::fwThread::Worker::sptr& _worker)
{
    ::fwCom::HasSlots::m_slots.setWorker(_worker);
}

// ----------------------------------------------------------------------------

::fwRenderOgre::SRender::sptr Layer::getRenderService() const
{
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

void Layer::setRenderService( const ::fwRenderOgre::SRender::sptr& _service)
{
    SLM_ASSERT("service not instanced", _service);

    m_renderService = _service;
}

// ----------------------------------------------------------------------------

void Layer::setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::sptr _interactor)
{
    m_connections.disconnect();

    bool alreadyExist = false;
    for(auto& interactor : m_interactors)
    {
        // This function is only kept to maintain compatibility with SInteractorStyle which will be removed soon
        // We return the first select interactor found
        auto moveInteractor = ::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(interactor.lock());
        if(moveInteractor)
        {
            interactor   = _interactor;
            alreadyExist = true;
            break;
        }
    }

    if(!alreadyExist)
    {
        m_interactors.push_back(_interactor);
    }

    m_moveInteractor = _interactor;
    m_moveInteractor->resizeEvent(static_cast<int>(m_renderTarget->getWidth()),
                                  static_cast<int>(m_renderTarget->getHeight()) );

    m_connections.connect(_interactor, ::fwRenderOgre::interactor::IMovementInteractor::s_RESET_CAMERA_SIG,
                          this->getSptr(), s_RESET_CAMERA_SLOT);

    m_connections.connect(_interactor, ::fwRenderOgre::interactor::IMovementInteractor::s_RENDER_REQUESTED_SIG,
                          this->getRenderService(), ::fwRenderOgre::SRender::s_REQUEST_RENDER_SLOT);

    if(m_cameraListener)
    {
        m_cameraListener->m_interactor = _interactor;
    }
}

// ----------------------------------------------------------------------------

void Layer::setSelectInteractor(::fwRenderOgre::interactor::IInteractor::sptr interactor)
{
    m_interactors.push_back(interactor);
}

// ----------------------------------------------------------------------------

::fwRenderOgre::interactor::IMovementInteractor::sptr Layer::getMoveInteractor()
{
    return m_moveInteractor;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::interactor::IPickerInteractor::sptr Layer::getSelectInteractor()
{
    for(auto& interactor : m_interactors)
    {
        // This function is only kept to maintain compatibility with SInteractorStyle which will be removed soon
        // We return the first select interactor found
        const auto selectInteractor = ::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(interactor.lock());
        if(selectInteractor)
        {
            return selectInteractor;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------

::Ogre::AxisAlignedBox Layer::computeWorldBoundingBox() const
{
    // The bounding box in which all the object's bounding boxes will be merged
    ::Ogre::AxisAlignedBox worldCoordBoundingBox;

    // Getting this render service scene manager
    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();

    // Needed to recompute world bounding boxes
    rootSceneNode->_update(true, false);

    // This stack will be used for iterate through the scene nodes graph
    ::std::stack< const ::Ogre::SceneNode* > childrenStack;
    childrenStack.push(rootSceneNode);

    while(!childrenStack.empty())
    {
        const ::Ogre::SceneNode* tempSceneNode = childrenStack.top();
        childrenStack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        ::Ogre::SceneNode::ConstObjectIterator entitiesIt = tempSceneNode->getAttachedObjectIterator();
        while(entitiesIt.hasMoreElements())
        {
            // First, we try to cast the MovableObject* into an Entity*
            const auto movable           = entitiesIt.getNext();
            const ::Ogre::Entity* entity = dynamic_cast< ::Ogre::Entity* > (movable);

            if(entity)
            {
                worldCoordBoundingBox.merge(entity->getWorldBoundingBox());
            }
            else
            {
                // Then try to cast into a ManualObject*
                const ::Ogre::ManualObject* manualObject = dynamic_cast< ::Ogre::ManualObject* > (movable);

                if(manualObject)
                {
                    worldCoordBoundingBox.merge(manualObject->getWorldBoundingBox());
                }
                else
                {
                    // Last try to cast into a Camera*
                    const ::Ogre::Camera* cameraObject = dynamic_cast< ::Ogre::Camera* > (movable);

                    if(cameraObject && cameraObject != this->getDefaultCamera() &&
                       cameraObject->isDebugDisplayEnabled())
                    {
                        worldCoordBoundingBox.merge(cameraObject->getWorldBoundingBox());
                    }
                }
            }
        }

        // Retrieves an iterator pointing to the child nodes of the current scene node
        ::Ogre::SceneNode::ConstChildNodeIterator childNodesIt = tempSceneNode->getChildIterator();
        while(childNodesIt.hasMoreElements())
        {
            // First, we must cast the Node* into a SceneNode*
            const ::Ogre::SceneNode* childNode = dynamic_cast< ::Ogre::SceneNode* >(childNodesIt.getNext());
            if(childNode)
            {
                // Push the current node into the stack in order to continue iteration
                childrenStack.push(childNode);
            }
        }
    }
    return worldCoordBoundingBox;
}

//------------------------------------------------------------------------------

compositor::transparencyTechnique Layer::getTransparencyTechnique()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyTechnique();
    }

    return compositor::DEFAULT;
}

//------------------------------------------------------------------------------

int Layer::getTransparencyDepth()
{
    if(m_coreCompositor)
    {
        return m_coreCompositor->getTransparencyDepth();
    }

    return 0;

}

//------------------------------------------------------------------------------

bool Layer::setTransparencyTechnique(compositor::transparencyTechnique _technique)
{
    bool success = false;
    if(m_coreCompositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->getRenderService()->makeCurrent();
        success = m_coreCompositor->setTransparencyTechnique(_technique);
        m_coreCompositor->update();
        this->requestRender();
    }

    return success;
}

//------------------------------------------------------------------------------

void Layer::setTransparencyDepth(int _depth)
{
    if(m_coreCompositor)
    {
        // Playing with the transparency may create new compositors, thus new resources in OpenGL, thus we
        // need to explicitly switch to our OpenGL context
        this->getRenderService()->makeCurrent();
        m_coreCompositor->setTransparencyDepth(_depth);
        m_coreCompositor->update();
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void Layer::resetCameraCoordinates() const
{
    const ::Ogre::AxisAlignedBox worldCoordBoundingBox = this->computeWorldBoundingBox();

    if(m_camera && m_camera->getProjectionType() == ::Ogre::PT_PERSPECTIVE)
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if( worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_NULL ||
            worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();
            camNode->setPosition(0.f, 0.f, 0.f);
        }
        else
        {
            // Arbitrary coefficient
            const ::Ogre::Real boundingBoxLength = worldCoordBoundingBox.getSize().length() > 0 ?
                                                   worldCoordBoundingBox.getSize().length() : 0;

            float coeffZoom = static_cast<float>(boundingBoxLength);
            OSLM_DEBUG("Zoom coefficient : " << coeffZoom);

            // Set the direction of the camera
            ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();
            const ::Ogre::Quaternion quat   = camNode->getOrientation();
            const ::Ogre::Vector3 direction = quat.zAxis();

            // Set the position of the camera
            camNode->setPosition((worldCoordBoundingBox.getCenter() + coeffZoom * direction ) );

            // Update interactor's mouse scale
            m_moveInteractor->setMouseScale( coeffZoom );

            resetCameraClippingRange(worldCoordBoundingBox);
        }

        m_renderService.lock()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void Layer::computeCameraParameters() const
{
    const ::Ogre::AxisAlignedBox worldCoordBoundingBox = this->computeWorldBoundingBox();

    if(m_camera && m_camera->getProjectionType() == ::Ogre::PT_PERSPECTIVE)
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if( worldCoordBoundingBox != ::Ogre::AxisAlignedBox::EXTENT_NULL &&
            worldCoordBoundingBox != ::Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();
            const ::Ogre::Quaternion quat   = camNode->getOrientation();
            const ::Ogre::Vector3 direction = quat.zAxis();
            const ::Ogre::Vector3 position  = camNode->getPosition();

            const ::Ogre::Vector3 div = (position - worldCoordBoundingBox.getCenter()) / direction;
            const float distance      = div.z;

            // Update interactor's mouse scale
            m_moveInteractor->setMouseScale( distance );

            resetCameraClippingRange(worldCoordBoundingBox);
        }
    }
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange() const
{
    resetCameraClippingRange(computeWorldBoundingBox());
}

//-----------------------------------------------------------------------------

void Layer::resetCameraClippingRange(const ::Ogre::AxisAlignedBox& worldCoordBoundingBox) const
{
    if(m_camera && m_camera->getProjectionType() == ::Ogre::PT_PERSPECTIVE)
    {
        // Check if bounding box is valid, otherwise, do nothing.
        if( worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_NULL ||
            worldCoordBoundingBox == ::Ogre::AxisAlignedBox::EXTENT_INFINITE)
        {
            return;
        }

        ::Ogre::SceneNode* camNode = m_camera->getParentSceneNode();

        // Set the direction of the camera
        ::Ogre::Quaternion quat   = camNode->getOrientation();
        ::Ogre::Vector3 direction = quat.zAxis();
        ::Ogre::Vector3 position  = camNode->getPosition();

        // Set near and far plan
        ::Ogre::Vector3 minimum = worldCoordBoundingBox.getMinimum();
        ::Ogre::Vector3 maximum = worldCoordBoundingBox.getMaximum();

        float a, b, c, d;
        a = -direction.x;
        b = -direction.y;
        c = -direction.z;
        d = -(a*position.x + b*position.y + c*position.z);

        // Max near and min far
        float maxNear = a*minimum.x + b*minimum.y + c*minimum.z + d;
        float minFar  = 1e-18f;
        float dist;

        float corners[6];
        corners[0] = minimum.x;
        corners[1] = maximum.x;
        corners[2] = minimum.y;
        corners[3] = maximum.y;
        corners[4] = minimum.z;
        corners[5] = maximum.z;
        // Find the closest / farthest bounding box vertex
        for (int k = 0; k < 2; k++ )
        {
            for (int j = 0; j < 2; j++ )
            {
                for (int i = 0; i < 2; i++ )
                {
                    dist    = a*corners[i] + b*corners[2+j] + c*corners[4+k] + d;
                    maxNear = (dist < maxNear) ? dist : maxNear;
                    minFar  = (dist > minFar) ? dist : minFar;
                }
            }
        }

        // Give ourselves a little breathing room
        maxNear = 0.99f*maxNear - (minFar - maxNear)*0.5f;
        minFar  = 1.01f*minFar + (minFar - maxNear)*0.5f;

        // Do not let the range behind the camera throw off the calculation.
        if (maxNear < 0.1f)
        {
            maxNear = 0.1f;
        }

        // Make sure near is not bigger than far
        maxNear = (maxNear >= minFar) ? (0.01f*minFar) : (maxNear);

        const auto& chain          = this->getCompositorChain();
        const auto saoCompositorIt = std::find_if(chain.begin(), chain.end(),
                                                  ::fwRenderOgre::compositor::ChainManager::FindCompositorByName("SAO"));

        const auto prevNear = m_camera->getNearClipDistance();
        const auto prevFar  = m_camera->getFarClipDistance();
        if(saoCompositorIt != chain.end() && saoCompositorIt->second)
        {
            // Near and far for SAO
            OSLM_TRACE("Near SAO");
            maxNear = 1;
            minFar  = 10000;
        }
        m_camera->setNearClipDistance( maxNear );
        m_camera->setFarClipDistance( minFar );

        if(maxNear != prevNear || minFar != prevFar)
        {
            this->signal<CameraUpdatedSignalType>(s_CAMERA_RANGE_UPDATED_SIG)->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void Layer::requestRender()
{
    m_renderService.lock()->requestRender();
}

//-----------------------------------------------------------------------------

void Layer::setStereoMode(StereoModeType mode)
{
    const std::string oldCompositorName = s_stereoCompositorMap.at(m_stereoMode);

    // Disable the old compositor
    if(m_stereoMode != StereoModeType::NONE && m_compositorChainManager)
    {
        m_compositorChainManager->updateCompositorState(oldCompositorName, false, m_id, m_renderService.lock());

        ::Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    // Enable the new one
    m_stereoMode = mode;

    const std::string compositorName = s_stereoCompositorMap.at(m_stereoMode);

    if(m_stereoMode != StereoModeType::NONE && m_compositorChainManager)
    {
        m_compositorChainManager->updateCompositorState(compositorName, true, m_id, m_renderService.lock());

        m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(this->getNumberOfCameras());
        ::Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
    }

    auto sig = this->signal<StereoModeChangedSignalType>(s_STEREO_MODE_CHANGED_SIG);
    sig->asyncEmit(m_stereoMode);
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundColor(std::string topColor, std::string botColor)
{
    m_topColor    = topColor;
    m_bottomColor = botColor;
}

//-----------------------------------------------------------------------------

void Layer::setBackgroundScale(float topScale, float botScale)
{
    m_topScale    = topScale;
    m_bottomScale = botScale;
}

//-------------------------------------------------------------------------------------

void Layer::setCoreCompositorEnabled(bool enabled, std::string transparencyTechnique, std::string numPeels)
{
    m_hasCoreCompositor = enabled;
    if(transparencyTechnique != "")
    {
        if(transparencyTechnique == "DepthPeeling")
        {
            m_transparencyTechnique = ::fwRenderOgre::compositor::DEPTHPEELING;
        }
        else if(transparencyTechnique == "CelShadingDepthPeeling")
        {
            m_transparencyTechnique = ::fwRenderOgre::compositor::CELSHADING_DEPTHPEELING;
        }
        else if(transparencyTechnique == "DualDepthPeeling")
        {
            m_transparencyTechnique = ::fwRenderOgre::compositor::DUALDEPTHPEELING;
        }
        else if(transparencyTechnique == "WeightedBlended")
        {
            m_transparencyTechnique = ::fwRenderOgre::compositor::WEIGHTEDBLENDEDOIT;
        }
        else if(transparencyTechnique == "HybridTransparency")
        {
            m_transparencyTechnique = ::fwRenderOgre::compositor::HYBRIDTRANSPARENCY;
        }
        else
        {
            OSLM_ERROR("Unknown transparency technique : " << transparencyTechnique);
        }
    }

    if(!numPeels.empty())
    {
        m_numPeels = std::stoi(numPeels);
    }
}

//-------------------------------------------------------------------------------------

void Layer::setCompositorChainEnabled(const std::string& compositorChain)
{
    m_hasCompositorChain = !compositorChain.empty();

    if(m_hasCompositorChain)
    {
        m_rawCompositorChain = compositorChain;
    }
}

//-------------------------------------------------------------------------------------

bool Layer::isCoreCompositorEnabled()
{
    return m_hasCoreCompositor;
}

//-------------------------------------------------------------------------------------

bool Layer::isCompositorChainEnabled()
{
    return m_hasCompositorChain;
}

//-------------------------------------------------------------------------------------

Layer::StereoModeType Layer::getStereoMode() const
{
    return m_stereoMode;
}

//-------------------------------------------------------------------------------------

void Layer::setupCore()
{
    // Needed to setup compositors in GL3Plus, Ogre creates render targets
    m_renderService.lock()->makeCurrent();

    m_coreCompositor = std::make_shared< ::fwRenderOgre::compositor::Core>(m_viewport);
    m_coreCompositor->setTransparencyTechnique(m_transparencyTechnique);
    m_coreCompositor->setTransparencyDepth(m_numPeels);
    m_coreCompositor->update();
}

//-------------------------------------------------------------------------------------

::fwRenderOgre::compositor::ChainManager::CompositorChainType Layer::getCompositorChain() const
{
    return m_compositorChainManager->getCompositorChain();
}

//-------------------------------------------------------------------------------------

::fwServices::IHasServices::ServiceVector Layer::getRegisteredAdaptors() const
{
    if(m_compositorChainManager)
    {
        return m_compositorChainManager->getRegisteredServices();
    }
    return ::fwServices::IHasServices::ServiceVector();
}

//-------------------------------------------------------------------------------------

::Ogre::Viewport* Layer::getViewport() const
{
    return m_viewport;
}

//-------------------------------------------------------------------------------------

bool Layer::isSceneCreated() const
{
    return m_sceneCreated;
}

//-------------------------------------------------------------------------------------

::Ogre::Camera* Layer::getDefaultCamera() const
{
    return m_camera;
}

//-------------------------------------------------------------------------------------

Ogre::Matrix4 Layer::getCameraProjMat(const uint8_t cameraIdx) const
{
    SLM_ASSERT("Index exceeds the number of cameras used for this stereo mode", cameraIdx < this->getNumberOfCameras());
    ::Ogre::Matrix4 extrinsicTransform(::Ogre::Matrix4::IDENTITY);

    if(m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5)
    {
        const float eyeAngle = 0.02321f;
        const float angle    = eyeAngle * (-2.f + float(cameraIdx));

        extrinsicTransform = ::fwRenderOgre::helper::Camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8)
    {
        const float eyeAngle = 0.01625f;
        const float angle    = eyeAngle * (-3.5f + float(cameraIdx));

        extrinsicTransform = ::fwRenderOgre::helper::Camera::computeFrustumShearTransform(*m_camera, angle);
    }
    else if(m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::STEREO)
    {
        if(cameraIdx == 1)
        {
            // Kind of arbitrary. Should be configurable in the future.
            extrinsicTransform = ::Ogre::Matrix4(1, 0, 0, 5,
                                                 0, 1, 0, 0,
                                                 0, 0, 1, 0,
                                                 0, 0, 0, 1);
        }
    }

    const ::Ogre::Matrix4 projMat = m_camera->getProjectionMatrixWithRSDepth();

    return projMat * extrinsicTransform;
}

//-------------------------------------------------------------------------------------

uint8_t Layer::getNumberOfCameras() const
{
    return m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 ? 8 :
           m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 ? 5 :
           m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::STEREO       ? 2 : 1;
}

//-------------------------------------------------------------------------------------

void Layer::setHasDefaultLight(bool hasDefaultLight)
{
    m_hasDefaultLight = hasDefaultLight;
}

//-------------------------------------------------------------------------------------

int Layer::getLightsNumber() const
{
    auto lightAdaptors = this->getRenderService()->getAdaptors< ::fwRenderOgre::ILight >();
    int lightsNumber(0);

    std::for_each(lightAdaptors.begin(), lightAdaptors.end(), [&](::fwRenderOgre::ILight::sptr adaptor)
        {
            if(adaptor->getLayerID() == this->getLayerID())
            {
                ++lightsNumber;
            }
        });

    return lightsNumber;
}

//-------------------------------------------------------------------------------------

std::vector< ::fwRenderOgre::ILight::sptr > Layer::getLightAdaptors() const
{
    auto lightAdaptors = this->getRenderService()->getAdaptors< ::fwRenderOgre::ILight >();
    std::vector< ::fwRenderOgre::ILight::sptr > layerLightAdaptors;

    std::for_each(lightAdaptors.begin(), lightAdaptors.end(), [&](::fwRenderOgre::ILight::sptr lightAdaptor)
        {
            if(lightAdaptor->getLayerID() == this->getLayerID())
            {
                layerLightAdaptors.push_back(lightAdaptor);
            }
        });

    return layerLightAdaptors;
}

//-------------------------------------------------------------------------------------

} // namespace fwRenderOgre
