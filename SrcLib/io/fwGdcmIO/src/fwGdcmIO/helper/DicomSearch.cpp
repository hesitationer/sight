/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomSearch.hpp"

#include <fwCore/base.hpp>

#include <fwJobs/Observer.hpp>

#include <boost/assign/list_of.hpp>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

bool isDICOM(const ::boost::filesystem::path& filepath)
{
    ::boost::filesystem::ifstream ifs( filepath, std::ios::binary );
    ifs.seekg(128);
    char DICM[5] = {0};
    ifs.read(DICM, 4);
    ifs.close();
    return strcmp(DICM, "DICM") == 0;
}

//------------------------------------------------------------------------------

void DicomSearch::searchRecursively(const ::boost::filesystem::path& dirPath,
                                    std::vector< ::boost::filesystem::path >& dicomFiles,
                                    bool checkIsDicom,
                                    const ::fwJobs::Observer::sptr& readerObserver)
{
    std::vector< ::boost::filesystem::path > fileVect;
    checkFilenameExtension(dirPath, fileVect, readerObserver);

    if(checkIsDicom)
    {
        if(readerObserver)
        {
            readerObserver->setTotalWorkUnits(fileVect.size());
        }

        std::uint64_t progress = 0;
        for(auto file : fileVect)
        {
            if(readerObserver)
            {
                readerObserver->doneWork(++progress);

                if(readerObserver->cancelRequested())
                {
                    dicomFiles.clear();
                    break;
                }
            }

            bool isDicom = isDICOM(file);
            if(isDicom)
            {
                dicomFiles.push_back( file );
            }
            SLM_WARN_IF("Failed to read: " + file.string(), !isDicom);
        }
    }
    else
    {
        dicomFiles = fileVect;
    }
}

//------------------------------------------------------------------------------

void DicomSearch::checkFilenameExtension(const ::boost::filesystem::path& dirPath,
                                         std::vector< ::boost::filesystem::path >& dicomFiles,
                                         const ::fwJobs::Observer::sptr& fileLookupObserver)
{
    dicomFiles.clear();

    std::set<std::string> extensions = ::boost::assign::list_of(".jpg")(".jpeg")(".htm")(".html")(".txt")(".xml")
                                           (".stm")(".str")(".lst")(".ifo")(".pdf")(".gif")
                                           (".png")(".exe")(".zip")(".gz")(".dir")(".dll")(".inf")
                                           (".DS_Store");

    for(::boost::filesystem::recursive_directory_iterator it(dirPath);
        it != ::boost::filesystem::recursive_directory_iterator(); ++it)
    {
        if(fileLookupObserver && fileLookupObserver->cancelRequested())
        {
            dicomFiles.clear();
            break;
        }

        if(!::boost::filesystem::is_directory(*it))
        {
            auto path       = it->path();
            std::string ext = path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

            if(extensions.find(ext) == extensions.end())
            {
                std::string stem = path.stem().string();
                std::transform(stem.begin(), stem.end(), stem.begin(), tolower);

                if(stem != "dicomdir")
                {
                    dicomFiles.push_back(path.string());
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace helper
} //namespace fwGdcmIO

