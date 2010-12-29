/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUI_SELECTOR_HPP_
#define FWGUI_SELECTOR_HPP_

#include <vector>
#include <fwCore/base.hpp>

#include "fwGui/fwContainer.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

//------------------------------------------------------------------------------

/**
 * @brief   ISelector allowing the choice of an element among severals (_selections)
 * @class   ISelector.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 */
class FWGUI_CLASS_API ISelector : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ISelector)(::fwCore::BaseObject) )

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    FWGUI_API ISelector() ;

    /// Destructor. Do nothing.
    FWGUI_API virtual ~ISelector();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUI_API virtual void setSelections(std::vector< std::string > _selections) = 0;

    /**
     * @brief Sets the selector title.
     */
    FWGUI_API virtual void setTitle(std::string _title) = 0;

    /**
     * @brief Show the selector and return the selection.
     * @param[in] _parent Parent container for the selector.
     */
    FWGUI_API virtual std::string show() = 0;
};


}

#endif /*FWGUI_SELECTOR_HPP_*/