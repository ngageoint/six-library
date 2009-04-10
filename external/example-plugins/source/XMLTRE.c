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


#include <nitf/IOInterface.h>
#include <nitf/TREUtils.h>
#include <nitf/TREPrivateData.h>
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
    /* DOM */
    xmlDoc* doc;

    /* DOM memory */
    char* memory;

    /* DOM memory size */
    int memorySize;    

    /* Flag set when field is updated */
    int dirty;
    
    /* only using this for its HashTable */
    nitf_TREPrivateData *priv;

} XMLTREData;

NITFPRIV(void) XMLTREData_destruct(XMLTREData** data)
{
    if (data && (*data))
    {
        if ((*data)->memory)
            NITF_FREE((*data)->memory);
        if ((*data)->priv)
            nitf_TREPrivateData_destruct(&(*data)->priv);
        NITF_FREE(*data);
        *data = NULL;
    }
}

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
    data->doc = NULL;
    data->dirty = 0;
    data->priv = nitf_TREPrivateData_construct(error);
    if (!data->priv)
    {
        XMLTREData_destruct(&data);
        return NULL;
    }
    return data;
}

NITFPRIV(XMLTREData*) XMLTREData_clone(XMLTREData *source, nitf_Error* error)
{
    XMLTREData* data = XMLTREData_construct(error);
    if (!data)
        return NULL;
    
    data->memorySize = source->memorySize;
    data->dirty = source->dirty;
    if (source->memory)
    {
        memcpy(data->memory, source->memory, data->memorySize);
        if(!data->memory)
        {
            XMLTREData_destruct(&data);
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
    }
    
    if (source->doc)
    {
        data->doc = xmlCopyDoc(source->doc, 1);
        if (!data->doc)
        {
            XMLTREData_destruct(&data);
            nitf_Error_init(error, "Unable to copy the XML document",
                    NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
    }
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
    int depth = 1;
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
            else depth = 1;
            
            strcpy(lastName, (char*)current->name);
            sprintf(name, "%s/%s[%d]", prepend, (char*)current->name, depth);

            putElementsInTRE(current->children, tre, name, error);
            
            text = getText(current->children);
            
            if (text != NULL)
            {
                field = nitf_Field_construct(strlen(text), NITF_BCS_A, error);
                nitf_Field_setString(field, text, error);
                if (!nitf_HashTable_insert(((XMLTREData*)tre->priv)->priv->hash,
                        name, field, error))
                {
                    //free(text);
                    return NITF_FAILURE;
                }
                //free(text);
            }
            
            if (current->properties)
            {
                xmlAttr* attrs = NULL;
                
                for (attrs = current->properties; attrs; attrs = attrs->next)
                {
                    char attName[128] = "";
                    char* value = (char*)xmlGetProp(current, attrs->name);
                    nitf_Field* attField = NULL;
                    sprintf(attName, "%s/@%s", name, attrs->name);
                    
                    attField = nitf_Field_construct(strlen(value), NITF_BCS_A, error);
                    nitf_Field_setString(attField, value, error);
                    if (!nitf_HashTable_insert(
                            ((XMLTREData*)tre->priv)->priv->hash,
                            attName, attField, error))
                    {
                        free(value);
                        return NITF_FAILURE;
                    }
                }
            }
        }
    }
    return NITF_SUCCESS;
}


/* Maybe we dont have to do this, but I cant find the
   getElementsByTagName equivalent in libxml2 -- lame! */

NITFPRIV(nitf_List*) getElementsByTagName(xmlNode* parent, const char* name, nitf_Error* error)
{
    xmlNode* current = NULL;
    nitf_List* list = nitf_List_construct(error);
    if (!list) goto CATCH_ERROR;

    for (current = parent->children; current; current = current->next)
    {
        if (current->type == XML_ELEMENT_NODE)
        {
            if (strcmp((const char*)current->name, name) == 0)
            {

                if (!nitf_List_pushBack(list, current, error))
                {
                    goto CATCH_ERROR;
                }
            }
        }
        
    }
    return list;
    
 CATCH_ERROR:
    if (list)
    {
        nitf_ListIterator it = nitf_List_begin(list);
        nitf_ListIterator end = nitf_List_end(list);
        
        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            current = (xmlNode*)nitf_ListIterator_get(&it);
            xmlUnlinkNode(current);
            xmlFreeNode(current);
            nitf_ListIterator_increment(&it);
        }
        nitf_List_destruct(&list);
    }
    return NULL;
}
                
