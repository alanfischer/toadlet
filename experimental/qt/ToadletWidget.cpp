#include "ToadletWidget.h"
#if defined (TOADLET_PLATFORM_POSIX)
	#include <QX11Info>
#endif

#if defined(TOADLET_PLATFORM_WIN32)
	TOADLET_C_API RenderTarget *new_WGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
	TOADLET_C_API RenderDevice *new_GLRenderDevice();
	TOADLET_C_API RenderTarget *new_D3D9WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
	TOADLET_C_API RenderDevice *new_D3D9RenderDevice();
	TOADLET_C_API RenderTarget *new_D3D10WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
	TOADLET_C_API RenderDevice *new_D3D10RenderDevice();
	TOADLET_C_API AudioDevice *new_MMAudioDevice();
	TOADLET_C_API AudioDevice *new_ALAudioDevice();
#elif defined(TOADLET_PLATFORM_OSX)
	TOADLET_C_API RenderTarget *new_NSGLRenderTarget(void *display,void *view,WindowRenderTargetFormat *format);
    TOADLET_C_API RenderDevice *new_GLRenderDevice();
	TOADLET_C_API AudioDevice *new_ALAudioDevice();
#elif defined(TOADLET_PLATFORM_POSIX)
	TOADLET_C_API RenderTarget *new_GLXWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
    TOADLET_C_API RenderDevice *new_GLRenderDevice();
	TOADLET_C_API AudioDevice *new_ALAudioDevice();
#endif

ToadletWidget::ToadletWidget(QWidget *parent):QWidget(parent),
	renderTarget(NULL),
	renderDevice(NULL),
	audioDevice(NULL)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
}

ToadletWidget::~ToadletWidget(){
	if(renderDevice!=NULL){
		renderDevice->destroy();
		renderDevice=NULL;
	}

	if(renderTarget!=NULL){
		renderTarget->destroy();
		renderTarget=NULL;
	}

	if(audioDevice!=NULL){
		audioDevice->destroy();
		audioDevice=NULL;
	}
}

void ToadletWidget::create(bool render,bool audio){
	WindowRenderTargetFormat::ptr format(new WindowRenderTargetFormat(TextureFormat::Format_RGB_5_6_5,16));
	#if defined(TOADLET_DEBUG)
		format->debug=true;
	#endif

	if(render){
		bool result=false;
		#if defined(TOADLET_PLATFORM_WIN32)
			#if 0
				if(result==false){
					TOADLET_TRY
						renderTarget=new_D3D10WindowRenderTarget(NULL,winId(),format);
						renderDevice=new_D3D10RenderDevice();
						if(renderTarget!=NULL && renderDevice!=NULL){
							result=renderDevice->create(renderTarget,NULL);
						}
						else{
							result=false;
						}
					TOADLET_CATCH_ANONYMOUS(){result=false;}
				}
			#endif
			#if 0
				if(result==false){
					TOADLET_TRY
						renderTarget=new_D3D9WindowRenderTarget(NULL,winId(),format);
						renderDevice=new_D3D9RenderDevice();
						if(renderTarget!=NULL && renderDevice!=NULL){
							result=renderDevice->create(renderTarget,NULL);
						}
						else{
							result=false;
						}
					TOADLET_CATCH_ANONYMOUS(){result=false;}
				}
			#endif
			#if 1
				if(result==false){
					TOADLET_TRY
						renderTarget=new_WGLWindowRenderTarget(NULL,winId(),format);
						renderDevice=new_GLRenderDevice();
						if(renderTarget!=NULL && renderDevice!=NULL){
							result=renderDevice->create(renderTarget,NULL);
						}
						else{
							result=false;
						}
					TOADLET_CATCH_ANONYMOUS(){result=false;}
				}
			#endif
		#elif defined(TOADLET_PLATFORM_OSX)
			if(result==false){
                TOADLET_TRY
                    renderTarget=new_NSGLRenderTarget(NULL,(void*)winId(),format);
                    renderDevice=new_GLRenderDevice();
                    if(renderTarget!=NULL && renderDevice!=NULL){
                        result=renderDevice->create(renderTarget,NULL);
                    }
                    else{
                        result=false;
                    }
                TOADLET_CATCH_ANONYMOUS(){result=false;}
            }
		#elif defined(TOADLET_PLATFORM_POSIX)
			if(result==false){
				TOADLET_TRY
					renderTarget=new_GLXWindowRenderTarget(x11Info().display(),(void*)winId(),format);
					renderDevice=new_GLRenderDevice();
					if(renderTarget!=NULL && renderDevice!=NULL){
						result=renderDevice->create(renderTarget,NULL);
					}
					else{
						result=false;
					}
				TOADLET_CATCH_ANONYMOUS(){result=false;}
			}
		#else
		#endif
		if(result==false){
			qWarning() << "Unable to create RenderDevice";
			QMessageBox::critical(this,"Error","Unable to create RenderDevice");
		}
	}

	if(audio){
		bool result=false;
		#if defined (TOADLET_PLATFORM_WIN32)
			#if 0
				if(result==false){
					audioDevice=new_ALAudioDevice();
					if(audioDevice!=NULL){
						TOADLET_TRY
							result=audioDevice->create(NULL);
						TOADLET_CATCH_ANONYMOUS(){result=false;}
					}
				}
			#endif
			#if 1
				if(result==false){
					audioDevice=new_MMAudioDevice();
					if(audioDevice!=NULL){
						TOADLET_TRY
							result=audioDevice->create(NULL);
						TOADLET_CATCH_ANONYMOUS(){result=false;}
					}
				}
			#endif
		#elif defined (TOADLET_PLATFORM_OSX)
			if(result==false){
                audioDevice=new_ALAudioDevice();
                if(audioDevice!=NULL){
                    TOADLET_TRY
                        result=audioDevice->create(NULL);
                    TOADLET_CATCH_ANONYMOUS(){result=false;}
                }
            }
		#elif defined (TOADLET_PLATFORM_POSIX)
			if(result==false){
				audioDevice=new_ALAudioDevice();
				if(audioDevice!=NULL){
					TOADLET_TRY
						result=audioDevice->create(NULL);
					TOADLET_CATCH_ANONYMOUS(){result=false;}
				}
			}
		#endif
	}
}
