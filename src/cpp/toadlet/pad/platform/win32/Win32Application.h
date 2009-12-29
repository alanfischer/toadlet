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

#ifndef TOADLET_PAD_WIN32APPLICATION_H
#define TOADLET_PAD_WIN32APPLICATION_H

#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

struct Win32Attributes;

class TOADLET_API Win32Application:public BaseApplication{
public:
	enum RendererPlugin{
		RendererPlugin_NONE,
		RendererPlugin_OPENGL,
		RendererPlugin_DIRECT3D9,
		RendererPlugin_DIRECT3D10,
	};

	Win32Application();
	virtual ~Win32Application();

	virtual void create();
	virtual void destroy();

	virtual bool start(bool runEventLoop);
	virtual void stepEventLoop();
	virtual void stop();
	virtual bool isRunning() const{return mRun;}

	virtual void setAutoActivate(bool autoActivate);
	virtual bool getAutoActivate() const{return mAutoActivate;}
	virtual void activate();
	virtual void deactivate();
	virtual bool isActive() const{return mActive;}

	virtual void setTitle(const egg::String &title);
	virtual const egg::String &getTitle() const;

	virtual void setPosition(int x,int y);
	virtual int getPositionX() const;
	virtual int getPositionY() const;

	virtual void setSize(int width,int height);
	virtual int getWidth() const;
	virtual int getHeight() const;

	virtual void setFullscreen(bool fullscreen);
	virtual bool getFullscreen() const;

	virtual void setVisual(const peeper::Visual &visual);
	virtual const peeper::Visual &getVisual() const;

	virtual void setApplicationListener(ApplicationListener *listener);
	virtual ApplicationListener *getApplicationListener() const;

	virtual tadpole::Engine *getEngine() const{return mEngine;}
	virtual peeper::Renderer *getRenderer() const{return mRenderer;}
	virtual ribbit::AudioPlayer *getAudioPlayer() const{return mAudioPlayer;}

	virtual peeper::RenderTarget *getRootRenderTarget(){return mRenderTarget;}
	virtual bool isPrimary() const{return mRenderTarget->isPrimary();}
	virtual bool isValid() const{return mRenderTarget->isValid();}

	virtual void resized(int width,int height);
	virtual void focusGained();
	virtual void focusLost();
	virtual void keyPressed(int key);
	virtual void keyReleased(int key);
	virtual void mousePressed(int x,int y,int button);
	virtual void mouseMoved(int x,int y);
	virtual void mouseReleased(int x,int y,int button);
	virtual void mouseScrolled(int x,int y,int scroll);
	virtual void update(int dt);
	virtual void render(peeper::Renderer *renderer);

	void setMouseLocked(bool locked);

	void changeRendererPlugin(int index);
	void setRendererOptions(int *options,int length);

	void setIcon(void *icon);
	void *getHINSTANCE() const;
	void *getHWND() const;

	void internal_resize(int width,int height);

	typedef egg::Map<void*,Win32Application*> ApplicationMap;
	static int mWindowCount;
	static ApplicationMap mApplicationMap;
	static int translateKey(int key);

protected:
	bool createWindow();
	void destroyWindow();
	peeper::RenderTarget *makeRenderTarget(int plugin);
	peeper::Renderer *makeRenderer(int plugin);
	bool createContextAndRenderer(int plugin);
	bool destroyRendererAndContext();
	bool createAudioPlayer();
	bool destroyAudioPlayer();
	bool changeVideoMode(int width,int height,int colorBits);

	egg::String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	peeper::Visual mVisual;
	ApplicationListener *mApplicationListener;
	bool mMouseLocked;

	tadpole::Engine *mEngine;
	peeper::RenderTarget *mRenderTarget;
	peeper::Renderer *mRenderer;
	int mRendererPlugin;
	int mChangeRendererPlugin;
	int *mRendererOptions;
	ribbit::AudioPlayer *mAudioPlayer;

	bool mRun;
	bool mAutoActivate;
	bool mActive;
	bool mDestroyed;

	Win32Attributes *win32;
};

}
}

#endif
