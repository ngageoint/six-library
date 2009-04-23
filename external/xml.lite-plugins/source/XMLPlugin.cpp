/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; if not, If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nitf.h>

#if 1
#include <import/coda/xml/lite.h>
#include <import/coda/io.h>
#include <import/coda/sys.h>
#include <import/coda/str.h>
#include <import/coda/except.h>
using namespace coda;
#else
#include <import/xml/lite.h>
#include <import/io.h>
#include <import/sys.h>
#include <import/str.h>
#include <import/except.h>
#endif


NITF_CXX_GUARD

struct PrivateData
{
    xml::lite::Document* doc;
    
    /* only using this for its HashTable */
    nitf_TREPrivateData *priv;
    
    std::string id;

    static void destruct(PrivateData** data);
    static PrivateData* construct(nitf_Error* error);
    static PrivateData* clone(PrivateData *source, 
                              nitf_Error* error);
    
    
};

struct EnumeratorImpl
{
    /*!
     * Fulfills interface for determining if there is
     * another field
     *
     */
    static NITF_BOOL hasNext(nitf_TREEnumerator** it);
    /*!
     * Fulfills interface for getting the next TRE
     * field (pair)
     */
    static nitf_Pair* next(nitf_TREEnumerator* it, 
                           nitf_Error* error);
};


/*!
 * This plugin is basically just glorified C.  I use all
 * static methods, so that its easy to bind this to the C
 * TRE.  There is not much need to get more complicated, so
 * I refrain.  
 *
 * There is an inner class for Data, which is private data
 * that gets passed around internally.
 *
 * There is also an inner class for our TRE enumerator 
 * implementation
 *
 * Both of these also use static methods
 *
 */
struct XMLHandler
{


    /*!
     *  Walk the sub-nodes of this current element, adding
     *  TRE fields where we find a leaf or an attribute.
     *
     */
    static NITF_BOOL _put(xml::lite::Element* current, 
                          nitf_TRE* tre,
                          std::string currentName,
                          nitf_Error* error);

    /*!
     *  Walk the name path and expand the DOM tree to include
     *  elements that do not yet exist
     */
    static void _expandDOM(xml::lite::Document* doc,
                           std::string name, 
                           std::string value);
        
    
    /*!
     * Insert a field with the tag name and dataStr
     * value into the nitf_TRE.
     */
    static NITF_BOOL _insertField(nitf_TRE* tre, 
                                  std::string tag, 
                                  std::string dataStr,
                                  nitf_Error* error);

    /*!
     * This fulfills the TRE Handler's interface for
     * reading a TRE in from the io.  
     *
     * \param ioIface The io interface (abstracted for 2.5)
     * \param length The number of bytes total for this TRE
     * \param tre The TRE that we will read from
     * \param record The Record, in case we need it
     * \param error An error to populate on failure
     * \return NITF_SUCCESS on success, NITF_FAILURE on failure
     *
     */
    static NITF_BOOL readXML(nitf_IOInterface* ioIface,
                             nitf_Uint32 length,
                             nitf_TRE* tre,
                             struct _nitf_Record* record,
                             nitf_Error* error);

    /*!
     *  Fulfills the TRE Handler's interface for writing
     *  a TRE to the io.
     *
     * \param ioIface The io interface (abstracted for 2.5)
     * \param tre The TRE that we will read from
     * \param record The Record, in case we need it
     * \param error An error to populate on failure
     * \return NITF_SUCCESS on success, NITF_FAILURE on failure
     *
     */
    static NITF_BOOL writeXML(nitf_IOInterface* ioIface, 
                              nitf_TRE* tre, 
                              struct _nitf_Record* record, 
                              nitf_Error* error);


    /*!
     * This function gets an ID.  Since our plugin is handling
     * multiple TREs already, it would be inconvenient to
     * fully implement this, since it is only required if there
     * are multiple handlers for the same plugin (e.g., different
     * versions exist)
     */
    static const char* getID(nitf_TRE* tre);
    
    /*!
     * Function initializes our TRE handler.  This
     * method is public to TREHandlers and is called
     * when a TRE is constructed by a developer from the API
     *
     * \param tre The TRE
     * \param id Should be NULL, since we arent supporting an ID here
     * \param error An error to populate on failure
     * \return NITF_SUCCESS on success, NITF_FAILURE on failure
     */
    static NITF_BOOL init(nitf_TRE* tre, 
                          const char* id, 
                          nitf_Error * error);

