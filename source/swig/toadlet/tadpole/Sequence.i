%{
#include <toadlet/tadpole/Sequence.h>
%}

namespace toadlet{
namespace tadpole{

%refobject Sequence "$this->retain();"
%unrefobject Sequence "$this->release();"

class Sequence{
public:
	void destroy();
	void compile();

	int getNumTracks() const;
	void addTrack(Track *track);
	void removeTrack(int i);
	Track *getTrack(int i);

	scalar getLength() const;
	void setLength(scalar length);
};

}
}
