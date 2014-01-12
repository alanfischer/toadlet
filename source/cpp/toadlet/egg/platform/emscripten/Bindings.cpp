#include <emscripten/bind.h>
#include <emscripten/wire.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>

using namespace emscripten;
using namespace toadlet;

namespace emscripten {
    namespace internal {

    template<>
    struct BindingType<egg::String> {
        typedef struct {
            size_t length;
            wchar_t data[1]; // trailing data
        }* WireType;
        static WireType toWireType(const egg::String& v) {
            WireType wt = (WireType)malloc(sizeof(size_t) + v.length() * sizeof(wchar_t));
            wt->length = v.length();
            wmemcpy(wt->data, v.wc_str(), v.length());
            return wt;
        }
        static egg::String fromWireType(WireType v) {
            return egg::String(v->data, v->length);
        }
        static void destroy(WireType v) {
            free(v);
        }
    };

    template<typename PointeeType>
    struct smart_ptr_trait<egg::IntrusivePointer<PointeeType>> {
        typedef egg::IntrusivePointer<PointeeType> PointerType;

        static PointeeType get(const PointerType& ptr) {
            return ptr.get();
        }

        static sharing_policy get_sharing_policy() {
            return sharing_policy::BY_EMVAL;
        }

        static egg::IntrusivePointer<PointeeType>* share(PointeeType* p, internal::EM_VAL v) {
            return new egg::IntrusivePointer<PointeeType>(
                p,
                val_deleter(val::take_ownership(v)));
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

	}
}

using namespace toadlet::egg;

EMSCRIPTEN_BINDINGS(egg) {
    using namespace emscripten::internal;

	_embind_register_std_wstring(TypeID<String>::get(), sizeof(stringchar), "String");

    class_<Log>("Log")
        .class_function("initialize", &Log::initialize)

		.class_function("error", select_overload<void(const String&)>(&Log::error))
        .class_function("warning", select_overload<void(const String&)>(&Log::warning))
        .class_function("alert", select_overload<void(const String&)>(&Log::alert))
        .class_function("debug", select_overload<void(const String&)>(&Log::debug))
        .class_function("excess", select_overload<void(const String&)>(&Log::excess))
        ;

	class_<Error>("Error")
		.class_function("unknown", select_overload<void(const String&)>(&Error::unknown))
        .class_function("tassert", select_overload<void(const String&)>(&Error::tassert))
        .class_function("invalidParameters", select_overload<void(const String&)>(&Error::invalidParameters))
        .class_function("nullPointer", select_overload<void(const String&)>(&Error::nullPointer))
        .class_function("unimplemented", select_overload<void(const String&)>(&Error::unimplemented))
        ;
}
