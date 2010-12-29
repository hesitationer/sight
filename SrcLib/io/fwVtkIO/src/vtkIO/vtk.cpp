/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <stdexcept>
#include <boost/bimap/bimap.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/cast.hpp>

#include <vtkImageImport.h>
#include <vtkSetGet.h>

// for mesh
#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataWriter.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataNormals.h>
#include <vtkLookupTable.h>

#include <vtkMassProperties.h>

#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageAccumulate.h>

#include <vtkDataSetAttributes.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
//#include <>
//vi->GetPointData()->GetScalars()->FillComponent(0, 1.0);
#include <fwMath/MeshFunctions.hpp>


#include "vtkIO/vtk.hpp"


namespace vtkIO
{


//namespace { // anonymous namespace


struct DynamicTypeOrderer
{
  bool operator()(const fwTools::DynamicType & d1, const fwTools::DynamicType &d2) const
  {
    return d1.string() < d2.string();
  }
};




// BOOST 1.38 -> ::boost::bimaps::bimap<....>    note bimapS  ## BOOST 1.39 -> ::boost::bimap::bimap<....>
typedef  ::boost::bimaps::bimap<
                    ::boost::bimaps::set_of< fwTools::DynamicType , ::vtkIO::DynamicTypeOrderer  >
                  , ::boost::bimaps::set_of< int >
                   > TypeTranslator;

TypeTranslator PixelTypeTranslation = boost::assign::list_of< TypeTranslator::relation >
                                                                    ( fwTools::makeDynamicType<signed char>(),    VTK_SIGNED_CHAR )
                                                                    ( fwTools::makeDynamicType<unsigned char>(),  VTK_UNSIGNED_CHAR )
                                                                    ( fwTools::makeDynamicType<signed short>(),   VTK_SHORT )
                                                                    ( fwTools::makeDynamicType<unsigned short>(), VTK_UNSIGNED_SHORT )
                                                                    ( fwTools::makeDynamicType<signed int>(),     VTK_INT )
                                                                    ( fwTools::makeDynamicType<unsigned int>(),   VTK_UNSIGNED_INT )
                                                                    ( fwTools::makeDynamicType<signed long>(),    VTK_LONG )
                                                                    ( fwTools::makeDynamicType<unsigned long>(),  VTK_UNSIGNED_LONG )
                                                                    ( fwTools::makeDynamicType<float>(),          VTK_FLOAT )
                                                                    ( fwTools::makeDynamicType<double>(),         VTK_DOUBLE );


//} // end anonymous namespace


const char *myScalarTypeCallback(void *imageData)
{
    ::fwData::Image *trueImageData = static_cast< ::fwData::Image *>(imageData);

    assert ( PixelTypeTranslation.left.find( trueImageData->getPixelType() )!= PixelTypeTranslation.left.end() );

    return vtkImageScalarTypeNameMacro( PixelTypeTranslation.left.at( trueImageData->getPixelType() ) );
}

//-----------------------------------------------------------------------------

vtkImageData* toVTKImage( ::boost::shared_ptr< ::fwData::Image > data,  vtkImageData *dst)
{
    vtkImageImport *importer = vtkImageImport::New();

    importer->SetDataSpacing( data->getSpacing().at(0),
                              data->getSpacing().at(1),
                              data->getSpacing().at(2)
                            );

    importer->SetDataOrigin( data->getCRefOrigin().at(0),
                             data->getCRefOrigin().at(1),
                             data->getCRefOrigin().at(2)
                            );

    importer->SetWholeExtent(   (int)data->getCRefOrigin().at(0), data->getCRefSize().at(0) -1,
                                (int)data->getCRefOrigin().at(1), data->getCRefSize().at(1) -1,
                                (int)data->getCRefOrigin().at(2), data->getCRefSize().at(2) -1
                            );

    // copy WholeExtent to DataExtent
    importer->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    importer->SetImportVoidPointer( data->getBuffer() );


    // used to set correct pixeltype to VtkImage
    // vtkImageImport::InvokeExecuteInformationCallbacks() will call
    // myScalarTypeCallback and waiting for const char * defintion type like "unsigned short"
    // to produce VTK_TYPE
    importer->SetCallbackUserData( data.get() );
    importer->SetScalarTypeCallback( myScalarTypeCallback );


    importer->Update();

    vtkImageData *vtkImage = importer->GetOutput();

    if (dst)
    {
        dst->ShallowCopy(vtkImage);
        importer->Delete();
    }

    SLM_ERROR_IF("::vtkIO::toVTKImage will not longer return a vtkImageData."
                  "You should update the usage of this function in your code, "
                  "and pass a vtkImageData as second argument", dst == NULL);

    return vtkImage;

}

//-----------------------------------------------------------------------------

bool fromVTKImage( vtkImageData* source, ::boost::shared_ptr< ::fwData::Image > destination )
{
    assert(destination && source );

    int dim = source->GetDataDimension() ;
    OSLM_TRACE("source->GetDataDimension() : " << dim);
    destination->setDimension( dim );
    std::copy( source->GetDimensions(), source->GetDimensions()+dim, destination->getRefSize().begin() );
    std::copy( source->GetSpacing(), source->GetSpacing()+dim, destination->getRefSpacing().begin() );
    std::copy( source->GetOrigin(), source->GetOrigin()+dim, destination->getRefOrigin().begin() );

    // ensure image size correct
    source->UpdateInformation();
    source->PropagateUpdateExtent();
    // GetEstimatedMemorySize return value in kilobytes
    unsigned long imageMemSize = source->GetEstimatedMemorySize() * 1024;

    void *input = source->GetScalarPointer();
    void *destBuffer;
    bool res=false;

    if (imageMemSize != 0)
    {
        int nbImg = (destination->getRefSize()[2] == 0) ? 1 : destination->getRefSize()[2] ;
        int bytePerPixel = (((imageMemSize / destination->getRefSize()[0] ) / destination->getRefSize()[1] ) / nbImg ) / source->GetNumberOfScalarComponents();
        int components = source->GetNumberOfScalarComponents();
        int size = ( imageMemSize / components );// * bytePerPixel;
        OSLM_TRACE("imageMemSize : " << imageMemSize << " - bytePerPixel : " << bytePerPixel << " - finalSize : " << size);
        try
        {
            destBuffer = new char[ size ];
            res = true;
        }
        catch (std::exception &e)
        {
            OSLM_ERROR ("Need more memory : " << e.what() );
        }

        if (res)
        {
            if (components == 3 && bytePerPixel == 2)
            {
                OSLM_TRACE ("RGB 16bits");
                unsigned short* destBufferTyped = ( unsigned short*)destBuffer;
                unsigned short* inputTyped= (unsigned short*)input;
                unsigned short* finalPtr = ((unsigned short*)destBuffer) + size;
                while (destBufferTyped < finalPtr)
                {
                    unsigned short valR = (unsigned short)(float((*(inputTyped++)) * 0.30));
                    unsigned short valG = (unsigned short)(float((*(inputTyped++)) * 0.59));
                    unsigned short valB = (unsigned short)(float((*(inputTyped++)) * 0.11));
                    (*destBufferTyped)= valR + valG + valB;
                    destBufferTyped++;
                }
                destination->setPixelType( fwTools::makeDynamicType<unsigned short>() );
            }
            else
            {
                if (components == 3 && bytePerPixel == 1)
                {
                    OSLM_TRACE ("RGB 8bits");
                    unsigned char* destBufferTyped = ( unsigned char*)destBuffer;
                    unsigned char* inputTyped= ( unsigned char*)input;
                    unsigned char* finalPtr = (( unsigned char*)destBuffer) + size;
                    while (destBufferTyped < finalPtr)
                    {
                        unsigned char valR = (unsigned char)(float((*(inputTyped++)) * 0.30));
                        unsigned char valG = (unsigned char)(float((*(inputTyped++)) * 0.59));
                        unsigned char valB = (unsigned char)(float((*(inputTyped++)) * 0.11));
                        (*destBufferTyped)= valR + valG + valB;
                        destBufferTyped++;
                    }
                    destination->setPixelType( fwTools::makeDynamicType<unsigned char>() );
                }
                else
                {

                    if (components == 1)
                    {
                        OSLM_TRACE ("Luminance image");
                        memcpy( destBuffer, input , imageMemSize);
                        destination->setPixelType( PixelTypeTranslation.right.at( source->GetScalarType() ) );
                    }
                    else
                    {
                        OSLM_ERROR ("Dicom image type not supported (image dimension)");
                    }
                }
            }
            destination->setBuffer( destBuffer );
        }
    }
    else
    {
        OSLM_INFO("imageMemSize : " << imageMemSize );
    }

    if (!res)
    {
        destination->getRefSize()[0] = 0;
        destination->getRefSize()[1] = 0;
        destination->getRefSize()[2] = 0;
    }

    for (boost::uint8_t  d=0; d<dim; ++d)
    {
        OSLM_TRACE("Size " << destination->getCRefSize()[d]);
        OSLM_TRACE("Origin " << destination->getCRefOrigin()[d]);
        OSLM_TRACE("Spacing " << destination->getCRefSpacing()[d]);
        destination->getRefOrigin()[d]=0.0; //FIXME !!! Hack because our framework (visu services) doesn't support origine
    }

    OSLM_TRACE ("Exit from fromVTKImage");

    return res;
}



//------------------------------------------------------------------------------

vtkImageImport* convertToVTKImageImport( ::boost::shared_ptr< ::fwData::Image > data )
{
    vtkImageImport *importer = vtkImageImport::New();

    importer->SetDataSpacing( data->getSpacing().at(0),
                              data->getSpacing().at(1),
                              data->getSpacing().at(2)
                            );

    importer->SetDataOrigin( data->getCRefOrigin().at(0),
                             data->getCRefOrigin().at(1),
                             data->getCRefOrigin().at(2)
                            );

    importer->SetWholeExtent(   (int)data->getCRefOrigin().at(0), data->getCRefSize().at(0) -1,
                                (int)data->getCRefOrigin().at(1), data->getCRefSize().at(1) -1,
                                (int)data->getCRefOrigin().at(2), data->getCRefSize().at(2) -1
                            );

    // copy WholeExtent to DataExtent
    importer->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    importer->SetImportVoidPointer( data->getBuffer() );


    // used to set correct pixeltype to VtkImage
    // vtkImageImport::InvokeExecuteInformationCallbacks() will call
    // myScalarTypeCallback and waiting for const char * defintion type like "unsigned short"
    // to produce VTK_TYPE
    importer->SetCallbackUserData( data.get() );
    importer->SetScalarTypeCallback( myScalarTypeCallback );

    importer->Update();

    return importer;

}

//------------------------------------------------------------------------------

void configureVTKImageImport( ::vtkImageImport * _pImageImport, ::boost::shared_ptr< ::fwData::Image > _pDataImage )
{

    _pImageImport->SetDataSpacing(  _pDataImage->getSpacing().at(0),
                                    _pDataImage->getSpacing().at(1),
                                    _pDataImage->getSpacing().at(2)
                                );

    _pImageImport->SetDataOrigin(   _pDataImage->getCRefOrigin().at(0),
                                    _pDataImage->getCRefOrigin().at(1),
                                    _pDataImage->getCRefOrigin().at(2)
                                );

    _pImageImport->SetWholeExtent(  (int)_pDataImage->getCRefOrigin().at(0), _pDataImage->getCRefSize().at(0) -1,
                                    (int)_pDataImage->getCRefOrigin().at(1), _pDataImage->getCRefSize().at(1) -1,
                                    (int)_pDataImage->getCRefOrigin().at(2), _pDataImage->getCRefSize().at(2) -1
                                );

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();

    // no copy, no buffer destruction/management
    _pImageImport->SetImportVoidPointer( _pDataImage->getBuffer() );


    // used to set correct pixeltype to VtkImage
    // vtkImageImport::InvokeExecuteInformationCallbacks() will call
    // myScalarTypeCallback and waiting for const char * defintion type like "unsigned short"
    // to produce VTK_TYPE
    _pImageImport->SetCallbackUserData( _pDataImage.get() );
    _pImageImport->SetScalarTypeCallback( myScalarTypeCallback );

}

//-----------------------------------------------------------------------------

vtkPolyData*  toVTKMesh( ::fwData::TriangularMesh::sptr mesh )
{
    vtkPolyData *polygonGrid = vtkPolyData::New();

    if ( mesh && !mesh->points().empty() )
    {
        unsigned int nbPts = mesh->points().size() ;
        assert( nbPts ) ;
        vtkPoints *trianPts = vtkPoints::New();
        for( unsigned int i=0 ; i<nbPts ; ++i )
        {
            float xyz[3] ;
            xyz[0] = mesh->points()[i][0] ;
            xyz[1] = mesh->points()[i][1] ;
            xyz[2] = mesh->points()[i][2] ;
            trianPts->InsertNextPoint(xyz);
        }
        unsigned int nbCells = mesh->cells().size() ;
        assert( nbCells ) ;

        polygonGrid->SetPoints(trianPts);

        vtkIdType typeCell = VTK_TRIANGLE;
        polygonGrid->Allocate(typeCell,nbCells);
        for(unsigned int i=0 ; i<nbCells ; ++i )
        {
            vtkIdType cell[3];
            cell[0] = mesh->cells()[i][0] ;
            cell[1] = mesh->cells()[i][1] ;
            cell[2] = mesh->cells()[i][2] ;
            polygonGrid->InsertNextCell( typeCell, 3, cell );
        }
    }
    else
    {
        SLM_INFO("vtkPolyData build from empty mesh.")
    }

    return polygonGrid;
}

//-----------------------------------------------------------------------------

bool fromVTKMesh( vtkPolyData *polyData, ::fwData::TriangularMesh::sptr triangularMesh)
{
    SLM_TRACE_FUNC();

    bool res = false;

    vtkPoints *trianPts = polyData->GetPoints();

    if (trianPts)
    {
        ::fwData::TriangularMesh::PointContainer pointContainer;
        ::fwData::TriangularMesh::CellContainer cellContainer;

        for (int i=0 ; i< trianPts->GetNumberOfPoints() ; i++)
        {
            std::vector< float > vPoint(3,0);
            vPoint[0] = trianPts->GetPoint(i)[0];
            vPoint[1] = trianPts->GetPoint(i)[1];
            vPoint[2] = trianPts->GetPoint(i)[2];
            pointContainer.push_back(vPoint);
        }
        triangularMesh->points() = pointContainer;

        for (int i=0 ; i< polyData->GetNumberOfCells() ; i++)
        {
            std::vector< int > vCell;
            for (int j=0 ; j < polyData->GetCell(i)->GetNumberOfPoints() ; j++)
            {
                vCell.push_back( polyData->GetCell(i)->GetPointId(j) );
            }
            cellContainer.push_back(vCell);
        }

        triangularMesh->cells() = cellContainer;

        res = true;
    }

    return res;
}

//-----------------------------------------------------------------------------

double computeVolume(  ::boost::shared_ptr< ::fwData::TriangularMesh > _triangularMesh )
{
    ::fwData::TriangularMesh::NewSptr closedMesh;
    //*closedMesh = * _triangularMesh;
    closedMesh->deepCopy(_triangularMesh);

    ::fwMath::closeSurface(closedMesh->points(), closedMesh->cells());

    vtkPolyData*  vtkMeshRaw = toVTKMesh( closedMesh );

    vtkPolyDataNormals* filter = vtkPolyDataNormals::New();
    filter->SetInput(vtkMeshRaw);
    filter->AutoOrientNormalsOn ();
    filter->FlipNormalsOn ();

    vtkMassProperties  *calculator = vtkMassProperties::New();
    calculator->SetInput( filter->GetOutput() );
    calculator->Update();
    double volume =  calculator->GetVolume();
    OSLM_DEBUG("GetVolume : " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected() );
    OSLM_DEBUG("Error : " << (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000);
    if ( (calculator->GetVolume()- fabs(calculator->GetVolumeProjected()))*10000 > calculator->GetVolume() )
    {
        std::stringstream ss;
        ss << "vtkMassProperties::GetVolume() - | vtkMassProperties::GetVolumeProjected() |";
        ss << ">  vtkMassProperties::GetVolume()/10000.0" << std::endl;
        ss << "vtkMassProperties::GetVolume() = " << volume << " vtkMassProperties::GetVolumeProjected = " << calculator->GetVolumeProjected();
        throw (std::out_of_range( ss.str() ));
    }


    calculator->Delete();
    filter->Delete();
    return volume;
}

//-----------------------------------------------------------------------------
//

double computeVolumeWithStencil(  ::boost::shared_ptr< ::fwData::TriangularMesh > _triangularMesh )
{

//  vtkPolyData*  vtkMesh = toVTKMesh( _triangularMesh );
//
//  vtkImageData* vi = vtkImageData::New();
//  vi->SetOrigin( 0,0,0 ); // adjust these to your needs
//  vi->SetSpacing( 0.5, 0.5, 0.5 ); // adjust these to your needs
//  vi->SetDimensions( vtkMesh->GetBounds()[1]*2,  vtkMesh->GetBounds()[3]*2,  vtkMesh->GetBounds()[5]*2 ); // adjust these to your needs
//  vi->SetScalarTypeToUnsignedChar ();
//  vi->AllocateScalars();
//  // outputMesh is of vtkPolyData* type and contains your mesh data
//  vtkPolyDataToImageStencil* pti = vtkPolyDataToImageStencil::New();
//  pti->SetInput( vtkMesh );
//  pti->Update();
//  vtkImageStencil* is = vtkImageStencil::New();
//  is->SetInput( vi );
//  is->SetStencil( pti->GetOutput() );
//  is->ReverseStencilOff();
//  is->SetBackgroundValue(1);
//  is->Update();
//  // is->GetOutput() returns your image data as vtkImageData*
//  return -1;

    vtkPolyData*  vtkMesh = toVTKMesh( _triangularMesh );

    vtkImageData* vi = vtkImageData::New();
    vi->SetOrigin( 0,0,0 ); // adjust these to your needs
    vi->SetSpacing( 0.5, 0.5, 0.5 ); // adjust these to your needs
    vi->SetDimensions( vtkMesh->GetBounds()[1]*2,  vtkMesh->GetBounds()[3]*2,  vtkMesh->GetBounds()[5]*2 ); // adjust these to your needs
    vi->SetScalarTypeToUnsignedChar ();
    vi->AllocateScalars();
    vi->GetPointData()->GetScalars()->FillComponent(0, 1.0);
    // outputMesh is of vtkPolyData* type and contains your mesh data
    vtkPolyDataToImageStencil* pti = vtkPolyDataToImageStencil::New();
    pti->SetInput( vtkMesh );
    //pti->Update(); do not update because outputspacing is not defined, let sub filter set them
//  pti->SetTolerance(0.5);
    vtkImageAccumulate* ac = vtkImageAccumulate::New();
    ac->SetInput( vi );
    ac->SetStencil( pti->GetOutput() );
    ac->ReverseStencilOff();
    ac->Update();

    unsigned long nbVoxel = ac->GetVoxelCount();

    pti->Delete();
    ac->Delete();
    vi->Delete();
    vtkMesh->Delete();

    return nbVoxel;
}

//-----------------------------------------------------------------------------


vtkMatrix4x4 *  toVTKMatrix( ::fwData::TransformationMatrix3D::sptr _transfoMatrix )
{
    vtkMatrix4x4  *matrix = vtkMatrix4x4 ::New();
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            matrix->SetElement(l, c, _transfoMatrix->getCoefficient(l, c));
        }
    }
    return matrix;
}

