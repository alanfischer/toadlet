#include "XCOLHandler.h"
#include <toadlet/egg/xml/Parser.h>
#include <toadlet/egg/math/MathTextSTDStream.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::egg::io;
using namespace toadlet::egg::xml;

ShapeDataCollection::ShapeDataCollection(){
}

ShapeDataCollection::~ShapeDataCollection(){
	while(mShapeData.size()>0){
		delete mShapeData[0];
		mShapeData.erase(mShapeData.begin());
	}
}

int ShapeDataCollection::getNumShapeDatas() const{
	return mShapeData.size();
}

void ShapeDataCollection::addShapeData(const ShapeData &data){
	mShapeData.push_back(data.clone());
}

const ShapeData &ShapeDataCollection::getShapeData(int i) const{
	return *mShapeData[i];
}

int XCOLHandler::getVersion(){
	return 2;
}

const char LF[]={0xA,0x0};
const char CRLF[]={0xD,0xA,0x0};

ShapeDataCollection *XCOLHandler::load(InputStream *in,const String &name){
	Parser parser;
	Node *node=parser.loadDocument(in);

	return load(node);
}

ShapeDataCollection *XCOLHandler::load(Node *node){
	if(node==NULL){
		throw std::runtime_error("Null node");
	}

	ShapeDataCollection *collection=new ShapeDataCollection();

	if(node->getData()=="XCOL"){
		Property *version=node->getProperty("Version");
		if(version!=NULL){
			int v;
			StringStream vs(version->getData());
			vs >> v;
			if(v!=getVersion()){
				throw std::runtime_error("Invalid version");
			}
		}

		Node *shapeNode=NULL;
		for(shapeNode=node->getChild("Shape");shapeNode!=NULL;shapeNode=shapeNode->getNext()){
			Matrix4x4 transform;
			Node *transformNode=shapeNode->getChild("Transform");
			if(transformNode!=NULL){
				Node *transformData=transformNode->getChildren();
				if(transformData!=NULL){
					StringStream ss(transformData->getData());
					ss >> transform;
				}
			}

			Property *type=shapeNode->getProperty("Type");

			ShapeData *shapeData=NULL;
			if(type->getData()=="Box"){
				Node *halfExtentsNode=shapeNode->getChild("HalfExtents");
				if(halfExtentsNode!=NULL){
					Node *halfExtentsData=halfExtentsNode->getChildren();
					if(halfExtentsData!=NULL){
						Vector3 halfExtents;
						StringStream ss(halfExtentsData->getData());
						ss >> halfExtents;

						BoxShapeData box(halfExtents);
						box.transform=transform;
						collection->addShapeData(box);
					}
				}
			}
			else if(type->getData()=="Sphere"){
				Node *radiusNode=shapeNode->getChild("Radius");
				if(radiusNode!=NULL){
					Node *radiusData=radiusNode->getChildren();
					if(radiusData!=NULL){
						real radius;
						StringStream ss(radiusData->getData());
						ss >> radius;

						SphereShapeData sphere(radius);
						sphere.transform=transform;
						collection->addShapeData(sphere);
					}
				}
			}
			else if(type->getData()=="Cylinder"){
				Node *halfExtentsNode=shapeNode->getChild("HalfExtents");
				if(halfExtentsNode!=NULL){
					Node *halfExtentsData=halfExtentsNode->getChildren();
					if(halfExtentsData!=NULL){
						Vector3 halfExtents;
						StringStream ss(halfExtentsData->getData());
						ss >> halfExtents;

						CylinderShapeData cylinder(halfExtents);
						cylinder.transform=transform;
						collection->addShapeData(cylinder);
					}
				}
			}
			else if(type->getData()=="Hull"){
				Collection<Vector3> points;

				Node *pointsNode=shapeNode->getChild("Points");
				if(pointsNode!=NULL){
					int count=0;

					Property *prop=pointsNode->getProperty("Count");
					if(prop!=NULL){
						count=-1;
						sscanf(prop->getData().c_str(),"%d",&count);
					}

					points.resize(count);

					String data=pointsNode->getChildren()->getData();

					int l=0;
					int i=0;
					while(true){
						int lf=data.find(LF,i+1);
						int crlf=data.find(CRLF,i+1);
						int end=data.length();
						int start=end+1;
						if(crlf!=String::npos){
							end=crlf;
							start=crlf+2;
						}
						else if(lf!=String::npos){
							end=lf;
							start=lf+1;
						}

						String line=Parser::removeWhiteSpace(data.substr(i,end-i));

						int c=0;
						int j=0;
						while(true){
							int space=line.find(' ',j+1);
							if(space==String::npos){
								space=line.length();
							}

							String element=line.substr(j,space);
							Vector3 point;
							sscanf(element.c_str(),"%f,%f,%f",&point.x,&point.y,&point.z);

							points.push_back(point);

							j=space;
							c++;
							if(space>=line.length()){
								break;
							}
						}

						i=start;
						l++;
						if(i>=data.length()){
							break;
						}
					}
				}

				HullShapeData hull(points);
				hull.transform=transform;
				collection->addShapeData(hull);
			}
		}
	}

	return collection;
}

