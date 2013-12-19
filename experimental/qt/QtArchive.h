#ifndef QTARCHIVE_H
#define QTARCHIVE_H

#include <QtCore/qfile.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/Stream.h>

class QtFileStream:public Object,public Stream{
public:
	TOADLET_OBJECT(QtFileStream);

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

class QtArchive:public BaseResource,public Archive{
public:
	TOADLET_RESOURCE(QtArchive,Archive);
	
	void destroy(){}

	Stream::ptr openStream(const String &name){
		QtFileStream::ptr stream=new QtFileStream(name.c_str());
		if(stream->closed()){
			stream=NULL;
		}
		return stream;
	}

	Resource::ptr openResource(const String &name){return NULL;}
	
	const Collection<String> &getEntries(){return mEntries;}

protected:
	Collection<String> mEntries;
};

#endif
