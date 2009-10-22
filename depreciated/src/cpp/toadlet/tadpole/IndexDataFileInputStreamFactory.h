#ifndef TOADLET_TADPOLE_INDEXDATAFILEINPUTSTREAMFACTORY_H
#define TOADLET_TADPOLE_INDEXDATAFILEINPUTSTREAMFACTORY_H

#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/InputStreamFactory.h>
#include <stdio.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API IndexDataFileInputStreamFactory:public egg::io::InputStreamFactory{
public:
	IndexDataFileInputStreamFactory();
	~IndexDataFileInputStreamFactory();

	bool init(const egg::String &indexFile,const egg::String &dataFile);

	egg::io::InputStream::ptr makeInputStream(const egg::String &name);

protected:
	class Index{
	public:
		int position;
		int length;
	};

	egg::Map<egg::String,Index> mIndex;
	FILE *mData;
};

}
}

#endif
