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


#include <nitf/IOHandle.h>
#include <nitf/TREUtils.h>
#include <nitf/Record.h>

/* This example uses libxml2.  It requires that, which in turn requires iconv and zlib */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

NITF_CXX_GUARD


static char *ident[] = { NITF_PLUGIN_TRE_KEY, "XMLTRE", NULL }; 
  
typedef struct _XMLTREData
{
	/* This contains the actual document */
	xmlDoc* doc;
	/* This is the size of our memory buffer.  Its resized whenever memory is accessed */
	/* Tis allows us to not specify XML_BUFFER_ALLOC_EXACT */
	//int memorySize;
	/* This contains the memory -- it will get allocated when getCurrentSize() is called */
	/* Or whenever data is written, after which it may be cleared */
	//xmlBuffer* memory;
	char* memory;
	int memorySize;
	
	/* This field is only 1 when setField() is called.  It indicates that the current size */
	/* or write functions must update memory */
	int dirty;
} XMLTREData;

NITFPRIV(XMLTREData*) XMLTREData_construct(nitf_Error* error)
{
    XMLTREData* data = (XMLTREData*)malloc(sizeof(XMLTREData));
    if (!data) 
    { 
	nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
	return NULL;
    }
    data->memorySize = -1;
    data->memory = NULL;
    data->dirty = 0;
    return data;
}



/*
 *  This function retrieves the text data under a node.
 *  TODO: Currently uses a fixed buffer size
 *  
 *
 */
NITFPRIV(char*) getText(xmlNode* node)
{
	xmlNode * current = NULL;
	char* data = NULL;
	char buf[1024] = "";
	for (current = node; current; current = current->next)
	{
		if (current->type == XML_TEXT_NODE)
		{
			strcat(buf, (char*)current->content);
		}	
	}
	if (!strlen(buf))
		return NULL;

	data = (char*)malloc(strlen(buf) + 1);
	
	strcpy(data, buf);
	return data;
}

/*
 *  Organizes data out of the DOM and puts it in the hash
 *
 *
 */
NITFPRIV(NITF_BOOL) putElementsInTRE(xmlNode* node, nitf_TRE* tre, const char* prepend, nitf_Error* error)
{
    xmlNode * current = NULL;
    
    int depth = 0;
    char lastName[512] = "";
    for (current = node; current; current = current->next)
    {
	
	if (current->type == XML_ELEMENT_NODE)
	{
	    
	    char name[512];
	    nitf_Field* field;
	    char* text;
	    
	    if (strcmp((char*)current->name, lastName) == 0)
	    {
		depth++;
	    }
	    else depth = 0;
	    
	    strcpy(lastName, (char*)current->name);
	    sprintf(name, "%s/%s[%d]", prepend, (char*)current->name, depth);
	    
	    putElementsInTRE(current->children, tre, name, error);
	    
	    text = getText(current->children);
	    
	    
	    
	    if (text != NULL)
	    {
		
		
		field = nitf_Field_construct(strlen(text), NITF_BCS_A, error);
		nitf_Field_setString(field, text, error);
		//printf("Name: <%s>: %s\n", name, text);			
		if (!nitf_HashTable_insert(tre->hash, name, field, error))
		{
		    //free(text);
		    return NITF_FAILURE;
		}
		//free(text);
	    }
	    
	    
	}
	
    }
    return NITF_SUCCESS;
}



/*
 *  Organizes data out of the DOM and puts it in the hash
 *
 *
 */
NITFPRIV(nitf_Pair*) getElementFromDOM(nitf_TRE* tre,
				       xmlNode* node, 
				       xmlNode* find,
				       const char* prepend)
{
    xmlNode * current = NULL;
    
    int depth = 0;
    char lastName[512] = "";

    for (current = node; current; current = current->next)
    {
	if (current->type == XML_ELEMENT_NODE)
	{

	    char name[512];
	    
	    if (strcmp((char*)current->name, lastName) == 0)
	    {
		depth++;
	    }
	    else depth = 0;
	    
	    strcpy(lastName, (char*)current->name);
	    sprintf(name, "%s/%s[%d]", prepend, (char*)current->name, depth);
	    //printf("Looking for name: %s\n", name);
	    if (current == find)
	    {
		//printf("Actually got here\n");
		return nitf_HashTable_find(tre->hash, name);
	    }

	    nitf_Pair* rv = 
		getElementFromDOM(tre, current->children, find, name);
	    if (rv) return rv;

		
	    
	    
	}
	
    }
    //printf("Got here\n");
}


/*
 *  This function is called by the Reader.  Since I am feeling lazy right now,
 *  Im going to read into memory.
 *  TODO: Read XML from the file handle
 *
 */
NITFPRIV(NITF_BOOL) XMLTRE_read(nitf_IOHandle ioHandle, 
				nitf_TRE* tre, 
				struct _nitf_Record* record, nitf_Error* error)
{
    XMLTREData* treData = NULL;
    
