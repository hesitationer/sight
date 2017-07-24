/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_HISTOGRAMVALUE_HPP__
#define __SCENE2D_ADAPTOR_HISTOGRAMVALUE_HPP__

#include "scene2D/config.hpp"

#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <QGraphicsTextItem>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief IAdaptor implementation to show the histogram values clicked by mouse.
 *
 *
 * \b xAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b yAxis     : see ::fwRenderQt::IAdaptor
 *
 * \b zValue    : see ::fwRenderQt::IAdaptor
 *
 * \b color     : inner color
 *
 * \b fontSize  : size of the font used to display the current value
 *
 * \b opacity   : from 0.0 to 1.0, default value is 0.8
 *
 * \b viewportUID : a viewport that help us to manage the scaling of the graphic object
 *
 */
class HistogramValue : public ::fwRenderQt::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (HistogramValue)(::fwRenderQt::IAdaptor) );
    SCENE2D_API HistogramValue() noexcept;
    SCENE2D_API virtual ~HistogramValue() noexcept;

protected:
    SCENE2D_API void configuring();
    SCENE2D_API void doStart();
    SCENE2D_API void doUpdate();
    SCENE2D_API void doSwap();
    SCENE2D_API void doStop();

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /// Color used for graphic item's inner color
    QPen m_color;

    /// An item which display the current values of the associated histogram pointed by this cursor.
    QGraphicsSimpleTextItem* m_text;

    /// If true, display the currently pointed intensity value
    bool m_isInteracting;

    ///
    QFont m_font;

    /// Coordinates of the current event.
    ::fwRenderQt::data::Coord m_coord;

    /// Size of the font used for rendering the current value of this tracker.
    float m_fontSize;

    /// The layer.
    QGraphicsItemGroup* m_layer;

    /// Curve point value at the current index of the histogram pointed by the mouse.
    std::string m_histogramPointUID;

private:

    ::fwRenderQt::data::Viewport::sptr m_viewport;

    /// fWID of the viewport
    std::string m_viewportID;

    /// Connection to the viewport
    ::fwCom::Connection m_connection;
};

}   // namespace adaptor
}   // namespace scene2D

#endif  // __SCENE2D_ADAPTOR_HISTOGRAMVALUE_HPP__

