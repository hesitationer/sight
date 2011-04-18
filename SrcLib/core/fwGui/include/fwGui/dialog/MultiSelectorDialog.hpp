/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_MULTISELECTORDIALOG_HPP_
#define _FWGUI_MULTISELECTORDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/IMultiSelectorDialog.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   MultiSelectorDialog allowing the choice of an element among severals (_selections)
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   MultiSelectorDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API MultiSelectorDialog : public IMultiSelectorDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MultiSelectorDialog)(::fwGui::dialog::IMultiSelectorDialog), (()), new MultiSelectorDialog );

    /// will instanciate the concrete implementation
    FWGUI_API MultiSelectorDialog();

    /// override
    FWGUI_API void setTitle(std::string title);

    /// override
    FWGUI_API virtual void setSelections(Selections _selections);

    /// override
    FWGUI_API Selections show();

    /// Set the message
    FWGUI_API virtual void setMessage(const std::string &msg);

protected :

    ::fwGui::dialog::IMultiSelectorDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_MULTISELECTORDIALOG_HPP_*/

