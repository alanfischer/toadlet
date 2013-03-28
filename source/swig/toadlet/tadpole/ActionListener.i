%{
#include <toadlet/tadpole/ActionListener.h>
%}

%feature("director") ActionListener;
%rename(NativeActionListener) ActionListener; 
%typemap(jstype) ActionListener* "ActionListener";
%typemap(javainterfaces) ActionListener "ActionListener";
%typemap(javain,pgcppname="n",
	pre="NativeActionListener n = makeNative($javainput);")
	ActionListener* "NativeActionListener.getCPtr(n)"

namespace toadlet{
namespace tadpole{

class ActionListener{
public:
	virtual ActionListener(){}

	virtual void actionStarted(Action *action)=0;
	virtual void actionStopped(Action *action)=0;
};

}
}

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
