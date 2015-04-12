/* =========================================================================
 * This file is part of xml.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * xml.lite-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "xml/lite/Document.h"

void xml::lite::Document::setRootElement(Element * element, bool own)
{
    destroy();
    mOwnRoot = own;
    mRootNode = element;
}

void xml::lite::Document::destroy()
{
    remove(mRootNode);
}

void xml::lite::Document::remove(Element * toDelete)
{
    //  Added code here to make sure we can remove from root
    if (toDelete == mRootNode)
    {
        if (mRootNode && mOwnRoot)
            delete mRootNode;
        mRootNode = NULL;
    }
    else
        remove(toDelete, mRootNode);
}

xml::lite::Element *
xml::lite::Document::createElement(const std::string& qname,
                                   const std::string& uri,
                                   std::string characterData)
{
    Element *elem = new Element();
    elem->setQName(qname);
    //std::cout << "qname: " << qname << std::endl;

    elem->setUri(uri);
    elem->setCharacterData(characterData);
    return elem;
}

void xml::lite::Document::insert(xml::lite::Element * element,
                                 xml::lite::Element * underThis)
{
    if (element != NULL && underThis != NULL)
        underThis->addChild(element);
}

void xml::lite::Document::remove(xml::lite::Element * toDelete,
                                 xml::lite::Element * fromHere)
{
    if (fromHere != NULL && toDelete != NULL)
    {
        for (std::vector<xml::lite::Element *>::iterator i =
                fromHere->getChildren().begin(); i
                != fromHere->getChildren().end(); ++i)
        {
            if (*i == toDelete)
            {
                fromHere->getChildren().erase(i);
                delete toDelete;
                toDelete = NULL;
                return;
            }
            else
            {
                remove(toDelete, *i);
            }
        }
    }
}
