/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_BUILDER_ITOOLBARBUILDER_HPP_
#define _FWGUI_BUILDER_ITOOLBARBUILDER_HPP_

#include <fwCore/base.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwGui/fwToolBar.hpp"
#include "fwGui/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for toolBar.
 * @class   IToolBarBuilder.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IToolBarBuilder : public ::fwCore::BaseObject
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IToolBarBuilder)(::fwCore::BaseObject) )

    typedef std::string RegistryKeyType;

    FWGUI_API const static RegistryKeyType REGISTRY_KEY;

    /// Constructor. Do nothing.
    FWGUI_API IToolBarBuilder();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~IToolBarBuilder();

    /**
     * @brief Returns the builded tool bar.
     */
    FWGUI_API virtual ::fwGui::fwToolBar::sptr getToolBar();

    /**
     * @brief Configure the layout before creation.
     */
    FWGUI_API virtual void initialize( ::fwRuntime::ConfigurationElement::sptr configuration);

    /**
     * @brief Instantiate layout with parent toolBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent toolBar must be instanced.
     */
    FWGUI_API virtual void createToolBar( ::fwGui::fwContainer::sptr parent ) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUI_API virtual void destroyToolBar() = 0;

protected:

    /// ToolBar.
    ::fwGui::fwToolBar::sptr m_toolBar;

    std::pair< int, int > m_toolBitmapSize;

};

} // namespace builder
} // namespace fwGui

#endif /*_FWGUI_BUILDER_ITOOLBARBUILDER_HPP_*/

