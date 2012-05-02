from Tkinter import *
from pytoadlet import *
from time import *

window=Tk()
renderDevice=new_GLRenderDevice()
renderDevice.create(new_WGLWindowRenderTarget(0,window.winfo_id(),0),None)

audioDevice=new_ALAudioDevice()
audioDevice.create(None)

engine=Engine()
engine.setRenderDevice(renderDevice)
engine.setAudioDevice(audioDevice)
engine.installHandlers()
scene=Scene(engine)

camera=CameraNode()
camera.create(scene)
camera.setLookAt((25,-150,0),(25,0,0),(0,0,1))
scene.getRoot().attach(camera)

audio=AudioComponent(engine)
audio.setAudioBuffer("C:\\users\\siralanf\\toadlet\\examples\\data\\boing.wav")
camera.attach(audio);

mesh=MeshNode()
mesh.create(scene)
mesh.setMesh("c:\\users\\siralanf\\toadlet\\examples\\tadpole\\python\\lt.tmsh")
scene.getRoot().attach(mesh)

animate=AnimationActionComponent("animate")
animate.attach(MeshAnimation(mesh,0))
animate.setCycling(AnimationActionComponent.Cycling_REFLECT);
animate.setStopGently(True)
animate.start();
mesh.attach(animate)

class BoingListener(ActionListener):
        def actionStarted(self,action):
                audio.play()
        def actionStopped(self,action):
                return
boing=BoingListener()
animate.setActionListener(boing)

class MoveAnimation(BaseAnimation):
	def setValue(self,value):
		mesh.setTranslate(0,0,value*20);
	def getMinValue(self):
		return 0.0
	def getMaxValue(self):
		return 0.0
	def getValue(self):
		return 0.0
	def getWeight(self):
		return 0.0
animation=MoveAnimation()
animate.attach(animation)

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

def key(event):
	if animate.getActive():
		animate.stop()
	else:
		animate.start()
window.bind("<Key>",key)

lastTime=time()
window.run=True
while window.run:
	newTime=time();
	dt=int((newTime-lastTime)*1000+0.5);

	window.update()
	scene.update(dt)

	renderDevice.beginScene()
	camera.render(renderDevice)
	renderDevice.endScene()
	renderDevice.swap()

	audioDevice.update(dt)
	
	lastTime=newTime
