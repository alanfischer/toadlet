#ifndef XCOLHANDLER_H
#define XCOLHANDLER_H

#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/xml/Node.h>
#include <toadlet/egg/math/Math.h>

class Shape{
public:
	typedef toadlet::egg::SharedPointer<Shape> Ptr;

	enum Type{
		TYPE_UNKNOWN=0,
		TYPE_BOX,
		TYPE_SPHERE,
		TYPE_CYLINDER,
		TYPE_HULL,
	};

	virtual Type getType() const=0;
	virtual Shape *clone() const=0;

	toadlet::egg::math::Matrix4x4 transform;
};

class BoxShape:public Shape{
public:
	typedef toadlet::egg::SharedPointer<BoxShape,Shape> Ptr;

	BoxShape(const toadlet::egg::math::Vector3 halfExtents){
		this->halfExtents=halfExtents;
	}

	Type getType() const{
		return TYPE_BOX;
	}

	Shape *clone() const{
		BoxShape *shape=new BoxShape(halfExtents);
		shape->transform=transform;
		return shape;
	}

	toadlet::egg::math::Vector3 halfExtents;
};

class SphereShape:public Shape{
public:
	typedef toadlet::egg::SharedPointer<SphereShape,Shape> Ptr;

	SphereShape(toadlet::egg::math::real radius){
		this->radius=radius;
	}

	Type getType() const{
		return TYPE_SPHERE;
	}

	Shape *clone() const{
		SphereShape *shape=new SphereShape(radius);
		shape->transform=transform;
		return shape;
	}

	toadlet::egg::math::real radius;
};

class CylinderShape:public Shape{
public:
	typedef toadlet::egg::SharedPointer<CylinderShape,Shape> Ptr;

	CylinderShape(toadlet::egg::math::Vector3 halfExtents){
		this->halfExtents=halfExtents;
	}

	Type getType() const{
		return TYPE_CYLINDER;
	}

	Shape *clone() const{
		CylinderShape *shape=new CylinderShape(halfExtents);
		shape->transform=transform;
		return shape;
	}

	toadlet::egg::math::Vector3 halfExtents;
};

class HullShape:public Shape{
public:
	typedef toadlet::egg::SharedPointer<HullShape,Shape> Ptr;

	HullShape(const toadlet::egg::Collection<toadlet::egg::math::Vector3> &points){
		this->points=points;
	}

	Type getType() const{
		return TYPE_HULL;
	}

	Shape *clone() const{
		HullShape *shape=new HullShape(points);
		shape->transform=transform;
		return shape;
	}

	toadlet::egg::Collection<toadlet::egg::math::Vector3> points;
};

class ShapeSet{
public:
	typedef toadlet::egg::SharedPointer<ShapeSet> Ptr;

	ShapeSet();
	virtual ~ShapeSet();

	int getNumShapes() const;
	void addShape(Shape::Ptr shape);
	Shape::Ptr getShape(int i) const;

protected:
	toadlet::egg::Collection<Shape::Ptr> mShape;
};

class XCOLHandler{
public:
	int getVersion();

	ShapeSet::Ptr load(toadlet::egg::io::InputStream::Ptr in);
	ShapeSet::Ptr load(toadlet::egg::xml::Node::Ptr node);
	void save(ShapeSet::Ptr collection,toadlet::egg::io::OutputStream::Ptr out);
	void save(ShapeSet::Ptr collection,toadlet::egg::xml::Node::Ptr node);
};

#endif
