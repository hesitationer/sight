/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <vector>

namespace fwGui
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   ISelectorDialog allows the choice of an element among several (_selections)
 */
class FWGUI_CLASS_API ISelectorDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ISelectorDialog)(::fwGui::GuiBaseObject) )

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor builds a selector proposing a string list
     */
    FWGUI_API ISelectorDialog();

    /// Destructor. Do nothing.
    FWGUI_API virtual ~ISelectorDialog();

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
     */
    FWGUI_API virtual std::string show() = 0;

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string& msg) = 0;

    /// Add a custom button to this dialog
    FWGUI_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) = 0;
};

} //namespace dialog
} //namespace fwGui
