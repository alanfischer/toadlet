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

#include "NodeTest.h"

bool NodeTest::testNodeTransform(){
	Node::ptr parent=new Node(NULL);
	Node::ptr child=new Node(NULL);
	Node::ptr gchild=new Node(NULL);

	gchild->setBound(new Bound(Sphere(0)));

	child->attach(gchild);
	parent->attach(child);
	
	gchild->setTranslate(Vector3(0,0,1));
	child->setTranslate(Vector3(0,0,1));
	
	Log::alert(String("GChild translate:")+gchild->getWorldTranslate().z);
	Log::alert(String("Parent bound:")+parent->getWorldBound()->getSphere().radius);

	return true;
}

bool NodeTest::run(){
	bool result=false;

	Log::alert("Testing NodeTransform");
	result=testNodeTransform();
	Log::alert(result?"Succeeded":"Failed");

	return result;
}
