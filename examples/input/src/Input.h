#ifndef INPUT_H
#define INPUT_H

#include <toadlet/toadlet.h>

class Input:public Object,public Applet,public InputDeviceListener{
public:
	TOADLET_OBJECT(Input);

	Input(Application *app);
	virtual ~Input();

	void create();
	void destroy();
	void render();
	void update(int dt);

	void resized(int width,int height){}
	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key){}
	void keyReleased(int key){}

	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}
	
	Node::ptr makeLabel(const String &label);
	Node::ptr makeNeedle();
	void setNeedle(Node::ptr needle,float value);

	void inputDetected(const InputData &data);

	void startLogging(Stream::ptr stream);
	void stopLogging();
	
	Application *app;
	Engine *engine;
	Scene::ptr scene;
	Camera::ptr camera;
	Node::ptr linearLabel,proximityLabel,lightLabel;
	Node::ptr linearNeedle[3],proximityNeedle,lightNeedle;
};

#endif
