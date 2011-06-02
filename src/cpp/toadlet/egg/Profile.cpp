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

#include <toadlet/egg/Profile.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace egg{

Profile::ProfileNode::ProfileNode(const char *name):
	mName(name),
	mCurrent(0),mTotal(0)
{}

Profile::ProfileNode *Profile::ProfileNode::getChild(const char *name){
	ProfileNode *node=NULL,*last=NULL;
	for(node=mFirstChild;node!=NULL;node=node->mNext){
		if(strcmp(node->mName,name)==0){
			break;
		}
		last=node;
	}

	if(node==NULL){
		if(mFirstChild==NULL){
			node=new ProfileNode(name);
			mFirstChild=ProfileNode::ptr(node);
		}
		else{
			node=new ProfileNode(name);
			last->mNext=ProfileNode::ptr(node);
		}
	}
	node->mParent=this;
	return node;
}

void Profile::ProfileNode::start(){
	mCurrent=System::utime();
}

void Profile::ProfileNode::stop(){
	uint64 time=System::utime();
	mTotal+=time-mCurrent;
}

void Profile::ProfileNode::clear(){
	mCurrent=0;
	mTotal=0;
}

Profile *Profile::getInstance(){
	if(mTheProfile==NULL){
		mTheProfile=new Profile();
	}
	return mTheProfile;
}

Profile::Profile(){
	mRoot=ProfileNode::ptr(new ProfileNode(NULL));
	mCurrent=mRoot;
}

Profile::~Profile(){
}

void Profile::beginSection(const char *name){
	TOADLET_ASSERT(mCurrent!=NULL);

	mCurrent=mCurrent->getChild(name);
	mCurrent->start();

	TOADLET_ASSERT(mCurrent!=NULL);
}

void Profile::endSection(const char *name){
	TOADLET_ASSERT(mCurrent!=NULL && strcmp(mCurrent->getName(),name)==0);

	mCurrent->stop();
	mCurrent=mCurrent->getParent();

	TOADLET_ASSERT(mCurrent!=NULL);
}

void Profile::outputTimings(ProfileNode *node,int depth){
	String spaces;
	for(int i=0;i<depth;++i){
		spaces+="\t";
	}

	if(node!=mRoot){
		Logger::alert(Categories::TOADLET_EGG_PROFILE,spaces+node->getName()+":"+node->getTotal());
	}

	for(node=node->getFirstChild();node!=NULL;node=node->getNext()){
		outputTimings(node,depth+1);
	}
}

void Profile::clearTimings(ProfileNode *node){
	node->clear();

	for(node=node->getFirstChild();node!=NULL;node=node->getNext()){
		clearTimings(node);
	}
}

Profile *Profile::mTheProfile;

}
}

