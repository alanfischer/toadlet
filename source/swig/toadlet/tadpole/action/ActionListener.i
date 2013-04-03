%{
#include <toadlet/tadpole/action/ActionListener.h>
%}

// @todo: Reenable this when dynamic_cast is supported by default on android, or write custom java directors
// #define ACTIONLISTENER_DIRECTOR

#ifdef ACTIONLISTENER_DIRECTOR
%feature("director") toadlet::tadpole::action::ActionListener;
#endif
%rename(NativeActionListener) toadlet::tadpole::action::ActionListener; 
%typemap(jstype) toadlet::tadpole::action::ActionListener* "ActionListener";
%typemap(javainterfaces) toadlet::tadpole::action::ActionListener "ActionListener";
%typemap(javain,pgcppname="n",
	pre="NativeActionListener n = tadpole.makeNative($javainput);")
	toadlet::tadpole::action::ActionListener* "NativeActionListener.getCPtr(n)"
	
namespace toadlet{
namespace tadpole{
namespace action{

class ActionListener{
public:
	virtual ~ActionListener(){}

	virtual void actionStarted(Action *action)=0;
	virtual void actionStopped(Action *action)=0;
};

}
}
}

#ifdef ACTIONLISTENER_DIRECTOR
%pragma(java) modulecode=%{
	private static class NativeActionListenerProxy extends NativeActionListener {
		private ActionListener delegate;
		public NativeActionListenerProxy(ActionListener i) {delegate = i;}

		public void actionStarted(Action action){
			delegate.actionStarted(action);
		}
		
		public void actionStopped(Action action){
			delegate.actionStopped(action);
		}
	}

	public static NativeActionListener makeNative(ActionListener i) {
		if (i instanceof NativeActionListener) {
			return (NativeActionListener)i;
		}
		return new NativeActionListenerProxy(i);
	}
%}
#else
%pragma(java) modulecode=%{
	public static NativeActionListener makeNative(ActionListener i) {
		if (i instanceof NativeActionListener) {
			return (NativeActionListener)i;
		}
		return new NativeActionListener(0,false);
	}
%}
#endif
