#ifndef TOADLET_EGG_BINDINGS_H
#define TOADLET_EGG_BINDINGS_H

#include <emscripten/bind.h>
#include <emscripten/wire.h>
#include <toadlet/egg/Iterator.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Object.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/io/BaseArchive.h>

namespace emscripten {
	using namespace toadlet;
	using namespace toadlet::egg;
	using namespace toadlet::egg::io;

	namespace internal {

	template<>
	struct BindingType<toadlet::egg::String> {
		typedef struct {
			size_t length;
			wchar_t data[1]; // trailing data
		}* WireType;
		static WireType toWireType(const toadlet::egg::String& v) {
			WireType wt = (WireType)malloc(sizeof(size_t) + v.length() * sizeof(wchar_t));
			wt->length = v.length();
			wmemcpy(wt->data, v.wc_str(), v.length());
			return wt;
		}
		static toadlet::egg::String fromWireType(WireType v) {
			return toadlet::egg::String(v->data, v->length);
		}
		static void destroy(WireType v) {
			free(v);
		}
	};

	}

	template<typename PointeeType>
	struct smart_ptr_trait<toadlet::egg::IntrusivePointer<PointeeType>> {
		typedef toadlet::egg::IntrusivePointer<PointeeType> PointerType;
		typedef typename PointerType::element_type element_type;

		static element_type* get(const PointerType& ptr) {
			return ptr.get();
		}

		static toadlet::egg::IntrusivePointer<PointeeType>* share(PointeeType* p, internal::EM_VAL v) {
			return new toadlet::egg::IntrusivePointer<PointeeType>(p);
		}

		static sharing_policy get_sharing_policy() {
			return sharing_policy::INTRUSIVE;
		}

		static PointerType* construct_null() {
            return new PointerType;
        }
	};

	template<typename T, typename... Args>
	toadlet::egg::IntrusivePointer<T> make_ptr(Args&&... args) {
		return toadlet::egg::IntrusivePointer<T>(new T(std::forward<Args>(args)...));
	}

	// range bindings
	template<typename RangeType>
	struct RangeIterator {
		RangeType& range;
		typename RangeType::iterator it;
		
		RangeIterator(RangeType& range):range(range),it(range.begin()){}
		typename RangeType::value_type next(){return *it++;}
		bool hasNext() const{return it!=range.end();}
		typename RangeType::value_type prev(){return *it--;}
		bool hasPrev() const{return it!=range.begin();}
	};

	template<typename RangeType>
	struct RangeAccess {
		static RangeIterator<RangeType> iterator(RangeType& range){return RangeIterator<RangeType>(range);}
	};

	template<typename RangeType>
	class_<RangeType> register_range(const char* name,const char* iteratorName) {
		class_<RangeIterator<RangeType>>(iteratorName)
			.function("next", &RangeIterator<RangeType>::next, allow_raw_pointers())
			.function("hasNext", &RangeIterator<RangeType>::hasNext)
			;
		
		return class_<RangeType>(name)
			.function("iterator", &RangeAccess<RangeType>::iterator)
			;
	}

	// list bindings
	template<typename ListType>
	struct ListIterator {
		ListType& list;
		typename ListType::iterator it;
		
		ListIterator(ListType& list):list(list),it(list.begin()){}
		typename ListType::value_type next(){return *it++;}
		bool hasNext() const{return it!=list.end();}
	};

	template<typename ListType>
	struct ListAccess {
		static ListIterator<ListType> iterator(ListType& list){return ListIterator<ListType>(list);}
		static void push_back(ListType& list,typename ListType::const_reference type){list.push_back(type);}
		static void remove(ListType& list,typename ListType::const_reference type){list.remove(type);}
	};

	template<typename ListType>
	class_<ListType> register_list(const char* name,const char* iteratorName) {
		class_<ListIterator<ListType>>(iteratorName)
			.function("next", &ListIterator<ListType>::next, allow_raw_pointers())
			.function("hasNext", &ListIterator<ListType>::hasNext)
			;
		
		return class_<ListType>(name)
            .template constructor<>()
			.function("iterator", &ListAccess<ListType>::iterator)
			.function("push_back", &ListAccess<ListType>::push_back)
			.function("remove", &ListAccess<ListType>::remove)
			;
	}

	class StreamRequestWrapper: public Object,public wrapper<StreamRequest> {
	public:
		TOADLET_IOBJECT(StreamRequestWrapper);
		EMSCRIPTEN_WRAPPER(StreamRequestWrapper);

		void streamReady(Stream *stream){
			return call<void>("streamReady",Stream::ptr(stream));
		}

		void streamException(const Exception &ex){
			return call<void>("streamException",ex);
		}

		void streamProgress(float progress){
			return call<void>("streamProgress",progress);
		}
	};

	class ResourceRequestWrapper: public Object,public wrapper<ResourceRequest> {
	public:
		TOADLET_IOBJECT(ResourceRequestWrapper);
		EMSCRIPTEN_WRAPPER(ResourceRequestWrapper);

		void resourceReady(Resource *resource){
			return call<void>("resourceReady",Resource::ptr(resource));
		}

		void resourceException(const Exception &ex){
			return call<void>("resourceException",ex);
		}

		void resourceProgress(float progress){
			return call<void>("resourceProgress",progress);
		}
	};

	class ArchiveWrapper: public wrapper<BaseArchive> {
	public:
		TOADLET_IOBJECT(ArchiveWrapper);
		EMSCRIPTEN_WRAPPER(ArchiveWrapper);

		bool openStream(const String &name,StreamRequest *request){
			return call<bool>("openStream",name,StreamRequest::ptr(request));
		}

		bool openResource(const String &name,ResourceRequest *request){
			return call<bool>("openResource",name,ResourceRequest::ptr(request));
		}

		const Collection<String> &getEntries(){return mEntries;}

	protected:
		Collection<String> mEntries;
	};
}

#endif
