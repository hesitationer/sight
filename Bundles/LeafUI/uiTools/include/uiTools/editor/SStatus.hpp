/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_EDITOR_SSTATUS_HPP__
#define __UITOOLS_EDITOR_SSTATUS_HPP__

#include "uiTools/config.hpp"

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLabel>
#include <QPointer>

class QPushButton;

namespace uiTools
{

namespace editor
{

/**
 * @brief   SStatus service shows a colored square (red, orange, green) representing a status.
 *
 * @note To change the status color, you should call the slots 'changeToGreen', * 'changeToOrange', 'changeToRed'
 * or 'toggleGreenRed'.
 *
 * @brief Configures the status tooltip
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="..." type="::uiTools::editor::SStatus">
       <form>square|circle</form>
       <count>1</count>
       <layout>horizontal|vertical</layout>
       <size>
           <width>20</width>
           <height>20</height>
       </size>
       <labels>
            <labelStatus>SCP Server</labelStatus>
            <labelStatus>TCP Server</labelStatus>
       </labels>
       <red>Stopped</red>
       <green>Tracking</green>
       <orange>Started</orange>
   </service>
   @endcode
 * - \b form (optional, 'square' by default): the form of the indicator
 * - \b count (optional, '1' by default): the number of status
 * - \b layout(optional, 'horizontal' by default): orientation of the layout
 * - \b size (optional): the size of the indicator
 *   - \b width (optional, 20 by default): the width of the indicator
 *   - \b height (optional, 20 by default): the height of the indicator
 * - \b labels (optional): the description associated to the indicators
 *   - \b labelStatus (optional): the description associated to the indicator
 * - \b red (optional): the description associated to the red status
 * - \b green (optional): the description associated to the green status
 * - \b orange (optional): the description associated to the orange status
 *
 * @section Slots Slots
 * - \b changeToGreen(): This slot allows to change the indicator color to green. If there is more than one status, it
 * only changes the first one.
 * - \b changeToRed(): This slot allows to change the indicator color to red. If there is more than one status, it only
 * changes the first one.
 * - \b changeToOrange(): This slot allows to change the indicator color to orange. If there is more than one status, it
 * only changes the first one.
 * - \b toggleGreenRed(bool): This slot allows to change the indicator color to green or red. If there is more than one
 * status, it only toggles the first one.
 * - \b positionChangeToGreen(int): This slot allows to change the indicator color to green for the ith status.
 * - \b positionChangeToRed(int): This slot allows to change the indicator color to red for the ith status.
 * - \b positionChangeToOrange(int): This slot allows to change the indicator color to orange for the ith status.
 * - \b positionToggleGreenRed(int,bool): This slot allows to change the indicator color to green or red for the ith
 * status.
 */
class UITOOLS_CLASS_API SStatus : public QObject,
                                  public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SStatus)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UITOOLS_API SStatus() noexcept;

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SStatus() noexcept;

    /**
     * @name Slots API
     *@{
     */
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_GREEN;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_RED;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_ORANGE;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_GREEN_RED;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_POSITION_CHANGE_TO_GREEN;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_POSITION_CHANGE_TO_RED;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_POSITION_CHANGE_TO_ORANGE;
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_POSITION_TOGGLE_GREEN_RED;
    /** @} */

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

    /// Configures the service
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

    /// SLOT : change label color
    void changeToGreen();

    /// SLOT : change label color
    void changeToRed();

    /// SLOT : change label color
    void changeToOrange();

    /// SLOT : change label color (true = green, false = red)
    void toggleGreenRed(const bool green);

    /// SLOT : change ith label color
    void positionChangeToGreen(const int index);

    /// SLOT : change ith label color
    void positionChangeToRed(const int index);

    /// SLOT : change ith label color
    void positionChangeToOrange(const int index);

    /// SLOT : change ith label color (true = green, false = red)
    void positionToggleGreenRed(const int index, const bool green);

private:

    /// Number of status
    size_t m_count;

    QVector< QPointer< QLabel > > m_indicator;
    QVector< QPointer< QLabel > > m_labelStatus;

    std::string m_greenTooltip; ///< Tooltip for green status
    std::string m_redTooltip; ///< Tooltip for red status
    std::string m_orangeTooltip; ///< Tooltip for orange status
    std::string m_layout; ///< Layout orientation

    bool m_isCircular; ///< label is a circle if true (else it's a square)

    size_t m_width; ///< width of idicator
    size_t m_height; ///< height of idicator
};

} // namespace editor
} // namespace uiTools

#endif /*__UITOOLS_EDITOR_SSTATUS_HPP__*/

