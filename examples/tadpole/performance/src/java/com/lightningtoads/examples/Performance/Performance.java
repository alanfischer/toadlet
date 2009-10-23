package com.lightningtoads.examples.Performance;

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.entity.*;
import com.lightningtoads.toadlet.pad.*;

/*
import com.lightningtoads.toadlet.egg.math.*;
import com.lightningtoads.toadlet.egg.math.Math;
*/
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public class Performance extends Application{
    public Performance(){
		super();
	}
	
	public void create(){
		super.create();
	
		mScene=new Scene();
		mScene.create(mEngine);
		mEngine.setScene(mScene);

		mCamera=(CameraEntity)mEngine.createEntity(CameraEntity.class);
		mCamera.setLookAt(new Vector3(0,-Math.fromInt(5),0),Math.ZERO_VECTOR3,Math.Z_UNIT_VECTOR3);
		mCamera.setClearColor(Colors.BLUE);
		mScene.attach(mCamera);

		setupTest(mTest++,10);
	}

	public void update(int dt){
		mNode.setRotate(0,0,Math.ONE,Math.fromMilli(mScene.getVisualTime())*4);

		mScene.update(dt);
	}
	
	public void render(Renderer renderer){
		renderer.beginScene();

		mScene.render(renderer,mCamera);

		renderer.endScene();

		renderer.swap();

		//setTitle("FPS:"+(int)(mCamera.getFramesPerSecond()/Math.ONE));
	}
	
	public void mousePressed(int x,int y,int button){
		if(setupTest(mTest++,0)==false){
			stop();
		}
	}
	
	public void resized(int width,int height){
		if(width>=height){
			mCamera.setProjectionFovY(Math.degToRad(Math.fromInt(45)),Math.div(Math.fromInt(width),Math.fromInt(height)),Math.fromMilli(100),Math.fromInt(100));
		}
		else{
			mCamera.setProjectionFovX(Math.degToRad(Math.fromInt(45)),Math.div(Math.fromInt(height),Math.fromInt(width)),Math.fromMilli(100),Math.fromInt(100));
		}
	}

	protected boolean setupTest(int test,int intensity){
		if(mNode!=null){
			mNode.destroy();
		}
		mNode=(ParentEntity)mEngine.createEntity(ParentEntity.class);
		mScene.attach(mNode);

		boolean result=true;
		for(int i=0;i<intensity+1;++i){
			switch(test){
				case 0:
					mNode.attach(setupMinimumTest());
				break;
				case 1:
					mNode.attach(setupFillrateTest());
				break;
				case 2:
					mNode.attach(setupVertexrateTest());
				break;
				case 3:
					mNode.attach(setupPointSpriteTest());
				break;
				default:
					result=false;
				break;
			}
		}

		return result;
	}

	protected Entity setupMinimumTest(){
		MeshEntity cubeEntity=(MeshEntity)mEngine.createEntity(MeshEntity.class);
		cubeEntity.load(makeCube(mEngine));
		return cubeEntity;
	}

	protected Entity setupFillrateTest(){
		int i;
		Mesh cubeMesh=makeCube(mEngine);
		ParentEntity node=(ParentEntity)mEngine.createEntity(ParentEntity.class);

		for(i=0;i<10;++i){
			MeshEntity cubeEntity=(MeshEntity)mEngine.createEntity(MeshEntity.class);
			cubeEntity.load(cubeMesh);
			cubeEntity.setScale(Math.fromInt(5),Math.fromInt(5),Math.fromInt(5));
			node.attach(cubeEntity);
		}

		for(i=0;i<node.getNumChildren();++i){
			node.getChild(i).setRotate(0,0,Math.ONE,Math.div(Math.fromInt(i)*Math.TWO_PI,Math.fromInt(node.getNumChildren())));
		}

		return node;
	}

	protected Entity setupVertexrateTest(){
		MeshEntity sphereEntity=(MeshEntity)mEngine.createEntity(MeshEntity.class);
		sphereEntity.load(makeSphere(mEngine,7));
		return sphereEntity;
	}

	class PointSpriteEntity extends RenderableEntity implements Renderable{
		public PointSpriteEntity(){}
		
		public Entity create(Engine engine){
			super.create(engine);
		
			VertexBuffer vertexBuffer=engine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,engine.getVertexFormats().POSITION_COLOR,4));
			{
				VertexBufferAccessor vba=new VertexBufferAccessor();
				vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

				vba.set3(0,0,-Math.ONE,-Math.ONE, Math.ONE); vba.setABGR(0,1,0x0000FF00);
				vba.set3(1,0, Math.ONE,-Math.ONE, Math.ONE); vba.setABGR(1,1,0x000000FF);
				vba.set3(2,0,-Math.ONE,Math.ONE,-Math.ONE);  vba.setABGR(2,1,0x0000FF00);
				vba.set3(3,0, Math.ONE,-Math.ONE,-Math.ONE); vba.setABGR(3,1,0x00FF00FF);

				vba.unlock();
			}
			mVertexData=new VertexData(vertexBuffer);
			
			mIndexData=new IndexData(IndexData.Primitive.POINTS,null,0,4);
			
			mMaterial=new Material();
			mMaterial.setFaceCulling(Renderer.FaceCulling.NONE);
			
			return this;
		}
		
		public void queueRenderables(Scene scene){
			scene.queueRenderable(this);
		}
		
		public void render(Renderer renderer){
			renderer.setPointParameters(true,Math.fromInt(10),false,0,0,0,0,0);
			renderer.renderPrimitive(mVertexData,mIndexData);
		}
		
		public Matrix4x4 getRenderTransform(){return mVisualWorldTransform;}
		public Material getRenderMaterial(){return mMaterial;}

		protected VertexData mVertexData;
		protected IndexData mIndexData;
		protected Material mMaterial;
	}
	
	protected Entity setupPointSpriteTest(){
		return (new PointSpriteEntity()).create(mEngine);
	}
	
	protected static Mesh makeCube(Engine engine){
		VertexBuffer vertexBuffer=engine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,engine.getVertexFormats().POSITION_COLOR,8));
		{
			VertexBufferAccessor vba=new VertexBufferAccessor();
			vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

			int i=0;
			vba.set3(i,0,-Math.HALF,-Math.HALF,-Math.HALF); vba.setRGBA(i,1,0xFF0000FF); i++;
			vba.set3(i,0, Math.HALF,-Math.HALF,-Math.HALF); vba.setRGBA(i,1,0x00FF00FF); i++;
			vba.set3(i,0,-Math.HALF, Math.HALF,-Math.HALF); vba.setRGBA(i,1,0x0000FFFF); i++;
			vba.set3(i,0, Math.HALF, Math.HALF,-Math.HALF); vba.setRGBA(i,1,0xFFFF00FF); i++;
			vba.set3(i,0,-Math.HALF,-Math.HALF, Math.HALF); vba.setRGBA(i,1,0xFF00FFFF); i++;
			vba.set3(i,0, Math.HALF,-Math.HALF, Math.HALF); vba.setRGBA(i,1,0x00FFFFFF); i++;
			vba.set3(i,0,-Math.HALF, Math.HALF, Math.HALF); vba.setRGBA(i,1,0xFFFFFFFF); i++;
			vba.set3(i,0, Math.HALF, Math.HALF, Math.HALF); vba.setRGBA(i,1,0x000000FF); i++;

			vba.unlock();
		}

		IndexBuffer indexBuffer=engine.loadIndexBuffer(new IndexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,IndexBuffer.IndexFormat.UINT_16,36));
		{
			IndexBufferAccessor iba=new IndexBufferAccessor();
			iba.lock(indexBuffer,Buffer.LockType.WRITE_ONLY);

			int i=0;
			// Bottom
			iba.set(i++,2);		iba.set(i++,1);		iba.set(i++,0);
			iba.set(i++,3);		iba.set(i++,1);		iba.set(i++,2);

			// Top
			iba.set(i++,4);		iba.set(i++,5);		iba.set(i++,6);
			iba.set(i++,6);		iba.set(i++,5);		iba.set(i++,7);

			// Left
			iba.set(i++,4);		iba.set(i++,2);		iba.set(i++,0);
			iba.set(i++,6);		iba.set(i++,2);		iba.set(i++,4);

			// Right
			iba.set(i++,1);		iba.set(i++,3);		iba.set(i++,5);
			iba.set(i++,5);		iba.set(i++,3);		iba.set(i++,7);

			// Front
			iba.set(i++,0);		iba.set(i++,1);		iba.set(i++,4);
			iba.set(i++,4);		iba.set(i++,1);		iba.set(i++,5);

			// Back
			iba.set(i++,6);		iba.set(i++,3);		iba.set(i++,2);
			iba.set(i++,7);		iba.set(i++,3);		iba.set(i++,6);
		}

		Mesh.SubMesh subMesh=new Mesh.SubMesh();
		subMesh.indexData=new IndexData(IndexData.Primitive.TRIS,indexBuffer);
		subMesh.material=new Material();
		subMesh.material.setFaceCulling(Renderer.FaceCulling.BACK);

		Mesh mesh=new Mesh();
		mesh.subMeshes=new Mesh.SubMesh[1];
		mesh.subMeshes[0]=subMesh;
		mesh.staticVertexData=new VertexData(vertexBuffer);

		return mesh;
	}
	
	protected static Mesh makeSphere(Engine engine,int bandPower){
		int bandPoints=1<<bandPower;
		int bandMask=bandPoints-2;
		int sectionsInBand=(bandPoints/2)-1;
		int totalPoints=sectionsInBand*bandPoints;
		int/*scalar*/ sectionArc=Math.TWO_PI/sectionsInBand;
		int/*scalar*/ radius=Math.HALF;

		VertexBuffer vertexBuffer=engine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,engine.getVertexFormats().POSITION_COLOR,totalPoints));
		{
			VertexBufferAccessor vba=new VertexBufferAccessor();
			vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

			int i=0,j=0;
			int/*scalar*/ xAngle=0,yAngle=0;
			for(i=0;i<totalPoints;++i){
				xAngle=Math.fromInt((i&1)+(i>>bandPower));
				yAngle=Math.fromInt(((i&bandMask)>>1)+((i>>bandPower)*(sectionsInBand)));
				xAngle=Math.mul(xAngle,sectionArc/2);
				yAngle=Math.mul(yAngle,-sectionArc);

				vba.set3(j,0,
					Math.mul(-radius,Math.mul(Math.sin(xAngle),Math.sin(yAngle))),
					Math.mul(-radius,Math.cos(xAngle)),
					Math.mul(-radius,Math.mul(Math.sin(xAngle),Math.cos(yAngle))));
				vba.setRGBA(j,1,
					Color.lerp(Colors.RED.getRGBA(),Colors.BLUE.getRGBA(),Math.div(Math.fromInt(i),Math.fromInt(totalPoints))));
				j++;
			}
		}

		Mesh.SubMesh subMesh=new Mesh.SubMesh();
		subMesh.indexData=new IndexData(IndexData.Primitive.TRISTRIP,null,0,vertexBuffer.getSize());
		subMesh.material=new Material();
		subMesh.material.setFaceCulling(Renderer.FaceCulling.BACK);

		Mesh mesh=new Mesh();
		mesh.subMeshes=new Mesh.SubMesh[1];
		mesh.subMeshes[0]=subMesh;
		mesh.staticVertexData=new VertexData(vertexBuffer);

		return mesh;
	}

	protected Scene mScene;
	protected CameraEntity mCamera;
	protected ParentEntity mNode;
	protected int mTest;
}
