/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QAction>
#include <QActionGroup>
#include <QToolBar>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/ActionCallback.hpp"
#include "fwGuiQt/container/QtToolBarContainer.hpp"
#include "fwGuiQt/container/QtMenuItemContainer.hpp"
#include "fwGuiQt/layoutManager/ToolBarLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IToolBarLayoutManager,
        ::fwGui::layoutManager::ToolBarLayoutManager,
         ::fwGui::layoutManager::IToolBarLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout( ::fwGui::fwToolBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtToolBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwToolBar to QtToolBarContainer failed", m_parent);

    QToolBar* toolBar = m_parent->getQtToolBar();

    QActionGroup * actionGroup = 0;
    unsigned int menuItemIndex = 0;
    BOOST_FOREACH ( ::fwGui::layoutManager::IToolBarLayoutManager::ActionInfo actionInfo, m_actionInfo)
    {
        if (actionInfo.m_isSeparator)
        {
            toolBar->addSeparator();
            actionGroup = 0;
        }
        else
        {
            ::fwGuiQt::container::QtMenuItemContainer::NewSptr menuItem;

            QIcon icon(QString::fromStdString(actionInfo.m_icon));

            QAction *action = toolBar->addAction( icon, QString::fromStdString(actionInfo.m_name) );

            action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

            if (actionInfo.m_isRadio)
            {
                if (!actionGroup)
                {
                    actionGroup = new QActionGroup(toolBar);
                }
                actionGroup->addAction(action);
            }

            menuItem->setQtMenuItem(action);

            if(!actionInfo.m_isSeparator)
            {
                m_menuItems.push_back(menuItem);
                OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
                ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

                ::fwGuiQt::ActionCallback::sptr qtCallback = ::fwGuiQt::ActionCallback::dynamicCast(callback);
                SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

                QObject::connect( action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
                QObject::connect( action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
                menuItemIndex++;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::destroyLayout()
{
    QToolBar* menu = m_parent->getQtToolBar();

    m_menuItems.clear();
    menu->clear();
}

//-----------------------------------------------------------------------------


void ToolBarLayoutManager::menuItemSetVisible(::fwGui::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(::fwGui::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(::fwGui::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui


