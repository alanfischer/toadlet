%{
#include <toadlet/tadpole/Track.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Track "$this->retain();"
%unrefobject Track "$this->release();"

class Track{
public:
	void compile();

	int addKeyFrame(scalar time,float frame[]=NULL);
	bool getKeyFrame(float frame[],int index);

	int getKeyFrameSize() const;
	int getNumKeyFrames() const;

	scalar getTime(int i);

	void setIndex(int index);
	int getIndex() const;

	void setLength(scalar length);
	scalar getLength() const;
	
protected:
	Track();
};

}
}
