package{
	import flash.display.Sprite;
	import flash.display.StageScaleMode;
	import flash.display.StageAlign;
	import flash.events.*;
	import flash.text.*;
	import flash.net.*;
	import com.lightningtoads.toadlet.egg.mathfixed.*;
	import com.lightningtoads.toadlet.tadpole.*;
	import com.lightningtoads.toadlet.tadpole.sg.*;
	import com.lightningtoads.toadlet.tadpole.millihop.*;

	public class tadpoleTest extends flash.display.Sprite{
		protected var engine:Engine;
		protected var sceneManager:HopSceneManager;
		protected var camera:CameraEntity;
		protected var light:LightEntity;
		protected var urlStream:URLStream;
		protected var mouseDown:Boolean;
		protected var lastX:Number,lastY:Number;
		protected var rotate:int;
		protected var hopEntity:HopEntity;
protected var text:TextField;
		public function tadpoleTest(){
			super();

			var w:Number=stage.stageWidth,h:Number=stage.stageHeight;

			engine=new Engine(w,h);

			sceneManager=new HopSceneManager(engine);
			engine.setSceneManager(sceneManager);
			engine.setRenderShiftBits(0);
			engine.getRenderer().setOrderingTableSize(2048);
			sceneManager.setGravity(Vector3.Vector3XYZ(0,0,-tMath.milliToFixed(1000)));

			camera=new CameraEntity(engine);
			camera.setLookAt(Vector3.Vector3XYZ(tMath.milliToFixed(300),0,tMath.milliToFixed(100)),tMath.NEG_X_UNIT_VECTOR3,tMath.Z_UNIT_VECTOR3);
			sceneManager.attach(camera);
			resized(null);

			light=new LightEntity(engine);
			light.setDirection(tMath.Y_UNIT_VECTOR3);
			light.setDiffuseColor(0x00FFFFFF);
			sceneManager.attach(light);

			urlStream=new URLStream();
			urlStream.addEventListener(Event.COMPLETE,complete);
			urlStream.addEventListener(IOErrorEvent.IO_ERROR,error);
			urlStream.addEventListener(SecurityErrorEvent.SECURITY_ERROR,error);
			urlStream.load(new URLRequest("http://www.lightningtoads.com/flash/merv.mmsh"));

			stage.scaleMode=StageScaleMode.NO_SCALE;
			stage.align=StageAlign.TOP_LEFT;
			stage.frameRate=60;

			addEventListener("enterFrame",update);
			stage.addEventListener(KeyboardEvent.KEY_DOWN,keyDown);
			stage.addEventListener(KeyboardEvent.KEY_UP,keyUp);
			stage.addEventListener(MouseEvent.MOUSE_DOWN,mouseButtonDown);
			stage.addEventListener(MouseEvent.MOUSE_UP,mouseButtonUp);
			stage.addEventListener(MouseEvent.MOUSE_MOVE,mouseMove);
			stage.addEventListener(Event.RESIZE,resized);
text=new TextField();
addChild(text);
		}

		public function complete(e:Event):void{
			if(urlStream==e.target){
try{
				hopEntity=new HopEntity(engine);
				{
					var mesh:Mesh=engine.loadMicroMeshFromDataInput(urlStream);
					var meshEntity:MeshEntity=new MeshEntity(engine);
					meshEntity.loadM(mesh);
					hopEntity.attach(meshEntity);
					meshEntity.startAnimation(1,true);
				}
				hopEntity.addShape(Shape.ShapeA(AABox.AABoxXYZXYZ(-tMath.milliToFixed(100),-tMath.milliToFixed(100),-tMath.milliToFixed(100),tMath.milliToFixed(100),tMath.milliToFixed(100),tMath.milliToFixed(100))));
				hopEntity.setTranslateXYZ(0,0,tMath.milliToFixed(300));
				hopEntity.setVelocity(Vector3.Vector3XYZ(0,0,tMath.milliToFixed(100)));
				hopEntity.setCoefficientOfRestitution(tMath.ONE);
				sceneManager.attach(hopEntity);

				var world:HopEntity=new HopEntity(engine);
				world.addShape(Shape.ShapeA(AABox.AABoxXYZXYZ(-tMath.milliToFixed(1000),-tMath.milliToFixed(1000),-tMath.milliToFixed(1000),tMath.milliToFixed(1000),tMath.milliToFixed(1000),tMath.milliToFixed(0))));
				world.setTranslateXYZ(0,0,tMath.milliToFixed(-100));
				world.setCoefficientOfGravity(0);
				world.setInfiniteMass();
				world.setCoefficientOfRestitution(tMath.ONE);
				sceneManager.attach(world);

}
catch(e:Error){
text.text=e.toString();
}
			}
		}

		public function error(e:Event):void{
		}	

		public function update(e:Event):void{
try{
			sceneManager.update(16);
			sceneManager.render(graphics,camera);
}
catch(e:Error){
text.text=e.toString();
}
		}

		public function resized(e:Event):void{
			var w:Number=stage.stageWidth,h:Number=stage.stageHeight;
			if(w>=h){
				camera.setProjectionFovY(tMath.degToRad(tMath.intToFixed(90)),tMath.floatToFixed(w/h),tMath.milliToFixed(100),tMath.milliToFixed(600));
			}
			else{
				camera.setProjectionFovX(tMath.degToRad(tMath.intToFixed(90)),tMath.floatToFixed(h/w),tMath.milliToFixed(100),tMath.milliToFixed(600));
			}
			engine.setScreenSize(w,h);
		}
		
		public function keyDown(e:Event):void{
		}

		public function keyUp(e:Event):void{
		}

		public function mouseButtonDown(e:Event):void{
			mouseDown=true;
			lastX=mouseX;
			lastY=mouseY;
		}

		public function mouseButtonUp(e:Event):void{
			mouseDown=false;
		}

		public function mouseMove(e:Event):void{
			if(mouseDown){
				var dx:Number=mouseX-lastX;
				var dy:Number=mouseY-lastY;
				lastX=mouseX;
				lastY=mouseY;
				rotate-=int(dx)<<8;

				var matrix:Matrix3x3=new Matrix3x3();
				tMath.setMatrix3x3FromZ(matrix,rotate);

//				var v:Vector3=Vector3.Vector3V(tMath.Y_UNIT_VECTOR3);
//				tMath.mulVM(v,matrix);
//				light.setDirection(v);

				var pos:Vector3=Vector3.Vector3XYZ(tMath.milliToFixed(300),0,0);
				tMath.mulVM(pos,matrix);
				var dir:Vector3=Vector3.Vector3V(pos);
				tMath.negV(dir);
				tMath.normalizeV(dir);
				pos.z+=tMath.milliToFixed(100);

//				camera.setLookAt(pos,dir,tMath.Z_UNIT_VECTOR3);
				hopEntity.setRotateM(matrix);
			}
		}
	}
}