void XCOLHandler::save(ShapeDataCollection *collection,OutputStream *out,const String &name){
	Node *node=new Node();

	save(collection,node);

	Parser parser;
	parser.saveDocument(node,out);

	delete node;
}

void XCOLHandler::save(ShapeDataCollection *collection,Node *node){
	node->setType(Node::TYPE_TAG);
	node->setData("XCOL");
	StringStream vs;
	vs << getVersion();
	node->addProperty(new Property("Version",vs.str()));

	int i;
	for(i=0;i<collection->getNumShapeDatas();++i){
		const ShapeData &shapeData=collection->getShapeData(i);

		Node *shapeNode=new Node();
		shapeNode->setType(Node::TYPE_TAG);
		shapeNode->setData("Shape");

		if(shapeData.getType()==ShapeData::TYPE_BOX){
			shapeNode->addProperty(new Property("Type","Box"));
			const BoxShapeData &box=(const BoxShapeData&)shapeData;

			Node *halfExtentsNode=new Node();
			halfExtentsNode->setType(Node::TYPE_TAG);
			halfExtentsNode->setData("HalfExtents");

			Node *halfExtentsData=new Node();
			halfExtentsData->setType(Node::TYPE_TEXT);
			StringStream ss;
			ss << box.halfExtents;
			halfExtentsData->setData(ss.str());
			halfExtentsNode->addChild(halfExtentsData);

			shapeNode->addChild(halfExtentsNode);
		}
		else if(shapeData.getType()==ShapeData::TYPE_SPHERE){
			shapeNode->addProperty(new Property("Type","Sphere"));
			const SphereShapeData &sphere=(const SphereShapeData&)shapeData;

			Node *radiusNode=new Node();
			radiusNode->setType(Node::TYPE_TAG);
			radiusNode->setData("Radius");

			Node *radiusData=new Node();
			radiusData->setType(Node::TYPE_TEXT);
			StringStream ss;
			ss << sphere.radius;
			radiusData->setData(ss.str());
			radiusNode->addChild(radiusData);

			shapeNode->addChild(radiusNode);
		}
		else if(shapeData.getType()==ShapeData::TYPE_CYLINDER){
			shapeNode->addProperty(new Property("Type","Cylinder"));
			const CylinderShapeData &cylinder=(const CylinderShapeData&)shapeData;

			Node *halfExtentsNode=new Node();
			halfExtentsNode->setType(Node::TYPE_TAG);
			halfExtentsNode->setData("HalfExtents");

			Node *halfExtentsData=new Node();
			halfExtentsData->setType(Node::TYPE_TEXT);
			StringStream ss;
			ss << cylinder.halfExtents;
			halfExtentsData->setData(ss.str());
			halfExtentsNode->addChild(halfExtentsData);

			shapeNode->addChild(halfExtentsNode);
		}
		else if(shapeData.getType()==ShapeData::TYPE_HULL){
			shapeNode->addProperty(new Property("Type","Hull"));
			const HullShapeData &hull=(const HullShapeData&)shapeData;

			Node *pointsNode=new Node();
			pointsNode->setType(Node::TYPE_TAG);
			pointsNode->setData("Points");
			shapeNode->addChild(pointsNode);
			{
				String data;

				StringStream ss;
				ss << hull.points.size();
				pointsNode->addProperty(new Property("Count",ss.str()));

				int j;
				for(j=0;j<hull.points.size();++j){
					if(j>0){
						data+="\t\t\t\t";
					}
					StringStream ss;
					ss << hull.points[j].x << "," << hull.points[j].y << "," << hull.points[j].z;
					data+=ss.str();
					if(j<hull.points.size()-1){
						data+='\n';
					}
				}

				Node *dataNode=new Node();
				dataNode->setType(Node::TYPE_TEXT);
				dataNode->setData(data);
				pointsNode->addChild(dataNode);
			}
		}

		Node *transformNode=new Node();
		transformNode->setType(Node::TYPE_TAG);
		transformNode->setData("Transform");

		Node *transformData=new Node();
		transformData->setType(Node::TYPE_TEXT);
		StringStream ss;
		ss << shapeData.transform;
		transformData->setData(ss.str());
		transformNode->addChild(transformData);

		shapeNode->addChild(transformNode);

		node->addChild(shapeNode);
	}
}
