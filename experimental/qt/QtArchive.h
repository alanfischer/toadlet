#ifndef QTARCHIVE_H
#define QTARCHIVE_H

#include <QtCore/qfile.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/io/BaseArchive.h>

namespace toadlet{

class QtFileStream:public Object,public Stream{
public:
	TOADLET_IOBJECT(QtFileStream);

	QtFileStream(const QString &name):
		mFile(name)
	{
		mFile.open(QFile::ReadOnly);
	}
	
	void close(){mFile.close();}
	bool closed(){return !mFile.isOpen();}

	bool readable(){return mFile.isReadable();}
	int read(tbyte *buffer,int length){return mFile.read((char*)buffer,length);}

	bool writeable(){return mFile.isWritable();}
	int write(const tbyte *buffer,int length){return mFile.write((const char*)buffer,length);}

	bool reset(){return mFile.reset();}
	int length(){return mFile.size();}
	int position(){return mFile.pos();}
	bool seek(int offs){return mFile.seek(offs);}
	bool flush(){return mFile.flush();}

protected:
	QFile mFile;
};

class QtArchive:public BaseArchive{
public:
	TOADLET_OBJECT(QtArchive);
	
	void destroy(){}

	Stream::ptr openStream(const String &name){
		QtFileStream::ptr stream=new QtFileStream(name.c_str());
		if(stream->closed()){
			stream=NULL;
		}
		return stream;
	}

	const Collection<String> &getEntries(){return mEntries;}

protected:
	Collection<String> mEntries;
};

}

#endif
