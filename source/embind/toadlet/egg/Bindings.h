#ifndef TOADLET_EGG_BINDINGS_H
#define TOADLET_EGG_BINDINGS_H

#include <emscripten/bind.h>
#include <emscripten/wire.h>
#include <toadlet/egg/Iterator.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/IntrusivePointer.h>

namespace emscripten {
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

        static sharing_policy get_sharing_policy() {
            return sharing_policy::BY_EMVAL;
        }

        static toadlet::egg::IntrusivePointer<PointeeType>* share(PointeeType* p, internal::EM_VAL v) {
            return new toadlet::egg::IntrusivePointer<PointeeType>(
                p);//, val_deleter(val::take_ownership(v)));
        }

    private:
        class val_deleter {
        public:
            val_deleter() = delete;
            explicit val_deleter(val v)
                : v(v)
            {}
            void operator()(void const*) {
                v();
                // eventually we'll need to support emptied out val
                v = val::undefined();
            }
        private:
            val v;
        };
    };

	template<typename T, typename... Args>
	toadlet::egg::IntrusivePointer<T> make_ptr(Args&&... args) {
		return toadlet::egg::IntrusivePointer<T>(new T(std::forward<Args>(args)...));
	}

	template<typename RangeType>
	struct RangeAccess {
		static bool atBegin(const RangeType& r){return r.it==r.begin();}
		static bool atEnd(const RangeType& r){return r.it==r.end();}

		static val next(RangeType& r){val v((typename RangeType::type)r.it);++r.it;return v;}
		static val prev(RangeType& r){val v((typename RangeType::type)r.it);--r.it;return v;}
    };

    template<typename T>
    class_<toadlet::egg::PointerIteratorRange<T>> register_range(const char* name) {
        typedef toadlet::egg::PointerIteratorRange<T> RangeType;

        return class_<toadlet::egg::PointerIteratorRange<T>>(name)
            .function("atBegin", &RangeAccess<RangeType>::atBegin)
            .function("atEnd", &RangeAccess<RangeType>::atEnd)
            .function("next", &RangeAccess<RangeType>::next)
            .function("prev", &RangeAccess<RangeType>::prev)
            ;
    }

}

#endif
