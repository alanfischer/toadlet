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

#ifndef TOADLET_TADPOLE_ASSIMPHANDLER_H
#define TOADLET_TADPOLE_ASSIMPHANDLER_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/Node.h>
#include <toadlet/tadpole/CameraComponent.h>
#include <toadlet/tadpole/MeshComponent.h>
#include <toadlet/tadpole/LightComponent.h>
#include <assimp/scene.h>
#include <assimp/vector3.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API AssimpHandler:public Object{
public:
	TOADLET_OBJECT(AssimpHandler);

	AssimpHandler(Engine *engine);
	
	Node::ptr load(Stream *stream,const String &format);
	bool save(Stream *stream,Node *node,const String &format);

	Node::ptr loadScene(const aiScene *ascene);
	const aiScene *saveScene(Node *node);

	static aiVector3D toVector3(const Vector3 &v){return aiVector3D(v.x,v.y,v.z);}
	static aiQuaternion toQuaternion(const Quaternion &q){return aiQuaternion(q.x,q.y,q.z,q.w);}
	static aiMatrix4x4 toMatrix4x4(const Matrix4x4 &m){return aiMatrix4x4(
		m.data[ 0],m.data[ 1],m.data[ 2],m.data[ 3],
		m.data[ 4],m.data[ 5],m.data[ 6],m.data[ 7],
		m.data[ 8],m.data[ 9],m.data[10],m.data[11],
		m.data[12],m.data[13],m.data[14],m.data[15]);}

	static Vector3 toVector3(const aiVector3D &v){return Vector3(v.x,v.y,v.z);}
	static Quaternion toQuaternion(const aiQuaternion &q){return Quaternion(q.x,q.y,q.z,q.w);}
	static Matrix4x4 toMatrix4x4(const aiMatrix4x4 &m){return Matrix4x4(
		m.a1,m.a2,m.a3,m.a4,
		m.b1,m.b2,m.b3,m.b4,
		m.c1,m.c2,m.c3,m.c4,
		m.d1,m.d2,m.d3,m.d4);}

protected:
	class Scene{
	public:
		Node::ptr root;

		Collection<Camera::ptr> cameras;
		Collection<LightState> lights;
		Collection<Mesh::ptr> meshes;
	};

	Node::ptr loadScene(Scene *scene,const aiScene *ascene,const aiNode *anode);
	aiNode *saveScene(Scene *scene,const aiScene *ascene,Node *node);

	Engine *mEngine;
};

}
}

#endif
