/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiZMQ/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGui/editor/IDialogEditor.hpp>

#include <zmqNetwork/Socket.hpp>

#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QSpinBox>
#include <QWidget>

namespace uiZMQ
{
/**
 *
 * @brief config editor for ioZMQ listener and sender services
 */
class UIZMQ_CLASS_API SZMQConfigEditor :
    public QObject,
    public ::fwGui::editor::IDialogEditor
{
Q_OBJECT
public:

    /**
     * @brief Configuration updated signal is emitted when a user confirm change
     */
    UIZMQ_API static const ::fwCom::Signals::SignalKeyType s_CONFIGURATION_UPDATED_SIGNAL;

    /**
     * @typedef ConfigurationUpdatedSignalType
     *
     * @brief ConfigurationUpdatedSignalType is stored and emitted when user confirm change
     */
    typedef ::fwCom::Signal < void (::zmqNetwork::Socket::PatternMode const,
                                    ::zmqNetwork::Socket::SocketMode const,
                                    std::string const& host) > ConfigurationUpdatedSignalType;

    fwCoreServiceClassDefinitionsMacro( (SZMQConfigEditor) (::fwGui::editor::IDialogEditor));

    /**
     * @brief create gui interface
     */
    UIZMQ_API virtual void configuring() override;

    /// Overrides
    UIZMQ_API virtual void starting() override;

    /// Overrides
    UIZMQ_API virtual void stopping() override;

    /**
     * @brief show the dialog
     */
    UIZMQ_API virtual void updating() override;

    /// Overrides
    UIZMQ_API virtual void swapping() override;

    /// Constructor
    UIZMQ_API SZMQConfigEditor();

    /**
     * @brief this method will be removed later implemented in child classes. These child classes only fill the
     *  m_patternBox
     */
    UIZMQ_API virtual void initPatternBox() = 0;

    /// Destructor
    UIZMQ_API virtual ~SZMQConfigEditor() noexcept;

private Q_SLOTS:
    /**
     * @brief called when a user click to validate button. This method send a ConfigurationUpdated signal.
     */
    void onValidate();

    /**
     * @brief called when user change protocol
     */
    void onChangeProtocol(QString const& protocol);
protected:
    QComboBox* m_patternBox;
private:

    QComboBox* m_modeBox;
    QComboBox* m_protocolBox;
    QLineEdit* m_host;
    QSpinBox* m_port;
    QLabel* m_title;
    QWidget* m_portWidget;
    QDialog m_dialog;
    std::string m_protocol;

    ConfigurationUpdatedSignalType::sptr m_configurationUpdatedSig;
};

} // namespace uiZMQ