/* We do have this element, so lets replace it */
/* This doesnt appear to actually be necessary, since I can just set the
   content of an existing node in this case -- see where I commented it out */
NITFPRIV(NITF_BOOL) removeTextNodes(xmlNode* parent, nitf_Error* error)
{
    NITF_BOOL rv = NITF_SUCCESS;
    xmlNode* current = NULL;
    nitf_List* list = nitf_List_construct(error);
    if (!list) return NITF_FAILURE;

    for (current = parent->children; current; current = current->next)
    {
        
        if (current->type == XML_TEXT_NODE)
        {
            if (!nitf_List_pushBack(list, current, error))
            {
                rv = NITF_FAILURE;
                goto CLEANUP;
            }
        }
        else
        {
            printf("Found a non-text node\n");
        }
    }

 CLEANUP:
    {
        nitf_ListIterator it = nitf_List_begin(list);
        nitf_ListIterator end = nitf_List_end(list);

        while (nitf_ListIterator_notEqualTo(&it, &end))
        {
            current = (xmlNode*)nitf_ListIterator_get(&it);
            xmlUnlinkNode(current);
            xmlFreeNode(current);
            nitf_ListIterator_increment(&it);
        }
        nitf_List_destruct(&list);
    }
    return rv;

}  

/*
 *  Here we go looking for all elements with the same name given in thisTag.
 *  If we are under count for what we need in the index, we need to create some
 *  intermediary dummy nodes to fill in those values for later.
 *
 *  Its the user's responsibility to fill those nodes in with text where
 *  needed.  For now, we are just creating them in the right places
 */
