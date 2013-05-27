#ifndef TOADLETWIDGET_H
#define TOADLETWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <toadlet/toadlet.h>

class ToadletWidget:public QWidget{
	Q_OBJECT
public:
	ToadletWidget(QWidget *parent);
	virtual ~ToadletWidget();

	void create(bool render,bool audio);

	QPaintEngine *paintEngine() const{return NULL;}

	inline RenderTarget *getRenderTarget(){return renderTarget;}
	inline RenderDevice *getRenderDevice(){return renderDevice;}
	inline AudioDevice *getAudioDevice(){return audioDevice;}

protected:
	RenderTarget::ptr renderTarget;
	RenderDevice::ptr renderDevice;
	AudioDevice::ptr audioDevice;
};

#endif