    /*!
     * Fulfills interface for destroying a TRE
     *
     */
    static void destruct(nitf_TRE *tre);


    /*!
     * Fulfills interface for clone
     */
    static NITF_BOOL clone(nitf_TRE *source,
                           nitf_TRE *tre,
                           nitf_Error *error);

    /*!
     *  Fulfills interface for getting a TRE field
     *
     */
    static nitf_Field* getField(nitf_TRE* tre, const char* tag);


    /*!
     * Fulfills interface for setting a TRE field
     *
     */
    static NITF_BOOL setField(nitf_TRE* tre, 
                              const char* tag, 
                              NITF_DATA* data, 
                              size_t dataLength, 
                              nitf_Error* error);

    /*!
     *  Fulfills interface for finding beginning of this
     *  TRE
     */
    static nitf_TREEnumerator* begin(nitf_TRE* tre,
                                     nitf_Error* error);
    /*!
     * Get the current size of a TRE.  For now, this
     * will print the DOM into a string and return the 
     * string length
     *
     */
    static int getCurrentSize(nitf_TRE* tre, nitf_Error* error);


    /*!
     * Literally stolen from DefaultTRE.c
     */
    static nitf_List* find(nitf_TRE* tre,
                           const char* pattern,
                           nitf_Error* error);


};

PrivateData* PrivateData::construct(nitf_Error* error)
{
    PrivateData* data = new PrivateData();
    if (!data) 
    { 
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), 
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }
    /*
     * For now, I will implement the doc as a DOM, but
     * this is not necessary, and we will probably do something
     * more efficient in the future
     */
    data->doc = new xml::lite::Document();
    data->priv = nitf_TREPrivateData_construct(error);
    if (!data->priv)
    {
        PrivateData::destruct(&data);
        return NULL;
    }
    return data;
    

}

