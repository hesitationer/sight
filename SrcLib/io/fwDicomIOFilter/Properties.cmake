set( NAME fwDicomIOFilter )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwTools
    fwData
    fwMath
    fwMedData
    fwLog
)
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    dcmtk/3.6.3@sight/stable
)