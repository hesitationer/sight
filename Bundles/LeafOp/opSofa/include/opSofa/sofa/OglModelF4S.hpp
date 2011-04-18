#ifndef OPSOFA_SOFA_OGLMODELF4S_H
#define OPSOFA_SOFA_OGLMODELF4S_H


#include <sofa/component/visualmodel/OglModel.h>
#include <fwData/TriangularMesh.hpp>

/**
 * @brief Class inherited from OglModel and permitting the SOFA framework
 * to load directly the data without reading the .trian file
 */
class OglModelF4S : public sofa::component::visualmodel::OglModel
{

public:
    void loadTriangularMesh(::fwData::TriangularMesh::sptr);
};


#endif // OPSOFA_SOFA_OGLMODELF4S_H