NITFPRIV(xmlNode*) doExpansion(xmlNode* parent, const char* thisTag, int index, int found, nitf_Error* error)
{
    xmlNode* current = NULL;
    int numElements = 0;
    nitf_List* elements = getElementsByTagName(parent, thisTag, error);
    if (!elements)
    {
        return NULL;
    }
    
    numElements = nitf_List_size(elements);

    /* 
     *  If the field was found already, that implies that the entire
     *  path of nodes to it should exist already as well
     */
    if (numElements <= index)
    {
        int have = 0;
        if (found)
        {
            /* Something is wrong */
            nitf_Error_init(error, "Not enough children exist for found field", NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
            return NULL;
        }
        /* Else, create dummy elements all the way up to the one we need */
        for (have = numElements; have <= index; have++)
        {
            printf("Creating dummy element\n");
            current = xmlNewChild(parent, NULL, (const xmlChar*)thisTag, NULL);
        }
    }
    else
    {
        /* Just need the ith element in the list! */
        nitf_ListIterator obj = nitf_List_at(elements, index);
        current = (xmlNode*)nitf_ListIterator_get(&obj);
    }

    return current;

}

/*
 *  We're going to want this function to traverse our DOM, installing
 *  new element nodes every step of the way, until we get where we are 
 *  going
 *  
 *  Current working alg
 *  /root[0]/next[1]
 *
 */
NITFPRIV(NITF_BOOL) putElementInDOM(nitf_TRE* tre,
                                    const char* tag,
                                    const char* value,
                                    xmlNode* parent,
                                    int found,
                                    nitf_Error* error) 
{
    xmlNode* current = NULL;
    char next[128] = "";
    char thisTag[128] = "";
    /* This is the next chunk we are on */
    size_t endOfTag = strcspn(tag, "[");
    size_t tagLength = strlen(tag);
    int index = 0;
    int rv;
    /* 
     *  Now, for example, we are pointing at
     *  root[0]/next[1], so we need to figure out what number
     */
    
    /* deal with attributes */
    if (tag[0] == '@' && endOfTag == tagLength)
    {
        xmlAttr *attr = NULL;
        
        thisTag[endOfTag] = 0;
        memcpy(thisTag, &tag[1], endOfTag);
        
        /* in case it exists, this unsets it */
        xmlUnsetProp(parent, (const xmlChar*)thisTag);
        attr = xmlNewProp(parent, (const xmlChar*)thisTag,
                (const xmlChar*)value);
        
        /* we're done! */
        if (!attr)
            return NITF_FAILURE;
        return NITF_SUCCESS;
    }
    
    
    if (endOfTag + 1 >= tagLength)
    {
        /* we default to the first index if there isn't an index */
        index = 1;
        rv = 1;
        endOfTag = tagLength;
        /* We ran out of rope 
        nitf_Error_init(error, "Over the line", 
        NITF_CTXT, NITF_ERR_INVALID_PARAMETER);
        return NITF_FAILURE;*/
    }
    else
        rv = sscanf(&tag[endOfTag], "[%d]/%s", &index, next);

    index -= 1;

    thisTag[endOfTag] = 0;
    memcpy(thisTag, tag, endOfTag);

    /* If we dont have a result of two -- assuming for now, a result of one,
       we have found the actual node we are trying to get to in this fully
       specified XPath.  Now we need to do the exansion on this one more time,
       and set our text content to the value */
    if (rv != 2)
    {
        if (!found)
        {
            current = doExpansion(parent, thisTag, index, found, error);
        
            xmlNodeSetContent(current, (const xmlChar*)value);
        }
        else
        {
            nitf_List* elements = getElementsByTagName(parent, thisTag, error);
            nitf_ListIterator obj = nitf_List_at(elements, index);
            current = (xmlNode*)nitf_ListIterator_get(&obj);
/*              if (!removeTextNodes(parent, error)) */
/*              { */
/*                  return NITF_FAILURE; */
             /*}*/
            xmlNodeSetContent(current, (const xmlChar*)value);
        }
/*         if (found) */
/*         { */
/*         } */

/*         xmlNewTextChild(parent, */
/*                         NULL,  */
/*                         (const xmlChar*)thisTag, */
/*                         (const xmlChar*)value); */
        return NITF_SUCCESS;
    }


    /* If we are still here, that means we are still above the target element,
       which means we need to keep expanding (fanning out) and drilling down
       to the sub-nodes underneath us, so we continue recursion */
    current = doExpansion(parent, thisTag, index, found, error);
    if (!current)
        return NITF_FAILURE;

    return putElementInDOM(tre, next, value, current, found, error);
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
    nitf_Pair* rv = NULL;
    
    int depth = 1;
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
            else depth = 1;
            
            strcpy(lastName, (char*)current->name);
            sprintf(name, "%s/%s[%d]", prepend, (char*)current->name, depth);
            if (current == find)
            {
                return nitf_HashTable_find(
                        ((XMLTREData*)tre->priv)->priv->hash, name);
            }
            rv = getElementFromDOM(tre, current->children, find, name);
            if (rv) return rv;
        }
    }
    
    return NULL;
}


/*
 *  This function is called by the Reader.  
 *  Since I am feeling lazy right now,
 *  Im going to read into memory.
 *  TODO: Read XML from the file handle
 *
 */
