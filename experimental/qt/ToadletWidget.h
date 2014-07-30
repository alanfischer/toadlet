#ifndef TOADLETWIDGET_H
#define TOADLETWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <toadlet/toadlet.h>

namespace toadlet{

class ToadletWidget:public QWidget{
public:
	inline ToadletWidget(QWidget *parent);
	inline ~ToadletWidget();

	inline void create(bool render,bool audio,RenderTarget *shareTarget=NULL);

	inline QPaintEngine *paintEngine() const{return NULL;}

	inline RenderTarget *getRenderTarget(){return renderTarget;}
	inline RenderDevice *getRenderDevice(){return renderDevice;}
	inline AudioDevice *getAudioDevice(){return audioDevice;}

protected:
	RenderTarget::ptr renderTarget;
	RenderDevice::ptr renderDevice;
	AudioDevice::ptr audioDevice;
};

}

#include "ToadletWidget.cpp"

#endif
