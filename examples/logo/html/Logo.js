var Logo=function(engine){ return{
	engine: engine,

	create: function(){
		this.scene=new toadlet.Scene(this.engine);

		var node=new toadlet.Node(this.scene);
		{
			var light=new toadlet.LightComponent();
			var state=new toadlet.LightState();
			state.type=toadlet.LightType.Type_DIRECTION;
			state.direction=toadlet.NEG_Z_UNIT_VECTOR3;
			light.setLightState(state);
			node.attach(light);
		}
		this.scene.getRoot().attach(node);

		var lt=new toadlet.Node(this.scene);
		{
			var mesh=new toadlet.MeshComponent(this.engine);
			lt.attach(mesh);

			var request=toadlet.ResourceRequest.implement({
				resourceReady: function(resource){
					if(resource){
						mesh.setMeshWithMesh(toadlet.toMesh(resource));
						var animation=new toadlet.AnimationAction(mesh.getSkeleton().getAnimationWithIndex(0));
						animation.setCycling(toadlet.Cycling.Cycling_REFLECT);
						lt.attach(new toadlet.ActionComponent("animation",animation));
					}

					lt.startAction("animation");
				},
				resourceException: function(ex){
					console.log(ex.getError());
				}
			});

			this.engine.getMeshManager().find("lt.tmsh",request,null);
		}
		this.scene.getRoot().attach(lt);

		node=new toadlet.Node(this.scene);
		{
			this.camera=new toadlet.CameraComponent(new toadlet.Camera(this.engine));
			this.camera.setClearColor(toadlet.BLUE);
			node.attach(this.camera);
			this.camera.setLookAt([0,-150,0],toadlet.ZERO_VECTOR3,toadlet.Z_UNIT_VECTOR3);
		}
		this.scene.getRoot().attach(node);
	},

	destroy: function(){
		this.scene.destroy();
	},

	render: function(){
		var renderDevice=this.engine.getRenderDevice();

		renderDevice.beginScene();
			this.camera.render(renderDevice,this.scene,null);
		renderDevice.endScene();
		renderDevice.swap();
	},

	update: function(dt){
		this.scene.update(dt);
	}
}};
