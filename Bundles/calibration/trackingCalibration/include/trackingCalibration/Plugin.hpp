/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TRACKINGCALIBRATION_PLUGIN_HPP__
#define __TRACKINGCALIBRATION_PLUGIN_HPP__

#include "trackingCalibration/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace trackingCalibration
{

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method.
     *
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime in order to initialize the bundle.
     */
    void start();

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() noexcept;
};

} // namespace trackingCalibration

#endif // __TRACKINGCALIBRATION_PLUGIN_HPP__