    xmlNode* node;
    
    //char *data = NULL; 
    NITF_BOOL success; 
    if (!tre) {goto CATCH_ERROR;} 
    
    treData = XMLTREData_construct(error);
    if (!treData) goto CATCH_ERROR;
    
    treData->dirty = 0;
    treData->memory = (char*)malloc(tre->length); //xmlBufferCreateSize(tre->length);
    treData->memorySize = tre->length;
    //data = (char*)NITF_MALLOC( tre->length ); 
    if (!treData->memory) 
    { 
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),NITF_CTXT, NITF_ERR_MEMORY );
        goto CATCH_ERROR;
    }
    memset(treData->memory, 0, tre->length);
    success = nitf_TREUtils_readField(ioHandle, treData->memory, (int)tre->length, error);
    if (!success) goto CATCH_ERROR;
    
    treData->doc = xmlReadMemory(treData->memory, treData->memorySize, NULL, NULL, 0);
    
    tre->priv = treData;
    
    node = xmlDocGetRootElement(treData->doc);
    //putElementsInTRE(node, tre);
    
    //  print_element_names(node);
    putElementsInTRE(node, tre, "", error);
    /*free the document */
    //xmlFreeDoc(doc);
    //tre->
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    return NITF_SUCCESS;
 CATCH_ERROR:
    if (treData->memory) NITF_FREE(treData->memory);
    if (treData) NITF_FREE(treData);
    return NITF_FAILURE;
    
}



NITFPRIV(NITF_BOOL) XMLTRE_initData(nitf_TRE * tre, const char* id, nitf_Error * error)
{
    
    /* first, we need to build our DOM */
    LIBXML_TEST_VERSION
    //xmlSetBufferAllocationScheme(XML_BUFFER_ALLOC_EXACT);
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) XMLTREData_updateBuffer(XMLTREData* treData, nitf_Error* error)
{
    if (!treData->dirty && treData->memory)
    {
	/* We can just write it out */
	return NITF_SUCCESS;
    }
    /* If we have the memory (and its just dirty) we need to free it */
    else if (treData->memory)
    {
	//xmlBufferFree(treData->memory);
	free(treData->memory);
    }
    xmlDocDumpMemory(treData->doc, (xmlChar**)& (treData->memory), & (treData->memorySize));
    
    /* In either case, dirty or memory need to allocate a buffer and get it */
    //treData->memory = xmlBufferCreate();
    
    //xmlSaveFileTo(treData->memory, treData->doc, NULL); // UTF-8??? 
    //xmlNode* node = xmlDocGetRootElement(treData->doc);
    
    treData->dirty = 0;
    return NITF_SUCCESS;
}

NITFPRIV(NITF_BOOL) XMLTRE_write(nitf_IOHandle ioHandle, nitf_TRE* tre, 
				 struct _nitf_Record* record, nitf_Error* error)
{
    //const char* xmlData;
    //int xmlDataSize;
    XMLTREData * treData = (XMLTREData*)tre->priv;
    
    /* Write out our DOM here */	
    if (!XMLTREData_updateBuffer(treData, error)) return NITF_FAILURE;
    //xmlDataSize = xmlBufferLength(treData->memory);
    //xmlData = (const char*)xmlBufferContent(treData->memory);
    if (!nitf_IOHandle_write(ioHandle, treData->memory, treData->memorySize, error))
	return NITF_FAILURE;
    
    return NITF_SUCCESS;
}

NITFPRIV(int) XMLTREIterator_getCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
    XMLTREData * treData = (XMLTREData*)tre->priv;
    if (!XMLTREData_updateBuffer(treData, error)) return -1;
    return treData->memorySize;
    //return xmlBufferLength(treData->memory);
}



NITFPRIV(nitf_List*) getFieldsFromXPath(nitf_TRE* tre, xmlNodeSet* nodes)
{
    int i, size;
    xmlNode* current;
    XMLTREData* treData = tre->priv;
    size = (nodes) ? nodes->nodeNr : 0;
    nitf_List* list = NULL;
    for (i = 0; i < size; i++)
    {
	if (nodes->nodeTab[i]->type == XML_NAMESPACE_DECL)
	{
	    printf("XML Namespace Decl\n");
	}
	else if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE)
	{
	    
	    nitf_Pair* pair = NULL;
	    nitf_Field* field = NULL;
	    xmlNode* root;
	    current = nodes->nodeTab[i];
	    //printf("Element: %s\n", current->name);
	    root = xmlDocGetRootElement(treData->doc);
	    pair = getElementFromDOM(tre, root, current, "");
	    if (pair)
	    {

		if (list == NULL)
		{
		    nitf_Error error;
		    list = nitf_List_construct(&error);
		    assert(list);
		    nitf_List_pushBack(list, pair, &error);
		}
		
		field = (nitf_Field*)pair->data;
	    //printf("Found: %s [%.*s]\n", pair->key, field->length, field->raw);
	    }
	    else
	    {
		printf("Found this key, but its not a field\n");
	    }
	}
	else
	{
	    current = nodes->nodeTab[i];
	    printf("Att?: %s\n", current->name);

	}
    }
    return list;

}

