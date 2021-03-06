/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKARAdaptor/config.hpp"

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace visuVTKARAdaptor
{

/**
 * @brief Display a 3D point list.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the pointlist
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKARAdaptor::SPointList3D" autoConnect="yes">
       <in key="pointList" uid="..." />
       <config renderer="default" color="#cb1f72" radius="3.0" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b pointList [::fwData::PointList]: displayed point list
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of the renderer the adaptor must use.
 * - \b color(optional, default=3.): color used to display the points.
 * - \b radius (optional, default="#ffffff") : point sphere radius.
 * - \b transform (optional) : transform applied to the displayed points.
 */
class VISUVTKARADAPTOR_CLASS_API SPointList3D : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPointList3D)(::fwRenderVTK::IAdaptor) );

    /// Constructor.
    VISUVTKARADAPTOR_API SPointList3D() noexcept;

    /// Destructor.
    VISUVTKARADAPTOR_API virtual ~SPointList3D() noexcept;

protected:

    /// Configure the service.
    VISUVTKARADAPTOR_API void configuring() override;

    /// Initialize the actor and update the point list.
    VISUVTKARADAPTOR_API void starting() override;

    /// Update the point list.
    VISUVTKARADAPTOR_API void updating() override;

    /// Destroy all vtk props created at start.
    VISUVTKARADAPTOR_API void stopping() override;

    /// Restart the service (stop-start).
    VISUVTKARADAPTOR_API void swapping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ::fwData::PointList::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ::fwData::PointList::s_POINT_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect ::fwData::PointList::s_POINT_REMOVED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKARADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: update point list visibility (true = visible)
    void updateVisibility ( bool isVisible );

    /// vtk point list.
    vtkSmartPointer<vtkPoints> m_points;

    /// Sphere radius.
    double m_radius;

    /// Point color.
    ::fwData::Color::sptr m_ptColor;

    /// Point list actor.
    vtkSmartPointer<vtkActor> m_actor;

};

} //namespace visuVTKRDAdaptor
