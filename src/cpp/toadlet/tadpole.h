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

#ifndef TOADLET_TADPOLE_H
#define TOADLET_TADPOLE_H

#include <toadlet/tadpole/ArchiveManager.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/ContextListener.h>
#include <toadlet/tadpole/DetailTraceable.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/FontManager.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/MathFormatter.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/Noise.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/ResourceHandler.h>
#include <toadlet/tadpole/ResourceHandlerData.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Transform.h>
#include <toadlet/tadpole/TransformKeyFrame.h>
#include <toadlet/tadpole/TransformTrack.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/VertexFormats.h>

#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/animation/Controller.h>
#include <toadlet/tadpole/animation/ControllerFinishedListener.h>
#include <toadlet/tadpole/animation/CameraProjectionAnimation.h>
#include <toadlet/tadpole/animation/MaterialLightEffectAnimation.h>
#include <toadlet/tadpole/animation/NodePathAnimation.h>
#include <toadlet/tadpole/animation/SkeletonAnimation.h>

#include <toadlet/tadpole/node/AnaglyphCameraNode.h>
#include <toadlet/tadpole/node/AudioNode.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/LabelNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/MeshNodeSkeleton.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/NodeAttacher.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/ParticleNode.h>
#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/node/SpriteNode.h>
#include <toadlet/tadpole/node/NodeListener.h>
#include <toadlet/tadpole/node/NodeTransformInterpolator.h>

#include <toadlet/tadpole/sensor/BoundingVolumeSensor.h>
#include <toadlet/tadpole/sensor/NameSensor.h>
#include <toadlet/tadpole/sensor/PotentiallyVisibleSensor.h>
#include <toadlet/tadpole/sensor/Sensor.h>
#include <toadlet/tadpole/sensor/SensorResults.h>
#include <toadlet/tadpole/sensor/SensorResultsListener.h>

#include <toadlet/tadpole/terrain/TerrainNode.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>

using namespace toadlet;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::handler;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::sensor;
using namespace toadlet::tadpole::terrain;

#if !defined(TOADLET_FIXED_POINT)
	#include <toadlet/tadpole/bsp/BSP30Handler.h>
	#include <toadlet/tadpole/bsp/BSP30Map.h>
	#include <toadlet/tadpole/bsp/BSP30Node.h>

	#include <toadlet/tadpole/studio/StudioHandler.h>
	#include <toadlet/tadpole/studio/StudioModel.h>
	#include <toadlet/tadpole/studio/StudioModelNode.h>
	#include <toadlet/tadpole/studio/StudioModelController.h>

	using namespace toadlet::tadpole::bsp;
	using namespace toadlet::tadpole::studio;
#endif

#endif
