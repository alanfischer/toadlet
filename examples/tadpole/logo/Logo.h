#ifndef LOGO_H
#define LOGO_H

#include <toadlet/toadlet.h>

class Logo:public Object,public Applet{
public:
	TOADLET_OBJECT(Logo);

	Logo(Application *app);
	virtual ~Logo();

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
	
	Application *app;
	Engine::ptr engine;
	Scene::ptr scene;
	CameraComponent::ptr camera;
};

#endif
