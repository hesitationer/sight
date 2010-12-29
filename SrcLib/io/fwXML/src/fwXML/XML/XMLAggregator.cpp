/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <list>

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/pathDifference.hpp>
#include <fwData/Object.hpp>

#include "fwXML/XML/XMLAggregator.hpp"
#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/XMLHierarchy.hpp"

namespace fwXML
{

XMLAggregator::XMLAggregator() : m_version( XMLAggregator::Version::current() )
{
}

XMLAggregator::~XMLAggregator()
{
}


void XMLAggregator::append( ::boost::weak_ptr< fwTools::Object >  obj )
{
    m_objects.insert(obj);
}




::boost::weak_ptr< fwTools::Object > XMLAggregator::getRootObject()
{
    typedef std::list< ::boost::shared_ptr<fwTools::Object>  >  CandidateRoots;
    CandidateRoots m_roots; //( m_objects.begin(),  m_objects.end() );
    for ( Elements::iterator o = m_objects.begin(); o != m_objects.end(); ++o  )
    {
        m_roots.push_back( (*o).lock() );
    }

    // for each object search if a child is in m_objects ==> TRUE : the child is not a root : FALSE => cannot conclude ( root or child of another parent)
    for ( Elements::iterator o = m_objects.begin(); o != m_objects.end(); ++o  )
    {
        ::boost::shared_ptr< fwTools::Object>  dobj = (*o).lock();
        for ( fwTools::Object::ChildContainer::iterator c = dobj->children().begin() ; c !=  dobj->children().end(); ++c  )
        {
            assert( ::boost::dynamic_pointer_cast< fwTools::Object >( *c ) );
            ::boost::shared_ptr<fwTools::Object> child = ::boost::dynamic_pointer_cast< fwTools::Object >(*c);
            if ( m_objects.find( child ) != m_objects.end() ) // child is in m_objects => it is not a root
            {
                m_roots.remove(child);
            }
        }

    }

    assert ( m_roots.size() == 1 );

    return m_roots.front();
}


std::string XMLAggregator::getXMLIncludePathValue(::boost::weak_ptr< fwTools::Object > obj)
{
    using boost::filesystem::path;
    path objFolderPath = XMLHierarchy::getDefault()->mapObjectAggregator()[obj]->localFolder();
    path thisAggregatorFolderPath = this->localFolder();

    path relativePath = fwTools::getPathDifference(thisAggregatorFolderPath,objFolderPath);

    return ( relativePath / XMLHierarchy::getDefault()->mapObjectAggregator()[obj]->getFullFilename() ).string();
}


// return an xi::include node

xmlNodePtr XMLAggregator::getXMLInclude(::boost::weak_ptr< fwTools::Object > obj)
{
    assert( !obj.expired() );
    assert ( m_objects.find( obj) ==  m_objects.end() ); // " xi::include must be performed only if obj is not in the same aggregator

    ::boost::shared_ptr<XMLAggregator> otherAggregator =  XMLHierarchy::getDefault()->mapObjectAggregator()[obj];

    if (  otherAggregator )
    {
        xmlNodePtr node = xmlNewNode(NULL, xmlStrdup( BAD_CAST "xi:include" ));
        xmlNewProp( node, BAD_CAST "xmlns:xi" , xmlStrdup( BAD_CAST  """http://www.w3.org/2003/XInclude""" ));
        std::string xmlrelativeFilename = getXMLIncludePathValue( obj );
        xmlNewProp( node, BAD_CAST "href" , xmlStrdup( BAD_CAST  xmlrelativeFilename.c_str() ));
        return node;
    }
    else
    {
        // other aggregator not yet constructed   !!! FIXME set an exception here when design terminated
        return xmlNewNode(NULL, xmlStrdup( BAD_CAST (   obj.lock()->getLeafClassname() + "_is_In_Another_NOTYETCreatedAggregator"  ).c_str() ) );
    }



}





// 1 create map Object <-> XML correspondance
// 2 for each object child c :
//   if c in same aggregator get its xml
//   if c not in the same aggregator set an include with correct path
// 3 verify that we have an xml tree and not a forest
// 4 set root and return document




xmlDocPtr XMLAggregator::getXMLDoc()
{
    // Phase 1
    for ( Elements::iterator o = m_objects.begin(); o != m_objects.end(); ++o  )
    {
        ::boost::shared_ptr< fwTools::Object> dobj = (*o).lock();
        ::boost::shared_ptr< fwXML::XMLTranslator > translator = fwTools::ClassFactoryRegistry::create< fwXML::XMLTranslator  >(  dobj->getClassname()  );
        //assert(translator);
        if (translator==NULL)
        {
            translator = ::boost::shared_ptr< fwXML::XMLTranslator >( new TrivialXMLTranslator );
            OSLM_ERROR( "no XMLTranslator for" << dobj->getClassname() << "(" << dobj << ") use Trivial XML Translator");
        }

        xmlNodePtr nodeXML = translator->getXMLFrom(dobj);
        XMLHierarchy::getDefault()->mapObjectXMLNode()[dobj] = nodeXML; // FIXME what to do with oldest ? perharps use a local info ?
    }


    // Phase 2
    for ( Elements::iterator o = m_objects.begin(); o != m_objects.end(); ++o  )
    {
//      assert( ::boost::dynamic_pointer_cast< fwTools::Object >( (*o).lock() ) );
//      ::boost::shared_ptr<fwTools::Object> dobj = ::boost::dynamic_pointer_cast< fwTools::Object >( (*o).lock() );
        ::boost::shared_ptr<fwTools::Object> dobj = (*o).lock() ;

        for ( fwTools::Object::ChildContainer::iterator c = dobj->children().begin() ; c !=  dobj->children().end(); ++c  )
        {
            assert( ::boost::dynamic_pointer_cast< fwTools::Object >( *c ) );
            ::boost::weak_ptr<fwTools::Object> child = ::boost::dynamic_pointer_cast< fwTools::Object >( *c );
            // in same aggregator ?
            if ( m_objects.find( child ) != m_objects.end() )
            {
                xmlNodePtr parent = XMLHierarchy::getDefault()->mapObjectXMLNode()[dobj];
                xmlNodePtr son    = XMLHierarchy::getDefault()->mapObjectXMLNode()[child];
                xmlAddChild( parent , son );
            }
            else
            {
                //in another Aggregator !!!
                xmlNodePtr parent = XMLHierarchy::getDefault()->mapObjectXMLNode()[dobj];
                xmlAddChild( parent ,   getXMLInclude(child)   );


            }
        }
    }

    // Phase 3;
    ::boost::weak_ptr<fwTools::Object> root = getRootObject();


    // set nodes
    xmlDocPtr doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);
    xmlNodePtr archive = xmlNewNode(NULL, BAD_CAST "archive");
    xmlNodePtr   protocol = xmlNewNode(NULL, BAD_CAST "protocol");
    xmlNodePtr   content = xmlNewNode(NULL, BAD_CAST "content");

    xmlDocSetRootElement ( doc, archive );

    //set protocol node
    xmlAddChild( archive , protocol );
    XMLTH::addProp( protocol, "version", m_version.string() );

    //set content node
    xmlAddChild( archive , content );
    xmlAddChild(  content,  XMLHierarchy::getDefault()->mapObjectXMLNode()[root] );
    return doc;
}






}