NITFPRIV(nitf_List*) XMLTRE_find(nitf_TRE* tre, const char* pattern, nitf_Error* error)
{
    nitf_List* list;
    xmlXPathContext* xpathContext;
    xmlXPathObject* xpathObject;
    /* We want XPath to get the job done here */
    XMLTREData* treData = (XMLTREData*)tre->priv;
    xpathContext = xmlXPathNewContext(treData->doc);
    if (xpathContext == NULL)
    {
	/* How do we solve these errors */
	return NULL;
    }
    xpathObject = xmlXPathEvalExpression((const xmlChar*)pattern, xpathContext);
    if (xpathObject == NULL)
    {
	xmlXPathFreeContext(xpathContext);
	return NULL;
    }
    
    list = getFieldsFromXPath(tre, xpathObject->nodesetval);
    

    xmlXPathFreeObject(xpathObject);
    xmlXPathFreeContext(xpathContext);
    
    return list;
    


}


NITFPRIV(NITF_BOOL) XMLTRE_setField(nitf_TRE* tre, const char* tag, NITF_DATA* data, size_t dataLength, nitf_Error* error)
{
	nitf_Field* field = NULL;
	XMLTREData * treData = (XMLTREData*)tre->priv;
	

	if (! nitf_HashTable_exists(tre->hash, tag))
	{

		field = nitf_Field_construct(dataLength, NITF_BCS_A, error);
		if (!field)
			return NITF_FAILURE;

		if (!nitf_HashTable_insert(tre->hash, tag, field, error))
			return NITF_FAILURE;
	}
	else
	{
		// If it exists alread, lets be real flexible
		
		nitf_Pair* pair = nitf_HashTable_find(tre->hash, tag);
		assert(pair);
		field = (nitf_Field*)pair->data;
		if ( field->length != dataLength )
		{
			nitf_Field_destruct(&field);
			if (!nitf_Field_construct(dataLength, NITF_BCS_A, error))
				return NITF_FAILURE;

			pair->data = field;
		}
	}

	treData->dirty = 1;

	return nitf_Field_setRawData(field, data, dataLength, error);
}


NITFPRIV(NITF_BOOL) XMLTRE_increment(nitf_TREEnumerator** it, nitf_Error* error)
{
	nitf_HashTableIterator* hashIt = (nitf_HashTableIterator*)(*it)->data;
	
	nitf_HashTableIterator end = nitf_HashTable_end( hashIt->hash );

	nitf_HashTableIterator_increment(hashIt);
	
	if (nitf_HashTableIterator_equals(&end, hashIt))
	{
		NITF_FREE(hashIt);
		NITF_FREE( (*it) );
		*it = NULL;
	}

	
	return NITF_SUCCESS;
	

}



NITFPRIV(nitf_Pair*) XMLTREIterator_get(nitf_TREEnumerator* it, nitf_Error* error)
{
	nitf_HashTableIterator* hashIter = (nitf_HashTableIterator*)it->data;
	
	return nitf_HashTableIterator_get(hashIter);
	
}

NITFPRIV(nitf_TREEnumerator*) XMLTRE_begin(nitf_TRE* tre, nitf_Error* error)
{
	/* Iteration is easy, we are using a hash table iterator underneath */

	nitf_TREEnumerator* it = (nitf_TREEnumerator*)NITF_MALLOC(sizeof(nitf_TREEnumerator));
	nitf_HashTableIterator* hashIter = (nitf_HashTableIterator*)NITF_MALLOC(sizeof(nitf_HashTableIterator));
	*hashIter = nitf_HashTable_begin(tre->hash);
	
	it->data = hashIter;
	it->next = XMLTRE_increment;
	it->get = XMLTREIterator_get;
	

	
	
	return it;
	
}


static nitf_TREHandler gHandler = 
{
	XMLTRE_initData,
	XMLTRE_read,
	XMLTRE_setField,
	XMLTRE_find,
	XMLTRE_write,
	XMLTRE_begin,
	XMLTREIterator_getCurrentSize,
	NULL

};

NITFAPI(char**) XMLTRE_init(nitf_Error* error)
{
	return ident;
} 
    
    
NITFAPI(nitf_TREHandler*) XMLTRE_handler(nitf_Error* error)
{
	return &gHandler;
}

NITFAPI(void) XMLTRE_cleanup(void)
{
	
}

NITF_CXX_ENDGUARD

