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
camera.setLookAt((0,-50,0),(0,0,0),(0,0,1))
scene.getRoot().attach(camera)

mesh=MeshNode()
mesh.create(scene)
mesh.setMesh("c:\\users\\siralanf\\toadlet\\examples\\tadpole\\python\\lt.tmsh")
scene.getRoot().attach(mesh)

mesh.totalRotate=0.0

def close():
	window.run=False
	window.destroy()
window.protocol("WM_DELETE_WINDOW",close)

def down(event):
	window.lastx=event.x
window.bind("<Button-1>",down)

def drag(event):
	mesh.totalRotate+=(event.x-window.lastx)/100.0
	mesh.setRotate((0,0,1),mesh.totalRotate)
	window.lastx=event.x;
window.bind("<B1-Motion>",drag)

lastTime=time()
window.run=True
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