void PrivateData::destruct(PrivateData** data)
{
    
    if (data && (*data))
    {
        delete (*data)->doc;
        
        if ((*data)->priv)
            nitf_TREPrivateData_destruct(&(*data)->priv);
        
        delete *data;
        *data = NULL;
    }
}
PrivateData* PrivateData::clone(PrivateData *source, 
                                nitf_Error* error)
{
    PrivateData* data = PrivateData::construct(error);
    if (!data)
        return NULL;

    data->doc = NULL;
    
    if (source->doc)
    {
        data->doc = new xml::lite::Document();
        // TODO: make it easier to do this in xml.lite


        if (source->doc->getRootElement())
        {
            xml::lite::Element* element = new xml::lite::Element();

            element->clone(*source->doc->getRootElement());
            source->doc->setRootElement(element);
        }

        if (!data->doc)
        {
            PrivateData::destruct(&data);
            nitf_Error_init(error, "Unable to copy the XML document",
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
    }
    return data;
}

NITF_BOOL XMLHandler::_put(xml::lite::Element* current, 
                           nitf_TRE* tre,
                           std::string currentName,
                           nitf_Error* error)
{
    // If we  have no children, we need to insert the TRE -- we are a leaf
    std::vector<xml::lite::Element*> children = current->getChildren();
    // If there is only one element
    if (children.size() == 0)
    {
        std::string tag;

        // Get the attribute nodes out as well
        xml::lite::Attributes atts = current->getAttributes();
        
        // For each attribute, give it a name /path/to/current/@name
        for (int i = 0; i < atts.getLength(); i++)
        {
            tag = currentName + "/@" + atts.getLocalName(i);
            std::string data = atts.getValue(i);
            if (!XMLHandler::_insertField(tre, tag, data, error))
                return NITF_FAILURE;
        }
        
        // Internally, we always keep around this tag with an index
        tag = currentName;
        if (!XMLHandler::_insertField(tre, 
                                      tag, 
                                      current->getCharacterData(), 
                                      error))
            return NITF_FAILURE;
        
        return NITF_SUCCESS;
    }
    
    std::string lastName;
    int count = 0;
    // Otherwise we will walk each
    for (unsigned int i = 0; i < children.size(); i++)
    {
        // Get the child name
        
        std::string localName = children[i]->getLocalName();
        if (localName != lastName)
            count = 0;
        else
            count++;
        lastName = localName;

        std::string childName = 
            FmtX("%s/%s[%d]", currentName.c_str(),
                 localName.c_str(), count);
        
        // Convert it to a nitf_Field
        if (!XMLHandler::_put(children[i], tre, childName, error))
            return NITF_FAILURE;
    }
    // Good work
    return NITF_SUCCESS;
}

// Read our TRE in from XML
NITF_BOOL XMLHandler::readXML(nitf_IOInterface* ioIface,
                              nitf_Uint32 length,
                              nitf_TRE* tre,
                              struct _nitf_Record* record,
                              nitf_Error* error)
{
    PrivateData* treData = NULL;
    try
    {
        if (!tre)
            throw except::Exception("Invalid TRE data");


        
        treData = PrivateData::construct(error);
        if (!treData)
            throw except::Exception("Failed to create TRE data");

        /*
         *  This would be ideal:
         *  io::InputStream* inputStream = (io::InputStream*)ioIface;
         */
        char* stream = new char[length];
        if (!nitf_IOInterface_read(ioIface, stream, length, error))
        {
            delete [] stream;
        }
        io::ByteStream inputStream;
        inputStream.write(stream, length);
        delete [] stream;
        
        xml::lite::MinidomParser xmlParser;
        
        // Read just the XML content
        xmlParser.parse(inputStream, length);
        treData->doc = new xml::lite::Document();
        xml::lite::Element* root = new xml::lite::Element();
        root->clone(*xmlParser.getDocument()->getRootElement());
        treData->doc->setRootElement( root );
        
        tre->priv = treData;

        if (!root)
            throw except::Exception("Invalid XML root element");
        
        
        if (!XMLHandler::_put(root, tre, root->getLocalName(), error))
            throw except::Exception("Failed to put DOM");

        return NITF_SUCCESS;
    }
    
    catch (except::Exception& ex)
    {
        std::cout << ex.getMessage() << std::endl;
        if (treData) PrivateData::destruct(&treData);
        
        nitf_Error_init(error, ex.getMessage().c_str(), NITF_CTXT, 
                        NITF_ERR_INVALID_OBJECT);

        return NITF_FAILURE;
    }
}

// Just do nothing here, we have only one handler for all TREs
const char* XMLHandler::getID(nitf_TRE* tre)
{
    PrivateData * treData = (PrivateData*)tre->priv;
    return treData->id.c_str();
}

// Init our TRE from scratch -- through TRE_construct
NITF_BOOL XMLHandler::init(nitf_TRE* tre, 
                           const char* id, 
                           nitf_Error * error)
{
    PrivateData* treData = NULL;
    if (!tre) { return NITF_FAILURE; } 

    if (id == NULL)
    {
        nitf_Error_init(error, "You must declare a root node!", NITF_CTXT, 
                        NITF_ERR_INVALID_OBJECT);

        return NITF_FAILURE;
    }
    
    treData = PrivateData::construct(error);
    if (!treData) return NITF_FAILURE;

    
    // Dont forget the root is NULL
    treData->doc = new xml::lite::Document();

    xml::lite::Element * root = new xml::lite::Element();

    treData->id = id;

    root->setLocalName(id);
    treData->doc->setRootElement(root);


    tre->priv = treData;
    
    return NITF_SUCCESS;
}

// For now, write the dom out
NITF_BOOL XMLHandler::writeXML(nitf_IOInterface* ioIface, 
                               nitf_TRE* tre, 
                               struct _nitf_Record* record, 
                               nitf_Error* error)
{
    PrivateData * treData = (PrivateData*)tre->priv;
    
    /*
     * This would be ideal
     * io::OutputStream* outputStream = (io::OutputStream*)ioIface;
     */
    io::ByteStream outputStream;
    if (!treData->doc)
    {
        return NITF_FAILURE;
    }
    xml::lite::Element* root = treData->doc->getRootElement();
    
    if (root == NULL)
        return NITF_FAILURE;
    
    root->print(outputStream);
    if (!nitf_IOInterface_write(ioIface, 
                                outputStream.stream().str().c_str(), 
                                outputStream.stream().str().length(),
                                error))
        return NITF_FAILURE;
    return NITF_SUCCESS;
}

/*
 *  Currently, in order to make this work, I find it easiest to just
 *  rewrite the DOM on the fly.
 *
 *  Once I have done that, the length is going to be the current DOM
 *  length.
 *
 */
int XMLHandler::getCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
    
    PrivateData * treData = (PrivateData*)tre->priv;
    
    if (!treData->doc)
    {
        return NITF_FAILURE;
    }
    xml::lite::Element* root = treData->doc->getRootElement();
    assert(root);
    io::ByteStream bs;
    root->print(bs);
    return bs.stream().str().length();
}

void XMLHandler::_expandDOM(xml::lite::Document* doc,
                            std::string name, 
                            std::string value)
{
    // First, we split on ] boundary.
    std::vector<std::string> toks = str::Tokenizer(name, "/");

    // This may be NULL if the tree is empty
    xml::lite::Element* current = doc->getRootElement();
    assert(current);
    // Figure out how many keys we got
    size_t numToks = toks.size();
    
    std::string currentName;

    // Freak out if we got none
    if (!numToks)
        throw except::Exception(
            Ctxt(
                FmtX("Need at least one token")
                )
            );
    
    // Walk through and fill any elements that are not yet filled
    unsigned int index = 0;


    // For each token
    for (unsigned int i = 1; i < numToks; i++)
    {
        // Strip the [ which accompanies the already stripped ]
        std::vector<std::string> v = str::Tokenizer(toks[i], "[");

        // That means the first token inside here is the base name
        currentName = v[0];

        if (v.size() == 1)
        {
            // Flip out if braces didnt match up
            if (i != numToks - 1)
            {
                throw except::Exception(
                    Ctxt(
                        FmtX("Ran out of tokens at %d", i)
                        )
                    );
            }
            
            if (currentName[0] != '@')
            {
                std::cout << "Current name " << currentName 
                          << " is not indexed.  " 
                          << "Assuming you want first element" 
                          << std::endl;
                index = 0;
            }
            else
            {
                // We have our final attribute and current is pointing
                // to our element
                assert(current);
                std::string attName = currentName.substr(1);
                // Figure out if it exists
                int attIndex = 
                    current->getAttributes().getIndex(attName);
                if (attIndex == -1)
                {

                    xml::lite::AttributeNode attNode;
                    attNode.setLocalName(currentName.substr(1));
                    attNode.setValue(value);
                    
                    current->getAttributes().add(attNode);
                }
                else
                {
                    current->getAttributes().getNode(attIndex).setValue(value);

                }
                return;
            }
            
        }
        else
        {
            std::string strIndex = v[1].substr(0, v[1].find("]"));
            index = str::toType<int>(strIndex);
        }
        
        // If there is only one element, we are at the end of our walk
        std::vector<xml::lite::Element*> children;

      
        current->getElementsByTagName(currentName, children);
        
        xml::lite::Element* lastChild = NULL;
        
        // Expand our offerings
        while (children.size() <= index)
        {
            lastChild = new xml::lite::Element();
            lastChild->setLocalName(currentName);
            current->addChild( lastChild );
            children.clear();
            // We need this again, since we have reinserted
            current->getElementsByTagName(currentName, children);
        }
        // Now we are on the index we wanted in the first place
            current = children[index];
    }
    
    // If we got here we must have character data
    current->setCharacterData(value);
    
}

/*
 * Check if a TRE is in the hash table already, and if so
 * Resize.  If not we need to add it.  Note this only happens
 * on leaf nodes
 */
NITF_BOOL XMLHandler::_insertField(nitf_TRE* tre, 
                                   std::string tag, 
                                   std::string dataStr,
                                   nitf_Error* error)
{
    
    //  And now check if its in the hash table
    if (!nitf_HashTable_exists(((PrivateData*)tre->priv)->priv->hash, 
                               tag.c_str()))
    {
        //  If not, make a new entry
        nitf_Field* field = 
            nitf_Field_construct(dataStr.size(), NITF_BCS_A, error);

        field->resizable = 1;
        if (!field)
            return NITF_FAILURE;
        
        //  And set it
        nitf_Field_setString(field, dataStr.c_str(), error);
        
        //  And insert it
        if (!nitf_HashTable_insert(((PrivateData*)tre->priv)->priv->hash,
                                   tag.c_str(), field, error))
        {
            nitf_Field_destruct(&field);
            return NITF_FAILURE;
        }
    }
    else
    {
        //  Otherwise, we just need to get it and reset the value 
        nitf_Pair* pair = nitf_HashTable_find(
            ((PrivateData*)tre->priv)->priv->hash, tag.c_str());
        
        assert(pair);
        
        nitf_Field* field = (nitf_Field*)pair->data;
        nitf_Field_resizeField(field, dataStr.length(), error);
        nitf_Field_setString(field, dataStr.c_str(), error);

    }
    return NITF_SUCCESS;
    
}

/*
 *  This function is reponsible for setting a field,
 *  fulfilling the required TRE interface.  In order
 *  to effectively set this up, we sync the DOM and
 *  the nitf_HashTable with the same data.  This 
 *  allows the end-user to have the linear mapped
 *  names that create a typical TRE experience while
 *  keeping the DOM up-to-date
 *
 */
NITF_BOOL XMLHandler::setField(nitf_TRE* tre, 
                               const char* tag, 
                               NITF_DATA* data, 
                               size_t dataLength, 
                               nitf_Error* error)
{
    
    PrivateData* treData = (PrivateData*)tre->priv;
    
    // Now we have the tag, and we have the data
    std::string tagStr(tag);    
    std::string dataStr((const char*)data, dataLength);
    

    //xml::lite::Element* root = treData->doc->getRootElement();
    /*
     *
     * First expand the DOM so that a new element is there 
     * for each subdirectory if it was not before
     */

    try
    {
        XMLHandler::_expandDOM(treData->doc, tagStr, dataStr);
    }
    catch (except::Exception& ex)
    {
        nitf_Error_init(error, ex.getMessage().c_str(), NITF_CTXT, 
                        NITF_ERR_INVALID_OBJECT);

        return NITF_FAILURE;
    }
    /*!
     * Then go ahead and insert the field
     */
    return XMLHandler::_insertField(tre, tagStr, dataStr, error);
}



/*
 *  Check if there is another field
 */
NITF_BOOL EnumeratorImpl::hasNext(nitf_TREEnumerator** it)
{
    nitf_HashTableIterator* hashIt = (nitf_HashTableIterator*)(*it)->data;
    
    nitf_HashTableIterator end = nitf_HashTable_end( hashIt->hash );
    
    if (nitf_HashTableIterator_equals(&end, hashIt))
    {
        NITF_FREE(hashIt);
        NITF_FREE( (*it) );
        *it = NULL;
        return NITF_FAILURE;
    }
    
    return NITF_SUCCESS;
}


/*
 *  Get the next field
 */
nitf_Pair* EnumeratorImpl::next(nitf_TREEnumerator* it, 
                                nitf_Error* error)
{
    nitf_HashTableIterator* hashIter = (nitf_HashTableIterator*)it->data;
    
    nitf_Pair* data = nitf_HashTableIterator_get(hashIter);
    nitf_HashTableIterator_increment(hashIter);
    return data;
}

/*
 * Get the enumerator to the beginning of the TRE
 * Iteration is easy, we are using a hash table iterator underneath 
 * Since I make no effort to keep this stuff ordered, its going to
 * be hash-ordered (i.e., not ordered at all)
 */
nitf_TREEnumerator* XMLHandler::begin(nitf_TRE* tre,
                                      nitf_Error* error)
{
    
    nitf_TREEnumerator* it = 
        (nitf_TREEnumerator*)NITF_MALLOC(sizeof(nitf_TREEnumerator));
    
    nitf_HashTableIterator hashEnd = nitf_HashTable_end(
        ((PrivateData*)tre->priv)->priv->hash);
    nitf_HashTableIterator* hashIter = 
        (nitf_HashTableIterator*)NITF_MALLOC(sizeof(nitf_HashTableIterator));
    
    *hashIter = nitf_HashTable_begin(((PrivateData*)tre->priv)->priv->hash);
    
    if (nitf_HashTableIterator_equals(&hashEnd, hashIter))
    {
        NITF_FREE(hashIter);
        NITF_FREE(it);
        return NULL;
    }
    
    it->data = hashIter;
    it->hasNext = EnumeratorImpl::hasNext;
    it->next = EnumeratorImpl::next;
    
    return it;
}

/*
 *  When we are done, its time to clean up shop.
 *
 */
void XMLHandler::destruct(nitf_TRE *tre)
{
    if (tre && tre->priv)
    {
        PrivateData::destruct((PrivateData**)&tre->priv);
    }
}

/*
 *  \TODO: Im not sure if this has been tested
 *
 */
NITF_BOOL XMLHandler::clone(nitf_TRE *source,
                            nitf_TRE *tre,
                            nitf_Error *error)
{
    PrivateData *sourcePriv = NULL;
    PrivateData *trePriv = NULL;
    
    if (!tre || !source || !source->priv)
        return NITF_FAILURE;
    
    sourcePriv = (PrivateData*)source->priv;
    trePriv = (PrivateData*)tre->priv;
    
    /* this clones the hash */
    if (!(trePriv = PrivateData::clone(sourcePriv, error)))
        return NITF_FAILURE;
    
    return NITF_SUCCESS;
}

/*
 *  Get the field, if it exists as a nitf_Field.  Otherwise,
 *  just return NULL
 *
 */
nitf_Field* XMLHandler::getField(nitf_TRE* tre, const char* tag)
{
    nitf_Pair* pair = nitf_HashTable_find(
        ((PrivateData*)tre->priv)->priv->hash, tag);
    if (!pair) return NULL;
    return (nitf_Field*)pair->data;
}


nitf_List* XMLHandler::find(nitf_TRE* tre,
                            const char* pattern,
                            nitf_Error* error)
{
    nitf_List* list;
    
    nitf_HashTableIterator it = nitf_HashTable_begin(
        ((nitf_TREPrivateData*)tre->priv)->hash);
    nitf_HashTableIterator end = nitf_HashTable_end(
        ((nitf_TREPrivateData*)tre->priv)->hash);
    
    list = nitf_List_construct(error);
    if (!list) return NULL;
    
    while (nitf_HashTableIterator_notEqualTo(&it, &end))
    {
        nitf_Pair* pair = nitf_HashTableIterator_get(&it);
        
    	if (strstr(pair->key, pattern))
    	{
    	    /* Should check this, I suppose */
    	    nitf_List_pushBack(list, pair, error);
    	}
    	nitf_HashTableIterator_increment(&it);
    }
    
    return list;
}

/*
 *  This is the descriptor for our XML handler
 */
static nitf_TREHandler gHandler = 
{
    XMLHandler::init,
    XMLHandler::getID,
    XMLHandler::readXML,
    XMLHandler::setField,
    XMLHandler::getField,
    XMLHandler::find,
    XMLHandler::writeXML,
    XMLHandler::begin,
    XMLHandler::getCurrentSize,
    XMLHandler::clone,
    XMLHandler::destruct,
    NULL
};


static char *ident[] = { NITF_PLUGIN_TRE_KEY, "XMLTRE", NULL };

/*
char** gIdent;
*/
NITFAPI(char**) XMLPlugin_init(nitf_Error* error)
{
    return ident;
}

/*{
    
    const char* treNames = getenv("XML_TRES");
    
    std::string treNamesStr;
    
    if (!treNames)
    {
        treNamesStr = "XMLTRE";
    }
    else
    {
        treNamesStr = treNames;
    }

    std::vector<std::string> tres = str::Tokenizer(treNamesStr, ";");
    size_t totalSize = tres.size() + 1;
    gIdent = new char*[totalSize + 1];


    for (unsigned int i = 0; i < totalSize; i++)
    {
        gIdent[i] = new char[NITF_MAX_PATH];
    }
    strcpy(gIdent[0], NITF_PLUGIN_TRE_KEY);
    for (unsigned int i = 0; i < totalSize-1; i++)
    {
        strcpy(gIdent[i+1], tres[i].c_str());
    }
    gIdent[totalSize] = NULL;
    return gIdent;
} 
*/
NITFAPI(nitf_TREHandler*) XMLTRE_handler(nitf_Error* error)
{
    return &gHandler;
}

NITFAPI(void) XMLTRE_cleanup(void)
{
/*    if (gIdent != NULL)
    {
        for (int i = 0; gIdent[i] != NULL; i++)
        {
            delete gIdent[i];
        }
        delete [] gIdent;
    }
*/  
}

NITF_CXX_ENDGUARD

