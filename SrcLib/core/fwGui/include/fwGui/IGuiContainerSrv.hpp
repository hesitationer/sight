/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IGUICONTAINERSRV_HPP_
#define _FWGUI_IGUICONTAINERSRV_HPP_

#include <fwServices/IService.hpp>

#include "fwGui/config.hpp"
#include "fwGui/fwContainer.hpp"

#include "fwGui/registrar/ViewRegistrar.hpp"
#include "fwGui/layoutManager/IViewLayoutManager.hpp"
#include "fwGui/builder/IToolBarBuilder.hpp"


/**
 * @brief   The namespace fwGui contains the base interface for IHM services.
 * @namespace   fwGui
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
namespace fwGui
{

/**
 * @brief   Defines the service interface managing the layout.
 * @class   IGuiContainerSrv.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IGuiContainerSrv : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (IGuiContainerSrv)(::fwServices::IService) ) ;

protected :
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWGUI_API IGuiContainerSrv() ;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWGUI_API virtual ~IGuiContainerSrv() ;

    FWGUI_API void initialize();

    FWGUI_API void create();

    FWGUI_API void destroy();

    FWGUI_API ::fwGui::fwContainer::sptr getContainer();

private:

    void initializeLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig );
    void initializeToolBarBuilder( ::fwRuntime::ConfigurationElement::sptr toolBarConfig );

    bool m_viewLayoutManagerIsCreated;
    ::fwGui::layoutManager::IViewLayoutManager::sptr m_viewLayoutManager;

    ::fwGui::registrar::ViewRegistrar::sptr          m_viewRegistrar;
    ::fwGui::builder::IToolBarBuilder::sptr           m_toolBarBuilder;

    ConfigurationType m_viewRegistrarConfig;
    ConfigurationType m_viewLayoutConfig;
    ConfigurationType m_toolBarConfig;

    bool m_hasToolBar;
};

} // namespace fwGui

#endif /*_FWGUI_IGUICONTAINERSRV_HPP_*/

