/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/ChainManager.hpp"

#include <fwCore/spyLog.hpp>

#include <fwCore/spyLog.hpp>

#include <OGRE/OgreCompositorManager.h>

namespace fwRenderOgre
{

namespace compositor
{

//-----------------------------------------------------------------------------

const ChainManager::CompositorIdType ChainManager::FINAL_CHAIN_COMPOSITOR = "FinalChainCompositor";

//-----------------------------------------------------------------------------

ChainManager::ChainManager(::Ogre::Viewport* viewport) :
    m_ogreViewport(viewport)
{
}

//-----------------------------------------------------------------------------

void ChainManager::addAvailableCompositor(CompositorIdType compositorName)
{
    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();

    // Look the final chain compositor
    auto finalChainCompositorIt = std::find_if(m_compositorChain.begin(),
                                               m_compositorChain.end(),
                                               FindCompositorByName(FINAL_CHAIN_COMPOSITOR));

    if(finalChainCompositorIt != m_compositorChain.end())
    {
        compositorManager->setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, false);
        compositorManager->removeCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
        m_compositorChain.pop_back();
    }

    // Add the new compositor
    m_compositorChain.push_back(CompositorType(compositorName, false));
    compositorManager->addCompositor(m_ogreViewport, compositorName);

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::clearCompositorChain()
{
    m_compositorChain.clear();
    this->getCompositorManager()->removeCompositorChain(m_ogreViewport);
}

//-----------------------------------------------------------------------------

void ChainManager::updateCompositorState(CompositorIdType compositorName, bool isEnabled)
{
    // If there isn't any compositor available, the update operation can't be done
    if(!m_compositorChain.empty())
    {
        auto compositorToUpdate = std::find_if(m_compositorChain.begin(),
                                               m_compositorChain.end(),
                                               FindCompositorByName(compositorName));

        if(compositorToUpdate != m_compositorChain.end())
        {
            compositorToUpdate->second = isEnabled;
            this->getCompositorManager()->setCompositorEnabled(m_ogreViewport, compositorName, isEnabled);
        }
    }
}

//-----------------------------------------------------------------------------

void ChainManager::setCompositorChain(std::vector<CompositorIdType> compositors)
{
    this->clearCompositorChain();

    ::Ogre::CompositorManager* compositorManager = this->getCompositorManager();

    for(CompositorIdType compositorName : compositors)
    {
        if(this->getCompositorManager()->resourceExists(compositorName))
        {
            m_compositorChain.push_back(CompositorType(compositorName, true));
            compositorManager->addCompositor(m_ogreViewport, compositorName);
            compositorManager->setCompositorEnabled(m_ogreViewport, compositorName, true);
        }
        else
        {
            OSLM_WARN("\"" << compositorName << "\" does not refer to an existing compositor");
        }
    }

    this->addFinalCompositor();
}

//-----------------------------------------------------------------------------

void ChainManager::addFinalCompositor()
{
    // Add final chain compositor
    m_compositorChain.push_back(CompositorType(FINAL_CHAIN_COMPOSITOR, true));
    this->getCompositorManager()->addCompositor(m_ogreViewport, FINAL_CHAIN_COMPOSITOR);
    this->getCompositorManager()->setCompositorEnabled(m_ogreViewport, FINAL_CHAIN_COMPOSITOR, true);
}

//-----------------------------------------------------------------------------

::Ogre::CompositorManager* ChainManager::getCompositorManager()
{
    return ::Ogre::CompositorManager::getSingletonPtr();
}

//-----------------------------------------------------------------------------

} // namespace compositor

} // namespace fwRenderOgre
