from Tkinter import *
from pytoadlet import *
from time import *

window=Tk()
target=new_WGLWindowRenderTarget(0,window.winfo_id(),0)
device=new_GLRenderDevice()
device.create(target,None)

engine=Engine()
engine.setRenderDevice(device)
engine.installHandlers()
scene=Scene(engine)

camera=CameraNode()
camera.create(scene)
camera.setLookAt((0,-50,0,0),(0,0,0,0),(0,0,1,0))
scene.getRoot().attach(camera)

mesh=MeshNode()
mesh.create(scene)
mesh.setMesh("c:\\users\\siralanf\\toadlet\\examples\\tadpole\\python\\lt.tmsh")
scene.getRoot().attach(mesh)

lastTime=time()
window.run=True
def close():
	window.run=False
	window.destroy()
window.protocol("WM_DELETE_WINDOW",close)

while window.run:
	newTime=time();
	dt=(newTime-lastTime)*1000;

	window.update()
	
	scene.update(10)

	device.beginScene()
	camera.render(device)
	device.endScene()
	device.swap()
	
	lastTime=newTime
