#ifndef TOADLETWIDGET_H
#define TOADLETWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <toadlet/toadlet.h>

class ToadletWidget:public QWidget{
public:
	inline ToadletWidget(QWidget *parent);
	inline ~ToadletWidget();

	inline void create(bool render,bool audio);

	inline QPaintEngine *paintEngine() const{return NULL;}

	inline RenderTarget *getRenderTarget(){return renderTarget;}
	inline RenderDevice *getRenderDevice(){return renderDevice;}
	inline AudioDevice *getAudioDevice(){return audioDevice;}

protected:
	RenderTarget::ptr renderTarget;
	RenderDevice::ptr renderDevice;
	AudioDevice::ptr audioDevice;
};

#include "ToadletWidget.cpp"

#endif
