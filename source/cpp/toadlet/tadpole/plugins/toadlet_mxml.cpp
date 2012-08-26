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

#include "toadlet_mxml.h"

const int MAX_TABS=10;
const char *TABS[MAX_TABS]={
	"",
	"\t",
	"\t\t",
	"\t\t\t",
	"\t\t\t\t",
	"\t\t\t\t\t",
	"\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t",
	"\t\t\t\t\t\t\t\t\t",
};

const char *mxmlSaveCallback(mxml_node_t *node,int ws){
	bool oneLiner=false;

	if(node->child!=NULL && node->child==node->last_child && node->child->type!=MXML_ELEMENT){
		oneLiner=true;
	}

	if(ws==MXML_WS_BEFORE_OPEN || (oneLiner==false && ws==MXML_WS_BEFORE_CLOSE)){
		int count=0;
		while((node=node->parent)!=NULL) count++;
		if(count>MAX_TABS-1){count=MAX_TABS-1;}
		return TABS[count];
	}
	else if((oneLiner==false && ws==MXML_WS_AFTER_OPEN) || ws==MXML_WS_AFTER_CLOSE){
		return "\n";
	}
	return NULL;
}