//-----------------------------------------------------------------------------

bool fromVTKMatrix( vtkMatrix4x4* _matrix, ::fwData::TransformationMatrix3D::sptr _transfoMatrix)
{
    SLM_TRACE_FUNC();
    bool res = true;
    for(int l=0; l<4; l++)
    {
        for(int c=0; c <4; c++)
        {
            _transfoMatrix->setCoefficient(l,c, _matrix->GetElement(l,c));
        }
    }

    return res;
}


//-----------------------------------------------------------------------------

void convertTF2vtkTF(
        ::fwData::TransfertFunction::sptr _pTransfertFunctionSrc ,
        vtkLookupTable * lookupTableDst,
        bool allow_transparency
        )
{
    SLM_TRACE_FUNC();
    //vtkWindowLevelLookupTable * lookupTable = vtkWindowLevelLookupTable::New();

    // Compute center and width
    std::pair< double, ::boost::int32_t > centerAndWidth = _pTransfertFunctionSrc->getCenterWidth();
    double width = centerAndWidth.second;

    // Compute min and max
    typedef ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPCIterator;
    std::pair< TFPCIterator, TFPCIterator > range = _pTransfertFunctionSrc->getTransfertFunctionPoints();
    int min = (*range.first)->getValue();


    // Convert tf points
    //-------------------

    // Init iterator
    TFPCIterator iterTF = range.first;
    TFPCIterator iterTFNext = range.first + 1;
    TFPCIterator end = range.second;

    // Must have point in data tf
    assert( iterTF != end );

    // Init parameters
    double r, g, b, x;
    int    i          = 0;
    double alpha      = 1.0;
    double widthScale = 255.0 / width;
    int    value      = 0;


    // Set first point
    value = ((*iterTF)->getValue() - min) * widthScale;
    const ::fwData::Color::ColorArray & vRGBA0 = (*iterTF)->getColor()->getCRefRGBA();

    if(allow_transparency)
    {
        alpha = vRGBA0[3];
    }
    lookupTableDst->SetTableValue(i, vRGBA0[0], vRGBA0[1], vRGBA0[2], alpha);

    i++;

    ::fwData::Color::ColorType R, G, B, A;
    ::fwData::Color::ColorType deltaR, deltaV, deltaB, deltaA;
    int valueNext, deltaValue;

    while ( iterTFNext != end )
    {
        // First point
        const ::fwData::Color::ColorArray &vRGBA     = (*iterTF)->getColor()->getCRefRGBA();
        // Second point
        const ::fwData::Color::ColorArray &vRGBANext = (*iterTFNext)->getColor()->getCRefRGBA();


        value = ((*iterTF)->getValue() - min) * widthScale;
        valueNext = ((*iterTFNext)->getValue() - min) * widthScale;

        R = vRGBA[0];
        G = vRGBA[1];
        B = vRGBA[2];
        A = vRGBA[3];
        deltaR = vRGBANext[0] - vRGBA[0];
        deltaV = vRGBANext[1] - vRGBA[1];
        deltaB = vRGBANext[2] - vRGBA[2];
        deltaA = vRGBANext[3] - vRGBA[3];
        deltaValue = valueNext - value;

        // Interpolation
        if(allow_transparency)
        {
            while (i <= valueNext)
            {
                x = (double)(i - value) / (double)(deltaValue);
                r     = ( R + (deltaR * x) );
                g     = ( G + (deltaV * x) );
                b     = ( B + (deltaB * x) );
                alpha = ( A + (deltaA * x) );

                lookupTableDst->SetTableValue( i, r, g, b , alpha );
                i++;
            }
        }
        else
        {
            while (i <= valueNext)
            {
                x = (double)(i - value) / (double)(deltaValue);
                r     = ( R + (deltaR * x) );
                g     = ( G + (deltaV * x) );
                b     = ( B + (deltaB * x) );

                lookupTableDst->SetTableValue( i, r, g, b , alpha );
                i++;
            }
        }

        iterTF++;
        iterTFNext++;
    }

    lookupTableDst->SetTableRange( min, min + width );

    lookupTableDst->Build();

}




} // namespace vtkIO