%module pytoadlet


%include "egg/Logger.i"
%include "egg/math/Vector3.i"
%include "egg/math/AABox.i"
%{
using namespace toadlet::egg;
using namespace toadlet::egg::math;
%}

%include "hop/Solid.i"
%include "hop/Shape.i"
%{
using namespace toadlet::hop;
%}

%include "peeper/RenderContext.i"
%include "peeper/Renderer.i"
%{
using namespace toadlet::peeper;
%}

%include "tadpole/Engine.i"
%include "tadpole/sg/Entity.i"
%include "tadpole/sg/CameraEntity.i"
%include "tadpole/sg/LightEntity.i"
%include "tadpole/sg/NodeEntity.i"
%include "tadpole/sg/MeshEntity.i"
%include "tadpole/sg/SceneManager.i"
%include "tadpole/plugins/hop/HopSceneManager.i"
%include "tadpole/plugins/hop/HopEntity.i"
%{
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;
%}

%include "goo/Application.i"
%{
using namespace toadlet::goo;
%}
