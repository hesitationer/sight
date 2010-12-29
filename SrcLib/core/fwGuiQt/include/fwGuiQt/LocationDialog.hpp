/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _fWGUIQT_LOCATIONDIALOG_HPP_
#define _fWGUIQT_LOCATIONDIALOG_HPP_

#include <fwCore/base.hpp>

#include <fwGui/ILocationDialog.hpp>

#include "fwGuiQt/config.hpp"


class QString;



namespace fwGuiQt
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API LocationDialog : public ::fwGui::ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::ILocationDialog), (()), new LocationDialog );

    LocationDialog();

    void setTitle(const std::string &title);

    ::fwData::location::ILocation::sptr show();

    // FIXME ok with Folder but not singleFile
    void setDefaultLocation( ::fwData::location::ILocation::csptr );

    void setType( ::fwGui::ILocationDialog::Types type );

    ::fwGui::ILocationDialog& setOption( ::fwGui::ILocationDialog::Options option);

    // exemple ( addFilter("images","*.png *.jpg");
    void addFilter(const std::string &filterName, const std::string &wildcardList );


protected:
    std::string m_title;
    ::boost::filesystem::path m_path;
    ::fwGui::ILocationDialog::Options m_style;
    ::fwGui::ILocationDialog::Types m_type;
    std::multimap< std::string, std::string > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();

};

} // namespace fwGuiQt

#endif /*_fWGUIQT_LOCATIONDIALOG_HPP_*/

