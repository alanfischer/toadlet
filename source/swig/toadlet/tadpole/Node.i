%{
#include <toadlet/tadpole/Node.h>
%}

namespace toadlet{
namespace tadpole{

class Node:public Component{
public:
	Node(Scene *scene);
	void destroy();

	Node *getParent();

	void setName(String name);
	String getName() const;

	bool getActive() const;

	void setTranslate(float x,float y,float z);
	void setRotate(float axis[3],float angle);

	void setScope(int scope);
	int getScope() const;

	bool attach(Component *node);
	bool remove(Component *node);

	ActionComponent *getAction(String name);
	void startAction(String name);
	void stopAction(String name);
	bool getActionActive(String name);
	
	VisibleRange getVisibles() const;
};

}
}

// This is from: http://stackoverflow.com/questions/9465856/no-iterator-for-java-when-using-swig-with-cs-stdmap

%typemap(javainterfaces) VisibleIterator "java.util.Iterator<Visible>"
%typemap(javacode) VisibleIterator %{
public void remove() throws UnsupportedOperationException { throw new UnsupportedOperationException(); }
public Visible next() throws java.util.NoSuchElementException { if (!hasNext()) { throw new java.util.NoSuchElementException(); } return nextImpl(); }
%}

%javamethodmodifiers VisibleIterator::nextImpl "private";
%inline %{
class VisibleIterator:public RangeIterator<toadlet::tadpole::VisibleRange>{
public:
	VisibleIterator(const toadlet::tadpole::VisibleRange& r) : RangeIterator(r) {}
	bool hasNext() const{return RangeIterator::hasNext();}
	toadlet::tadpole::Visible *nextImpl(){return RangeIterator::nextImpl();}
};
%}

%typemap(javainterfaces) toadlet::tadpole::VisibleRange "Iterable<Visible>"

namespace toadlet{namespace tadpole{class VisibleRange{};}}

%newobject toadlet::tadpole::VisibleRange::iterator() const;
%extend toadlet::tadpole::VisibleRange {
	VisibleIterator *iterator() const {
		return new VisibleIterator(*$self);
	}
}
