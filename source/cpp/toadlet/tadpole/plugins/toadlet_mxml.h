/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_MXML_H
#define TOADLET_MXML_H

#include <mxml.h>

#define mxmlGetElementName(x) ((x->type==MXML_ELEMENT)?x->value.element.name:"")
#define mxmlFindChild(x,s) mxmlFindElement(x->child,x,s,NULL,NULL,MXML_NO_DESCEND)
#define mxmlGetOpaque(x) ((x!=NULL)?x->value.opaque:NULL)
#define mxmlAddChild(x,c) mxmlAdd(x,MXML_ADD_AFTER,MXML_ADD_TO_PARENT,c)

const char *mxmlSaveCallback(mxml_node_t *node,int ws);

#endif
