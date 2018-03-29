/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Camera.hpp"

namespace fwRenderOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

Ogre::Matrix4 Camera::computeFrustumShearTransform(const ::Ogre::Camera& _camera, float _angle)
{
    ::Ogre::Matrix4 shearTransform = ::Ogre::Matrix4::IDENTITY;

    const float focalLength  = _camera.getFocalLength();
    const float xshearFactor = std::tan(_angle);

    shearTransform[0][2] = -xshearFactor;
    shearTransform[0][3] = -focalLength * xshearFactor;

    return shearTransform;
}

//-----------------------------------------------------------------------------

Ogre::Matrix4 Camera::computeProjectionMatrix(const ::arData::Camera& _calibration, float _width,
                                              float _height, float _near, float _far)
{
    const float fx = static_cast< float >(_calibration.getFx());
    const float fy = static_cast< float >(_calibration.getFy());

    const float cx = static_cast< float >( _calibration.getCx() );
    const float cy = static_cast< float >( _calibration.getCy() );

    //calibration images size
    const float imW = static_cast< float >( _calibration.getWidth() );
    const float imH = static_cast< float >( _calibration.getHeight() );

    //compute ratio between calibration image height & displayed image height
    const float ratioH = _height / imH;

    //compute new fx, fy
    const float nfx = fx * ratioH;
    const float nfy = fy * ratioH;

    // compute principle point offset according to size of displayed image
    float px       = ratioH * cx;
    const float py = ratioH * cy;

    const long expectedWindowSize = std::lround(ratioH * imW);

    if( expectedWindowSize != static_cast<long>(_width))
    {
        const long diffX = (static_cast<long>(_width) - expectedWindowSize) / 2;
        px += static_cast<float>(diffX);
    }

    const float cx1 = _width - px;
    const float cy1 = _height - py;

    // compute the offset according to current size
    const float wcx = cx1 / ( (_width - 1.f) / 2.f) -1.f;
    const float wcy = cy1 / ( (_height - 1.f) / 2.f) -1.f;

    // setup projection matrix
    ::Ogre::Matrix4 m;

    m[0][0] = 2.f * nfx / _width;
    m[0][1] = 0.f;
    m[0][2] = wcx;
    m[0][3] = 0.f;

    m[1][0] = 0.f;
    m[1][1] = 2.f * nfy / _height;
    m[1][2] = -wcy;
    m[1][3] = 0.f;

    m[2][0] = 0.f;
    m[2][1] = 0.f;
    m[2][2] = -(_far + _near) / (_far - _near);
    m[2][3] = -2.f * _far * _near / (_far - _near);

    m[3][0] = 0.f;
    m[3][1] = 0.f;
    m[3][2] = -1.f;
    m[3][3] = 0.f;

    return m;
}

} // namespace helper

} // namespace fwRenderOgre