NITFPRIV(NITF_BOOL) XMLTRE_read(nitf_IOInterface* ioIface,
                                nitf_Uint32 length,
                                nitf_TRE* tre,
                                struct _nitf_Record* record,
                                nitf_Error* error)
{
    XMLTREData* treData = NULL;
    
    xmlNode* node;
    
    NITF_BOOL success; 
    if (!tre) {goto CATCH_ERROR;} 
    
    treData = XMLTREData_construct(error);
    if (!treData) goto CATCH_ERROR;
    
    treData->dirty = 0;
    treData->memory = (char*)malloc(length);
    treData->memorySize = length;
    if (!treData->memory) 
    { 
        nitf_Error_init(error, NITF_STRERROR( NITF_ERRNO ),
                        NITF_CTXT, NITF_ERR_MEMORY );
        goto CATCH_ERROR;
    }
    memset(treData->memory, 0, length);
    success = nitf_TREUtils_readField(ioIface, treData->memory, length, error);
    if (!success) goto CATCH_ERROR;
    
    treData->doc = xmlReadMemory(treData->memory, treData->memorySize, NULL, NULL, 0);
    
    tre->priv = treData;
    
    node = xmlDocGetRootElement(treData->doc);
    
    putElementsInTRE(node, tre, "", error);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
    return NITF_SUCCESS;
 CATCH_ERROR:
    if (treData) XMLTREData_destruct(&treData);
    return NITF_FAILURE;
    
}


NITFPRIV(const char*) XMLTRE_getID(nitf_TRE* tre)
{
    return NULL;
}


NITFPRIV(NITF_BOOL) XMLTRE_initData(nitf_TRE * tre, const char* id, nitf_Error * error)
{
    XMLTREData* treData = NULL;
    xmlNode* node = NULL;
    
    /* first, we need to build our DOM */
    LIBXML_TEST_VERSION;
    
    if (!tre) { return NITF_FAILURE; } 
    
    treData = XMLTREData_construct(error);
    if (!treData) return NITF_FAILURE;
    
    /* We have no memory data, so we are starting off dirty */
    treData->dirty = 1; 
    treData->memory = NULL; 
    treData->memorySize = 0;
    
    treData->doc = xmlNewDoc(BAD_CAST "1.0");
    node = xmlNewNode(NULL, BAD_CAST id);
    xmlDocSetRootElement(treData->doc, node);
    
    tre->priv = treData;

    return NITF_SUCCESS;
}

/*
 *  If we have made modifications to the TRE, its up
 *  to this function to capture those changes before they
 *  are written.
 *
 *  This function checks if we have modified the TRE first
 *  If not, it can return success without changing anything,
 *  since the buffer has the same content as the fields
 *
 *  If there is memory allocated, and the function has
 *  continued, it must mean that our buffer is dirty.  Therefore
 *  we need to free the memory.
 *
 *  In either case, we need to rewrite the DOM to the buffer.
 *  Then we can safely set the dirty flag to false.
 */
NITFPRIV(NITF_BOOL) XMLTREData_updateBuffer(XMLTREData* treData, 
                                            nitf_Error* error)
{
    if (!treData->dirty && treData->memory)
    {
        /* We can just write it out */
        return NITF_SUCCESS;
    }
    /* If we have the memory (and its just dirty) we need to free it */
    else if (treData->memory)
    {
        free(treData->memory);
    }
    xmlDocDumpMemory(treData->doc, 
                     (xmlChar**)& (treData->memory),
                     &(treData->memorySize));
    
    /* In either case, dirty or memory need to allocate a buffer and get it */
    treData->dirty = 0;
    return NITF_SUCCESS;
}

/*
 *  In order to write the XML TRE out, we need to make sure that
 *  we rewrite the memory buffer, if it is stale.  This happens
 *  since we sync only the DOM on TRE mods.
 */
NITFPRIV(NITF_BOOL) XMLTRE_write(nitf_IOInterface* ioIface, 
                                 nitf_TRE* tre, 
                                 struct _nitf_Record* record, 
                                 nitf_Error* error)
{
    XMLTREData * treData = (XMLTREData*)tre->priv;
    
    /* Write out our DOM here */        
    if (!XMLTREData_updateBuffer(treData, error)) return NITF_FAILURE;
    if (!nitf_IOInterface_write(ioIface, 
                                treData->memory, 
                                treData->memorySize, error))
        return NITF_FAILURE;
    
    return NITF_SUCCESS;
}

