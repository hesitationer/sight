/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMATH_INTRASECTYPES_HPP__
#define __FWMATH_INTRASECTYPES_HPP__

#include <vector>
#include <array>

/**
 * @brief Definition of a type for a vector of tree double.
 */
typedef std::array<double, 3> fwVec3d;

/**
 * @brief Definition of a type for a line defined by a position and a direction
 */
typedef std::pair<fwVec3d, fwVec3d> fwLine;

/**
 * @brief Definition of type for a plane defined by a normal and a distance
 */
typedef std::array<double, 4> fwPlane;

/**
 * @brief Definition of a type for a matrix 4*4
 */
typedef std::array< std::array< double,4 >, 4> fwMatrix4x4;

/**
 * @brief Definition of a type for vertex positions
 */
typedef std::vector < std::vector <float> > fwVertexPosition;

/**
 * @brief Definition of a type for vertex index
 */
typedef std::vector < std::vector <int> > fwVertexIndex;


#endif /* __FWMATH_INTRASECTYPES_HPP__ */