/*
 *  Currently, in order to make this work, I find it easiest to just
 *  re-sync the DOM.
 *
 *  Once I have done that, the length is going to be the current DOM
 *  length.
 *
 */
NITFPRIV(int) XMLTREIterator_getCurrentSize(nitf_TRE* tre, nitf_Error* error)
{
    XMLTREData * treData = (XMLTREData*)tre->priv;
    if (!XMLTREData_updateBuffer(treData, error)) return -1;
    return treData->memorySize;
}


/*
 *  This function assumes that I already made an XPath query
 *  on the DOM, and have retrieved a result set.  We are doing
 *  this to fulfill the find() function, giving it an implementation
 *  that you might actually want to do something with
 *
 *  For each element node in our result set, we retrieve the
 *  associated nitf_Pair containing the path name and the node as
 *  a Field.
 *
 */
NITFPRIV(nitf_List*) getFieldsFromXPath(nitf_TRE* tre, xmlNodeSet* nodes)
{
    int i, size;
    xmlNode* current = NULL;
    nitf_List* list = NULL;
    XMLTREData* treData = tre->priv;
    
    size = (nodes) ? nodes->nodeNr : 0;
    
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

/*
 *  The ability to find a node comes from NITRO 2.0's TRE interface.
 *  The default behavior is typically to look for the pattern as a
 *  substring.  For typical TREs, if we knew that we had a regex
 *  library at our disposal, we might use it to find the pattern.
 *
 *  However, since this is XML, by far the most useful thing to do
 *  here is to let the end-user run XPath queries against it.
 *
 *  This function builds an XPath query from the pattern, and
 *  evaluates it against the DOM, producing a list of nitf_Pair
 *  values back to the caller.
 *
 *  \TODO: Allow attribute results too!
 *
 */
NITFPRIV(nitf_List*) XMLTRE_find(nitf_TRE* tre, 
                                 const char* pattern, 
                                 nitf_Error* error)
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
NITFPRIV(NITF_BOOL) XMLTRE_setField(nitf_TRE* tre, 
                                    const char* tag, 
                                    NITF_DATA* data, 
                                    size_t dataLength, 
                                    nitf_Error* error)
{
    char* value = NULL;
    nitf_Field* field = NULL;
    XMLTREData * treData = (XMLTREData*)tre->priv;
    

    /* This is the next chunk we are on */
    size_t endOfTag = strcspn(tag, "]");
    
    xmlNode* root = xmlDocGetRootElement(treData->doc);
    
    /* 
     *  Note that doing it this way could cause a problem, so we 
     *  remove the value if we can't make it work
     *  TODO: clean up this mess!
     */
    
    value = (char*)malloc(dataLength + 1);
    value[dataLength] = 0;
    memcpy(value, data, dataLength);

    /*  Put the element in the DOM directly */
    if (!putElementInDOM(tre, &tag[endOfTag + 2], value, root, 0, error))
        goto CATCH_ERROR;
    
    /*  And now check if its in the hash table  */
    if (! nitf_HashTable_exists(((XMLTREData*)tre->priv)->priv->hash, tag))
    {
        /*  If not, make a new entry */
        field = nitf_Field_construct(dataLength, NITF_BCS_A, error);
        if (!field)
            return NITF_FAILURE;
        
        /*  And set it */
        nitf_Field_setString(field, value, error);
        
        /*  And insert it  */
        if (!nitf_HashTable_insert(((XMLTREData*)tre->priv)->priv->hash,
                tag, field, error))
            goto CATCH_ERROR;
    }
    else
    {
        /*  Otherwise, we just need to get it and reset the value  */

        /*  
         *  !!! This looks really bad!  What if the field length 
         *      is too short?  Will this get truncated? !!!
         */
        nitf_Pair* pair = nitf_HashTable_find(
                ((XMLTREData*)tre->priv)->priv->hash, tag);
        assert(pair);
        field = (nitf_Field*)pair->data;
        nitf_Field_setString(field, value, error);
    
    }

    /*  Either way, the TRE is dirty now */
    treData->dirty = 1;
    free(value);
    return NITF_SUCCESS;
    
 CATCH_ERROR:
    if (field)
    {
        nitf_Field_destruct(&field);
    }
    if (value)
    {
        free(value);
    }
    return NITF_FAILURE;
}

/*
 *  Check if there is another field
 */
NITFPRIV(NITF_BOOL) XMLTRE_hasNext(nitf_TREEnumerator** it)
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
NITFPRIV(nitf_Pair*) XMLTREIterator_next(nitf_TREEnumerator* it, 
                                         nitf_Error* error)
{
    nitf_HashTableIterator* hashIter = (nitf_HashTableIterator*)it->data;
    
    nitf_Pair* data = nitf_HashTableIterator_get(hashIter);
    nitf_HashTableIterator_increment(hashIter);
    return data;
}

/*
 *  Get the enumerator to the beginning of the TRE
 *  Iteration is easy, we are using a hash table iterator underneath 
 *  Since I make no effort to keep this stuff ordered, its going to
 *  be hash-ordered (i.e., not ordered at all)
 */
NITFPRIV(nitf_TREEnumerator*) XMLTRE_begin(nitf_TRE* tre, nitf_Error* error)
{

    nitf_TREEnumerator* it = 
        (nitf_TREEnumerator*)NITF_MALLOC(sizeof(nitf_TREEnumerator));

    nitf_HashTableIterator hashEnd = nitf_HashTable_end(
            ((XMLTREData*)tre->priv)->priv->hash);
    nitf_HashTableIterator* hashIter = 
        (nitf_HashTableIterator*)NITF_MALLOC(sizeof(nitf_HashTableIterator));

    *hashIter = nitf_HashTable_begin(((XMLTREData*)tre->priv)->priv->hash);
    
    if (nitf_HashTableIterator_equals(&hashEnd, hashIter))
    {
        NITF_FREE(hashIter);
        NITF_FREE(it);
        return NULL;
    }
    
    it->data = hashIter;
    it->hasNext = XMLTRE_hasNext;
    it->next = XMLTREIterator_next;
    
    return it;
}

/*
 *  When we are done, its time to clean up shop.
 *
 */
NITFPRIV(void) XMLTRE_destruct(nitf_TRE *tre)
{
    if (tre && tre->priv)
    {
        XMLTREData_destruct((XMLTREData**)&tre->priv);
    }
}

/*
 *  \TODO: Im not sure if this has been tested
 *
 */
NITFPRIV(NITF_BOOL) XMLTRE_clone(nitf_TRE *source,
                                 nitf_TRE *tre,
                                 nitf_Error *error)
{
    XMLTREData *sourcePriv = NULL;
    XMLTREData *trePriv = NULL;
    
    if (!tre || !source || !source->priv)
        return NITF_FAILURE;
    
    sourcePriv = (XMLTREData*)source->priv;
    trePriv = (XMLTREData*)tre->priv;
    
    /* this clones the hash */
    if (!(trePriv = XMLTREData_clone(sourcePriv, error)))
        return NITF_FAILURE;
    
    return NITF_SUCCESS;
}

/*
 *  Get the field, if it exists as a nitf_Field.  Otherwise,
 *  just return NULL
 *
 */
NITFPRIV(nitf_Field*) XMLTRE_getField(nitf_TRE* tre, const char* tag)
{
    nitf_Pair* pair = nitf_HashTable_find(
            ((XMLTREData*)tre->priv)->priv->hash, tag);
    if (!pair) return NULL;
    return (nitf_Field*)pair->data;
}

/*
 *  This is the descriptor for our XML handler
 */
static nitf_TREHandler gHandler = 
{
        XMLTRE_initData,
        XMLTRE_getID,
        XMLTRE_read,
        XMLTRE_setField,
        XMLTRE_getField,
        XMLTRE_find,
        XMLTRE_write,
        XMLTRE_begin,
        XMLTREIterator_getCurrentSize,
        XMLTRE_clone,
        XMLTRE_destruct,
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

