/*********************************************************************
 * NAN - Native Abstractions for Node.js
 *
 * Copyright (c) 2016 NAN contributors:
 *   - Rod Vagg <https://github.com/rvagg>
 *   - Benjamin Byholm <https://github.com/kkoopa>
 *   - Trevor Norris <https://github.com/trevnorris>
 *   - Nathan Rajlich <https://github.com/TooTallNate>
 *   - Brett Lawson <https://github.com/brett19>
 *   - Ben Noordhuis <https://github.com/bnoordhuis>
 *   - David Siegel <https://github.com/agnat>
 *
 * MIT License <https://github.com/nodejs/nan/blob/master/LICENSE.md>
 *
 * Version 2.3.3: current Node 6.0.0, Node 12: 0.12.13, Node 10: 0.10.44, iojs: 3.3.1
 *
 * See https://github.com/nodejs/nan for the latest update to this file
 **********************************************************************************/

#ifndef NAN_H_
#define NAN_H_

#include <node_version.h>

#define NODE_0_10_MODULE_VERSION 11
#define NODE_0_12_MODULE_VERSION 14
#define ATOM_0_21_MODULE_VERSION 41
#define IOJS_1_0_MODULE_VERSION  42
#define IOJS_1_1_MODULE_VERSION  43
#define IOJS_2_0_MODULE_VERSION  44
#define IOJS_3_0_MODULE_VERSION  45
#define NODE_4_0_MODULE_VERSION  46
#define NODE_5_0_MODULE_VERSION  47
#define NODE_6_0_MODULE_VERSION  48

#ifdef _MSC_VER
# define NAN_HAS_CPLUSPLUS_11 (_MSC_VER >= 1800)
#else
# define NAN_HAS_CPLUSPLUS_11 (__cplusplus >= 201103L)
#endif

#if NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION && !NAN_HAS_CPLUSPLUS_11
# error This version of node/NAN/v8 requires a C++11 compiler
#endif

#include <uv.h>
#include <node.h>
#include <node_buffer.h>
#include <node_object_wrap.h>
#include <algorithm>
#include <cstring>
#include <climits>
#include <cstdlib>
#if defined(_MSC_VER)
# pragma warning( push )
# pragma warning( disable : 4530 )
# include <string>
# include <vector>
# pragma warning( pop )
#else
# include <string>
# include <vector>
#endif

// uv helpers
#ifdef UV_VERSION_MAJOR
# ifndef UV_VERSION_PATCH
#  define UV_VERSION_PATCH 0
# endif
# define NAUV_UVVERSION ((UV_VERSION_MAJOR << 16) | \
                         (UV_VERSION_MINOR <<  8) | \
                         (UV_VERSION_PATCH))
#else
# define NAUV_UVVERSION 0x000b00
#endif

#if NAUV_UVVERSION < 0x000b0b
# ifdef WIN32
#  include <windows.h>
# else
#  include <pthread.h>
# endif
#endif

namespace Nan {

#if defined(__GNUC__) && !(defined(DEBUG) && DEBUG)
# define NAN_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !(defined(DEBUG) && DEBUG)
# define NAN_INLINE __forceinline
#else
# define NAN_INLINE inline
#endif

#if defined(__GNUC__) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
# define NAN_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && \
    !(defined(V8_DISABLE_DEPRECATIONS) && V8_DISABLE_DEPRECATIONS)
# define NAN_DEPRECATED __declspec(deprecated)
#else
# define NAN_DEPRECATED
#endif

#if NAN_HAS_CPLUSPLUS_11
# define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&) = delete;
# define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&) = delete;
# define NAN_DISALLOW_MOVE(CLASS)                                              \
    CLASS(CLASS&&) = delete;  /* NOLINT(build/c++11) */                        \
    void operator=(CLASS&&) = delete;
#else
# define NAN_DISALLOW_ASSIGN(CLASS) void operator=(const CLASS&);
# define NAN_DISALLOW_COPY(CLASS) CLASS(const CLASS&);
# define NAN_DISALLOW_MOVE(CLASS)
#endif

#define NAN_DISALLOW_ASSIGN_COPY(CLASS)                                        \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_COPY(CLASS)

#define NAN_DISALLOW_ASSIGN_MOVE(CLASS)                                        \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_MOVE(CLASS)

#define NAN_DISALLOW_COPY_MOVE(CLASS)                                          \
    NAN_DISALLOW_COPY(CLASS)                                                   \
    NAN_DISALLOW_MOVE(CLASS)

#define NAN_DISALLOW_ASSIGN_COPY_MOVE(CLASS)                                   \
    NAN_DISALLOW_ASSIGN(CLASS)                                                 \
    NAN_DISALLOW_COPY(CLASS)                                                   \
    NAN_DISALLOW_MOVE(CLASS)

#define TYPE_CHECK(T, S)                                                       \
    while (false) {                                                            \
      *(static_cast<T *volatile *>(0)) = static_cast<S*>(0);                   \
    }

//=== RegistrationFunction =====================================================

typedef v8::Local<v8::Object> ADDON_REGISTER_FUNCTION_ARGS_TYPE;

#define NAN_MODULE_INIT(name)                                                  \
    void name(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target)

//=== CallbackInfo =============================================================

#include "nan_callbacks.h"  // NOLINT(build/include)

//==============================================================================

typedef v8::UnboundScript      UnboundScript;
typedef v8::Script             BoundScript;


typedef v8::String::ExternalOneByteStringResource
    ExternalOneByteStringResource;



template<typename T>
class NonCopyablePersistentTraits :
    public v8::NonCopyablePersistentTraits<T> {};
template<typename T>
class CopyablePersistentTraits :
    public v8::CopyablePersistentTraits<T> {};

template<typename T>
class PersistentBase :
    public v8::PersistentBase<T> {};

template<typename T, typename M = v8::NonCopyablePersistentTraits<T> >
class Persistent;


#include "nan_maybe_43_inl.h"  // NOLINT(build/include)

#include "nan_converters.h"  // NOLINT(build/include)
#include "nan_new.h"  // NOLINT(build/include)

#if NAUV_UVVERSION < 0x000b17
#define NAUV_WORK_CB(func) \
    void func(uv_async_t *async, int)
#else
#define NAUV_WORK_CB(func) \
    void func(uv_async_t *async)
#endif

#if NAUV_UVVERSION >= 0x000b0b

typedef uv_key_t nauv_key_t;

inline int nauv_key_create(nauv_key_t *key) {
  return uv_key_create(key);
}

inline void nauv_key_delete(nauv_key_t *key) {
  uv_key_delete(key);
}

inline void* nauv_key_get(nauv_key_t *key) {
  return uv_key_get(key);
}

inline void nauv_key_set(nauv_key_t *key, void *value) {
  uv_key_set(key, value);
}

#else

/* Implement thread local storage for older versions of libuv.
 * This is essentially a backport of libuv commit 5d2434bf
 * written by Ben Noordhuis, adjusted for names and inline.
 */

#ifndef WIN32

typedef pthread_key_t nauv_key_t;

inline int nauv_key_create(nauv_key_t* key) {
  return -pthread_key_create(key, NULL);
}

inline void nauv_key_delete(nauv_key_t* key) {
  if (pthread_key_delete(*key))
    abort();
}

inline void* nauv_key_get(nauv_key_t* key) {
  return pthread_getspecific(*key);
}

inline void nauv_key_set(nauv_key_t* key, void* value) {
  if (pthread_setspecific(*key, value))
    abort();
}

#else

typedef struct {
  DWORD tls_index;
} nauv_key_t;

inline int nauv_key_create(nauv_key_t* key) {
  key->tls_index = TlsAlloc();
  if (key->tls_index == TLS_OUT_OF_INDEXES)
    return UV_ENOMEM;
  return 0;
}

inline void nauv_key_delete(nauv_key_t* key) {
  if (TlsFree(key->tls_index) == FALSE)
    abort();
  key->tls_index = TLS_OUT_OF_INDEXES;
}

inline void* nauv_key_get(nauv_key_t* key) {
  void* value = TlsGetValue(key->tls_index);
  if (value == NULL)
    if (GetLastError() != ERROR_SUCCESS)
      abort();
  return value;
}

inline void nauv_key_set(nauv_key_t* key, void* value) {
  if (TlsSetValue(key->tls_index, value) == FALSE)
    abort();
}

#endif
#endif



typedef v8::WeakCallbackType WeakCallbackType;


template<typename P> class WeakCallbackInfo;


#include "nan_persistent_12_inl.h"  // NOLINT(build/include)

namespace imp {
  static const size_t kMaxLength = 0x3fffffff;
  // v8::String::REPLACE_INVALID_UTF8 was introduced
  // in node.js v0.10.29 and v0.8.27.

  static const unsigned kReplaceInvalidUtf8 = v8::String::REPLACE_INVALID_UTF8;
}  // end of namespace imp

//=== HandleScope ==============================================================

class HandleScope {
  v8::HandleScope scope;

 public:
  inline HandleScope() : scope(v8::Isolate::GetCurrent()) {}
  inline static int NumberOfHandles() {
    return v8::HandleScope::NumberOfHandles(v8::Isolate::GetCurrent());
  }

 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  HandleScope(const HandleScope &);
  void operator=(const HandleScope &);
  void *operator new(size_t size);
  void operator delete(void *, size_t);
};

class EscapableHandleScope {
 public:
  inline EscapableHandleScope() : scope(v8::Isolate::GetCurrent()) {}

  inline static int NumberOfHandles() {
    return v8::EscapableHandleScope::NumberOfHandles(v8::Isolate::GetCurrent());
  }

  template<typename T>
  inline v8::Local<T> Escape(v8::Local<T> value) {
    return scope.Escape(value);
  }

 private:
  v8::EscapableHandleScope scope;

 private:
  // Make it hard to create heap-allocated or illegal handle scopes by
  // disallowing certain operations.
  EscapableHandleScope(const EscapableHandleScope &);
  void operator=(const EscapableHandleScope &);
  void *operator new(size_t size);
  void operator delete(void *, size_t);
};

//=== TryCatch =================================================================

class TryCatch {
  v8::TryCatch try_catch_;
  friend void FatalException(const TryCatch&);

 public:
  TryCatch() : try_catch_(v8::Isolate::GetCurrent()) {}

  NAN_INLINE bool HasCaught() const { return try_catch_.HasCaught(); }

  NAN_INLINE bool CanContinue() const { return try_catch_.CanContinue(); }

  NAN_INLINE v8::Local<v8::Value> ReThrow() {
    return try_catch_.ReThrow();
  }

  NAN_INLINE v8::Local<v8::Value> Exception() const {
    return try_catch_.Exception();
  }

  NAN_INLINE v8::MaybeLocal<v8::Value> StackTrace() const {
    return try_catch_.StackTrace(GetCurrentContext());
  }

  NAN_INLINE v8::Local<v8::Message> Message() const {
    return try_catch_.Message();
  }

  NAN_INLINE void Reset() { try_catch_.Reset(); }

  NAN_INLINE void SetVerbose(bool value) { try_catch_.SetVerbose(value); }

  NAN_INLINE void SetCaptureMessage(bool value) {
    try_catch_.SetCaptureMessage(value);
  }
};

//============ =================================================================

/* node 0.12  */

  NAN_INLINE
  void SetCounterFunction(v8::CounterLookupCallback cb) {
    v8::Isolate::GetCurrent()->SetCounterFunction(cb);
  }

  NAN_INLINE
  void SetCreateHistogramFunction(v8::CreateHistogramCallback cb) {
    v8::Isolate::GetCurrent()->SetCreateHistogramFunction(cb);
  }

  NAN_INLINE
  void SetAddHistogramSampleFunction(v8::AddHistogramSampleCallback cb) {
    v8::Isolate::GetCurrent()->SetAddHistogramSampleFunction(cb);
  }


  NAN_INLINE bool IdleNotification(int idle_time_in_ms) {
    return v8::Isolate::GetCurrent()->IdleNotificationDeadline(
        idle_time_in_ms * 0.001);
  }

  NAN_INLINE void LowMemoryNotification() {
    v8::Isolate::GetCurrent()->LowMemoryNotification();
  }

  NAN_INLINE void ContextDisposedNotification() {
    v8::Isolate::GetCurrent()->ContextDisposedNotification();
  }



  NAN_INLINE v8::Local<v8::Primitive> Undefined() {
    return v8::Undefined(v8::Isolate::GetCurrent());
  }

  NAN_INLINE v8::Local<v8::Primitive> Null() {
    return v8::Null(v8::Isolate::GetCurrent());
  }

  NAN_INLINE v8::Local<v8::Boolean> True() {
    return v8::True(v8::Isolate::GetCurrent());
  }

  NAN_INLINE v8::Local<v8::Boolean> False() {
    return v8::False(v8::Isolate::GetCurrent());
  }

  NAN_INLINE v8::Local<v8::String> EmptyString() {
    return v8::String::Empty(v8::Isolate::GetCurrent());
  }

  NAN_INLINE int AdjustExternalMemory(int bc) {
    return static_cast<int>(
        v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(bc));
  }

  NAN_INLINE void SetTemplate(
      v8::Local<v8::Template> templ
    , const char *name
    , v8::Local<v8::Data> value) {
    templ->Set(v8::Isolate::GetCurrent(), name, value);
  }

  NAN_INLINE void SetTemplate(
      v8::Local<v8::Template> templ
    , v8::Local<v8::String> name
    , v8::Local<v8::Data> value
    , v8::PropertyAttribute attributes) {
    templ->Set(name, value, attributes);
  }

  NAN_INLINE v8::Local<v8::Context> GetCurrentContext() {
    return v8::Isolate::GetCurrent()->GetCurrentContext();
  }

  NAN_INLINE void* GetInternalFieldPointer(
      v8::Local<v8::Object> object
    , int index) {
    return object->GetAlignedPointerFromInternalField(index);
  }

  NAN_INLINE void SetInternalFieldPointer(
      v8::Local<v8::Object> object
    , int index
    , void* value) {
    object->SetAlignedPointerInInternalField(index, value);
  }

# define NAN_GC_CALLBACK(name)                                                 \
    void name(v8::Isolate *isolate, v8::GCType type, v8::GCCallbackFlags flags)

  typedef v8::Isolate::GCCallback GCEpilogueCallback;
  typedef v8::Isolate::GCCallback GCPrologueCallback;

  NAN_INLINE void AddGCEpilogueCallback(
      GCEpilogueCallback callback
    , v8::GCType gc_type_filter = v8::kGCTypeAll) {
    v8::Isolate::GetCurrent()->AddGCEpilogueCallback(callback, gc_type_filter);
  }

  NAN_INLINE void RemoveGCEpilogueCallback(
      GCEpilogueCallback callback) {
    v8::Isolate::GetCurrent()->RemoveGCEpilogueCallback(callback);
  }

  NAN_INLINE void AddGCPrologueCallback(
      GCPrologueCallback callback
    , v8::GCType gc_type_filter = v8::kGCTypeAll) {
    v8::Isolate::GetCurrent()->AddGCPrologueCallback(callback, gc_type_filter);
  }

  NAN_INLINE void RemoveGCPrologueCallback(
      GCPrologueCallback callback) {
    v8::Isolate::GetCurrent()->RemoveGCPrologueCallback(callback);
  }

  NAN_INLINE void GetHeapStatistics(
      v8::HeapStatistics *heap_statistics) {
    v8::Isolate::GetCurrent()->GetHeapStatistics(heap_statistics);
  }

# define X(NAME)                                                               \
    NAN_INLINE v8::Local<v8::Value> NAME(const char *msg) {                    \
      EscapableHandleScope scope;                                              \
      return scope.Escape(v8::Exception::NAME(New(msg).ToLocalChecked()));     \
    }                                                                          \
                                                                               \
    NAN_INLINE                                                                 \
    v8::Local<v8::Value> NAME(v8::Local<v8::String> msg) {                     \
      return v8::Exception::NAME(msg);                                         \
    }                                                                          \
                                                                               \
    NAN_INLINE void Throw ## NAME(const char *msg) {                           \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(New(msg).ToLocalChecked()));                     \
    }                                                                          \
                                                                               \
    NAN_INLINE void Throw ## NAME(v8::Local<v8::String> msg) {                 \
      HandleScope scope;                                                       \
      v8::Isolate::GetCurrent()->ThrowException(                               \
          v8::Exception::NAME(msg));                                           \
    }

  X(Error)
  X(RangeError)
  X(ReferenceError)
  X(SyntaxError)
  X(TypeError)

# undef X

  NAN_INLINE void ThrowError(v8::Local<v8::Value> error) {
    v8::Isolate::GetCurrent()->ThrowException(error);
  }

  NAN_INLINE MaybeLocal<v8::Object> NewBuffer(
      char *data
    , size_t length
    , node::Buffer::FreeCallback callback
    , void *hint
  ) {
    // arbitrary buffer lengths requires
    // NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION
    assert(length <= imp::kMaxLength && "too large buffer");

    return node::Buffer::New(
        v8::Isolate::GetCurrent(), data, length, callback, hint);

  }

  NAN_INLINE MaybeLocal<v8::Object> CopyBuffer(
      const char *data
    , uint32_t size
  ) {
    // arbitrary buffer lengths requires
    // NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION
    assert(size <= imp::kMaxLength && "too large buffer");

    return node::Buffer::Copy(
        v8::Isolate::GetCurrent(), data, size);
  }

  NAN_INLINE MaybeLocal<v8::Object> NewBuffer(uint32_t size) {
    // arbitrary buffer lengths requires
    // NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION
    assert(size <= imp::kMaxLength && "too large buffer");

    return node::Buffer::New(
        v8::Isolate::GetCurrent(), size);
  }

  NAN_INLINE MaybeLocal<v8::Object> NewBuffer(
      char* data
    , uint32_t size
  ) {
    // arbitrary buffer lengths requires
    // NODE_MODULE_VERSION >= IOJS_3_0_MODULE_VERSION
    assert(size <= imp::kMaxLength && "too large buffer");

    return node::Buffer::New(v8::Isolate::GetCurrent(), data, size);
  }


  NAN_INLINE MaybeLocal<v8::String>
  NewOneByteString(const uint8_t * value, int length = -1) {
    return v8::String::NewFromOneByte(v8::Isolate::GetCurrent(), value,
          v8::NewStringType::kNormal, length);
  }

  NAN_INLINE MaybeLocal<BoundScript> CompileScript(
      v8::Local<v8::String> s
    , const v8::ScriptOrigin& origin
  ) {
    v8::ScriptCompiler::Source source(s, origin);
    return v8::ScriptCompiler::Compile(GetCurrentContext(), &source);
  }

  NAN_INLINE MaybeLocal<BoundScript> CompileScript(
      v8::Local<v8::String> s
  ) {
    v8::ScriptCompiler::Source source(s);
    return v8::ScriptCompiler::Compile(GetCurrentContext(), &source);
  }

  NAN_INLINE MaybeLocal<v8::Value> RunScript(
      v8::Local<UnboundScript> script
  ) {
    return script->BindToCurrentContext()->Run(GetCurrentContext());
  }

  NAN_INLINE MaybeLocal<v8::Value> RunScript(
      v8::Local<BoundScript> script
  ) {
    return script->Run(GetCurrentContext());
  }


  NAN_INLINE v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::Function> func
    , int argc
    , v8::Local<v8::Value>* argv) {
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, func, argc, argv);
  }

  NAN_INLINE v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , v8::Local<v8::String> symbol
    , int argc
    , v8::Local<v8::Value>* argv) {
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, symbol, argc, argv);
  }

  NAN_INLINE v8::Local<v8::Value> MakeCallback(
      v8::Local<v8::Object> target
    , const char* method
    , int argc
    , v8::Local<v8::Value>* argv) {
    return node::MakeCallback(
        v8::Isolate::GetCurrent(), target, method, argc, argv);
  }

  NAN_INLINE void FatalException(const TryCatch& try_catch) {
    node::FatalException(v8::Isolate::GetCurrent(), try_catch.try_catch_);
  }

  NAN_INLINE v8::Local<v8::Value> ErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return node::ErrnoException(v8::Isolate::GetCurrent(), errorno, syscall,
            message, path);
  }

  NAN_DEPRECATED NAN_INLINE v8::Local<v8::Value> NanErrnoException(
          int errorno
       ,  const char* syscall = NULL
       ,  const char* message = NULL
       ,  const char* path = NULL) {
    return ErrnoException(errorno, syscall, message, path);
  }

  template<typename T>
  NAN_INLINE void SetIsolateData(
      v8::Isolate *isolate
    , T *data
  ) {
      isolate->SetData(0, data);
  }

  template<typename T>
  NAN_INLINE T *GetIsolateData(
      v8::Isolate *isolate
  ) {
      return static_cast<T*>(isolate->GetData(0));
  }

class Utf8String {
 public:
  NAN_INLINE explicit Utf8String(v8::Local<v8::Value> from) :
      length_(0), str_(str_st_) {
    if (!from.IsEmpty()) {
      v8::Local<v8::String> string = from->ToString();
      if (!string.IsEmpty()) {
        size_t len = 3 * string->Length() + 1;
        assert(len <= INT_MAX);
        if (len > sizeof (str_st_)) {
          str_ = static_cast<char*>(malloc(len));
          assert(str_ != 0);
        }
        const int flags =
            v8::String::NO_NULL_TERMINATION | imp::kReplaceInvalidUtf8;
        length_ = string->WriteUtf8(str_, static_cast<int>(len), 0, flags);
        str_[length_] = '\0';
      }
    }
  }

  NAN_INLINE int length() const {
    return length_;
  }

  NAN_INLINE char* operator*() { return str_; }
  NAN_INLINE const char* operator*() const { return str_; }

  NAN_INLINE ~Utf8String() {
    if (str_ != str_st_) {
      free(str_);
    }
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(Utf8String)

  int length_;
  char *str_;
  char str_st_[1024];
};


typedef void (*FreeCallback)(char *data, void *hint);

typedef const FunctionCallbackInfo<v8::Value>& NAN_METHOD_ARGS_TYPE;
typedef void NAN_METHOD_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_GETTER_ARGS_TYPE;
typedef void NAN_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<void>& NAN_SETTER_ARGS_TYPE;
typedef void NAN_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>&
    NAN_PROPERTY_GETTER_ARGS_TYPE;
typedef void NAN_PROPERTY_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>&
    NAN_PROPERTY_SETTER_ARGS_TYPE;
typedef void NAN_PROPERTY_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Array>&
    NAN_PROPERTY_ENUMERATOR_ARGS_TYPE;
typedef void NAN_PROPERTY_ENUMERATOR_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Boolean>&
    NAN_PROPERTY_DELETER_ARGS_TYPE;
typedef void NAN_PROPERTY_DELETER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Integer>&
    NAN_PROPERTY_QUERY_ARGS_TYPE;
typedef void NAN_PROPERTY_QUERY_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_INDEX_GETTER_ARGS_TYPE;
typedef void NAN_INDEX_GETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Value>& NAN_INDEX_SETTER_ARGS_TYPE;
typedef void NAN_INDEX_SETTER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Array>&
    NAN_INDEX_ENUMERATOR_ARGS_TYPE;
typedef void NAN_INDEX_ENUMERATOR_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Boolean>&
    NAN_INDEX_DELETER_ARGS_TYPE;
typedef void NAN_INDEX_DELETER_RETURN_TYPE;

typedef const PropertyCallbackInfo<v8::Integer>&
    NAN_INDEX_QUERY_ARGS_TYPE;
typedef void NAN_INDEX_QUERY_RETURN_TYPE;

#define NAN_METHOD(name)                                                       \
    Nan::NAN_METHOD_RETURN_TYPE name(Nan::NAN_METHOD_ARGS_TYPE info)
#define NAN_GETTER(name)                                                       \
    Nan::NAN_GETTER_RETURN_TYPE name(                                          \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_GETTER_ARGS_TYPE info)
#define NAN_SETTER(name)                                                       \
    Nan::NAN_SETTER_RETURN_TYPE name(                                          \
        v8::Local<v8::String> property                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_SETTER_ARGS_TYPE info)
#define NAN_PROPERTY_GETTER(name)                                              \
    Nan::NAN_PROPERTY_GETTER_RETURN_TYPE name(                                 \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_GETTER_ARGS_TYPE info)
#define NAN_PROPERTY_SETTER(name)                                              \
    Nan::NAN_PROPERTY_SETTER_RETURN_TYPE name(                                 \
        v8::Local<v8::String> property                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_PROPERTY_SETTER_ARGS_TYPE info)
#define NAN_PROPERTY_ENUMERATOR(name)                                          \
    Nan::NAN_PROPERTY_ENUMERATOR_RETURN_TYPE name(                             \
        Nan::NAN_PROPERTY_ENUMERATOR_ARGS_TYPE info)
#define NAN_PROPERTY_DELETER(name)                                             \
    Nan::NAN_PROPERTY_DELETER_RETURN_TYPE name(                                \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_DELETER_ARGS_TYPE info)
#define NAN_PROPERTY_QUERY(name)                                               \
    Nan::NAN_PROPERTY_QUERY_RETURN_TYPE name(                                  \
        v8::Local<v8::String> property                                         \
      , Nan::NAN_PROPERTY_QUERY_ARGS_TYPE info)
# define NAN_INDEX_GETTER(name)                                                \
    Nan::NAN_INDEX_GETTER_RETURN_TYPE name(                                    \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_GETTER_ARGS_TYPE info)
#define NAN_INDEX_SETTER(name)                                                 \
    Nan::NAN_INDEX_SETTER_RETURN_TYPE name(                                    \
        uint32_t index                                                         \
      , v8::Local<v8::Value> value                                             \
      , Nan::NAN_INDEX_SETTER_ARGS_TYPE info)
#define NAN_INDEX_ENUMERATOR(name)                                             \
    Nan::NAN_INDEX_ENUMERATOR_RETURN_TYPE                                      \
    name(Nan::NAN_INDEX_ENUMERATOR_ARGS_TYPE info)
#define NAN_INDEX_DELETER(name)                                                \
    Nan::NAN_INDEX_DELETER_RETURN_TYPE name(                                   \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_DELETER_ARGS_TYPE info)
#define NAN_INDEX_QUERY(name)                                                  \
    Nan::NAN_INDEX_QUERY_RETURN_TYPE name(                                     \
        uint32_t index                                                         \
      , Nan::NAN_INDEX_QUERY_ARGS_TYPE info)

class Callback {
 public:
  Callback() {
    HandleScope scope;
    v8::Local<v8::Object> obj = New<v8::Object>();
    handle.Reset(obj);
  }

  explicit Callback(const v8::Local<v8::Function> &fn) {
    HandleScope scope;
    v8::Local<v8::Object> obj = New<v8::Object>();
    handle.Reset(obj);
    SetFunction(fn);
  }

  ~Callback() {
    if (handle.IsEmpty()) return;
    handle.Reset();
  }

  bool operator==(const Callback &other) const {
    HandleScope scope;
    v8::Local<v8::Value> a = New(handle)->Get(kCallbackIndex);
    v8::Local<v8::Value> b = New(other.handle)->Get(kCallbackIndex);
    return a->StrictEquals(b);
  }

  bool operator!=(const Callback &other) const {
    return !this->operator==(other);
  }

  NAN_INLINE
  v8::Local<v8::Function> operator*() const { return this->GetFunction(); }

  NAN_INLINE v8::Local<v8::Value> operator()(
      v8::Local<v8::Object> target
    , int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    return this->Call(target, argc, argv);
  }

  NAN_INLINE v8::Local<v8::Value> operator()(
      int argc = 0
    , v8::Local<v8::Value> argv[] = 0) const {
    return this->Call(argc, argv);
  }

  NAN_INLINE void SetFunction(const v8::Local<v8::Function> &fn) {
    HandleScope scope;
    Set(New(handle), kCallbackIndex, fn);
  }

  NAN_INLINE v8::Local<v8::Function> GetFunction() const {
    EscapableHandleScope scope;
    return scope.Escape(New(handle)->Get(kCallbackIndex)
        .As<v8::Function>());
  }

  NAN_INLINE bool IsEmpty() const {
    HandleScope scope;
    return New(handle)->Get(kCallbackIndex)->IsUndefined();
  }

  NAN_INLINE v8::Local<v8::Value>
  Call(v8::Local<v8::Object> target
     , int argc
     , v8::Local<v8::Value> argv[]) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    return Call_(isolate, target, argc, argv);
  }

  NAN_INLINE v8::Local<v8::Value>
  Call(int argc, v8::Local<v8::Value> argv[]) const {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    return Call_(isolate, isolate->GetCurrentContext()->Global(), argc, argv);
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(Callback)
  Persistent<v8::Object> handle;
  static const uint32_t kCallbackIndex = 0;


  v8::Local<v8::Value> Call_(v8::Isolate *isolate
                           , v8::Local<v8::Object> target
                           , int argc
                           , v8::Local<v8::Value> argv[]) const {
    EscapableHandleScope scope;

    v8::Local<v8::Function> callback = New(handle)->
        Get(kCallbackIndex).As<v8::Function>();
    return scope.Escape(node::MakeCallback(
        isolate
      , target
      , callback
      , argc
      , argv
    ));
  }
};

/* abstract */ class AsyncWorker {
 public:
  explicit AsyncWorker(Callback *callback_)
      : callback(callback_), errmsg_(NULL) {
    request.data = this;

    HandleScope scope;
    v8::Local<v8::Object> obj = New<v8::Object>();
    persistentHandle.Reset(obj);
  }

  virtual ~AsyncWorker() {
    HandleScope scope;

    if (!persistentHandle.IsEmpty())
      persistentHandle.Reset();
    delete callback;
    delete[] errmsg_;
  }

  virtual void WorkComplete() {
    HandleScope scope;

    if (errmsg_ == NULL)
      HandleOKCallback();
    else
      HandleErrorCallback();
    delete callback;
    callback = NULL;
  }

  NAN_INLINE void SaveToPersistent(
      const char *key, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(New(key).ToLocalChecked(), value);
  }

  NAN_INLINE void SaveToPersistent(
      const v8::Local<v8::String> &key, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(key, value);
  }

  NAN_INLINE void SaveToPersistent(
      uint32_t index, const v8::Local<v8::Value> &value) {
    HandleScope scope;
    New(persistentHandle)->Set(index, value);
  }

  NAN_INLINE v8::Local<v8::Value> GetFromPersistent(const char *key) const {
    EscapableHandleScope scope;
    return scope.Escape(
        New(persistentHandle)->Get(New(key).ToLocalChecked()));
  }

  NAN_INLINE v8::Local<v8::Value>
  GetFromPersistent(const v8::Local<v8::String> &key) const {
    EscapableHandleScope scope;
    return scope.Escape(New(persistentHandle)->Get(key));
  }

  NAN_INLINE v8::Local<v8::Value> GetFromPersistent(uint32_t index) const {
    EscapableHandleScope scope;
    return scope.Escape(New(persistentHandle)->Get(index));
  }

  virtual void Execute() = 0;

  uv_work_t request;

  virtual void Destroy() {
      delete this;
  }

 protected:
  Persistent<v8::Object> persistentHandle;
  Callback *callback;

  virtual void HandleOKCallback() {
    callback->Call(0, NULL);
  }

  virtual void HandleErrorCallback() {
    HandleScope scope;

    v8::Local<v8::Value> argv[] = {
      v8::Exception::Error(New<v8::String>(ErrorMessage()).ToLocalChecked())
    };
    callback->Call(1, argv);
  }

  void SetErrorMessage(const char *msg) {
    delete[] errmsg_;

    size_t size = strlen(msg) + 1;
    errmsg_ = new char[size];
    memcpy(errmsg_, msg, size);
  }

  const char* ErrorMessage() const {
    return errmsg_;
  }

 private:
  NAN_DISALLOW_ASSIGN_COPY_MOVE(AsyncWorker)
  char *errmsg_;
};

/* abstract */ class AsyncProgressWorker : public AsyncWorker {
 public:
  explicit AsyncProgressWorker(Callback *callback_)
      : AsyncWorker(callback_), asyncdata_(NULL), asyncsize_(0) {
    async = new uv_async_t;
    uv_async_init(
        uv_default_loop()
      , async
      , AsyncProgress_
    );
    async->data = this;

    uv_mutex_init(&async_lock);
  }

  virtual ~AsyncProgressWorker() {
    uv_mutex_destroy(&async_lock);

    delete[] asyncdata_;
  }

  void WorkProgress() {
    uv_mutex_lock(&async_lock);
    char *data = asyncdata_;
    size_t size = asyncsize_;
    asyncdata_ = NULL;
    uv_mutex_unlock(&async_lock);

    // Dont send progress events after we've already completed.
    if (callback) {
        HandleProgressCallback(data, size);
    }
    delete[] data;
  }

  class ExecutionProgress {
    friend class AsyncProgressWorker;
   public:
    void Signal() const {
        uv_async_send(that_->async);
    }
    // You could do fancy generics with templates here.
    void Send(const char* data, size_t size) const {
        that_->SendProgress_(data, size);
    }

   private:
    explicit ExecutionProgress(AsyncProgressWorker* that) : that_(that) {}
    NAN_DISALLOW_ASSIGN_COPY_MOVE(ExecutionProgress)
    AsyncProgressWorker* const that_;
  };

  virtual void Execute(const ExecutionProgress& progress) = 0;
  virtual void HandleProgressCallback(const char *data, size_t size) = 0;

  virtual void Destroy() {
      uv_close(reinterpret_cast<uv_handle_t*>(async), AsyncClose_);
  }

 private:
  void Execute() /*final override*/ {
      ExecutionProgress progress(this);
      Execute(progress);
  }

  void SendProgress_(const char *data, size_t size) {
    char *new_data = new char[size];
    memcpy(new_data, data, size);

    uv_mutex_lock(&async_lock);
    char *old_data = asyncdata_;
    asyncdata_ = new_data;
    asyncsize_ = size;
    uv_mutex_unlock(&async_lock);

    delete[] old_data;
    uv_async_send(async);
  }

  NAN_INLINE static NAUV_WORK_CB(AsyncProgress_) {
    AsyncProgressWorker *worker =
            static_cast<AsyncProgressWorker*>(async->data);
    worker->WorkProgress();
  }

  NAN_INLINE static void AsyncClose_(uv_handle_t* handle) {
    AsyncProgressWorker *worker =
            static_cast<AsyncProgressWorker*>(handle->data);
    delete reinterpret_cast<uv_async_t*>(handle);
    delete worker;
  }

  uv_async_t *async;
  uv_mutex_t async_lock;
  char *asyncdata_;
  size_t asyncsize_;
};

NAN_INLINE void AsyncExecute (uv_work_t* req) {
  AsyncWorker *worker = static_cast<AsyncWorker*>(req->data);
  worker->Execute();
}

NAN_INLINE void AsyncExecuteComplete (uv_work_t* req) {
  AsyncWorker* worker = static_cast<AsyncWorker*>(req->data);
  worker->WorkComplete();
  worker->Destroy();
}

NAN_INLINE void AsyncQueueWorker (AsyncWorker* worker) {
  uv_queue_work(
      uv_default_loop()
    , &worker->request
    , AsyncExecute
    , reinterpret_cast<uv_after_work_cb>(AsyncExecuteComplete)
  );
}

namespace imp {

inline
ExternalOneByteStringResource const*
GetExternalResource(v8::Local<v8::String> str) {
    return str->GetExternalOneByteStringResource();
}

inline
bool
IsExternal(v8::Local<v8::String> str) {
    return str->IsExternalOneByte();
}

}  // end of namespace imp

enum Encoding {ASCII, UTF8, BASE64, UCS2, BINARY, HEX, BUFFER};


NAN_INLINE v8::Local<v8::Value> Encode(
    const void *buf, size_t len, enum Encoding encoding = BINARY) {

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  node::encoding node_enc = static_cast<node::encoding>(encoding);

  if (encoding == UCS2) {
    return node::Encode(
        isolate
      , reinterpret_cast<const uint16_t *>(buf)
      , len / 2);
  } else {
    return node::Encode(
        isolate
      , reinterpret_cast<const char *>(buf)
      , len
      , node_enc);
  }
}

NAN_INLINE ssize_t DecodeBytes(
    v8::Local<v8::Value> val, enum Encoding encoding = BINARY) {

  return node::DecodeBytes(
      v8::Isolate::GetCurrent()
    , val
    , static_cast<node::encoding>(encoding));
}

NAN_INLINE ssize_t DecodeWrite(
    char *buf
  , size_t len
  , v8::Local<v8::Value> val
  , enum Encoding encoding = BINARY) {

  return node::DecodeWrite(
      v8::Isolate::GetCurrent()
    , buf
    , len
    , val
    , static_cast<node::encoding>(encoding));
}

NAN_INLINE void SetPrototypeTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , const char *name
  , v8::Local<v8::Data> value
) {
  SetTemplate(templ->PrototypeTemplate(), name, value);
}

NAN_INLINE void SetPrototypeTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , v8::Local<v8::String> name
  , v8::Local<v8::Data> value
  , v8::PropertyAttribute attributes
) {
  SetTemplate(templ->PrototypeTemplate(), name, value, attributes);
}

NAN_INLINE void SetInstanceTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , const char *name
  , v8::Local<v8::Data> value
) {
  SetTemplate(templ->InstanceTemplate(), name, value);
}

NAN_INLINE void SetInstanceTemplate(
    v8::Local<v8::FunctionTemplate> templ
  , v8::Local<v8::String> name
  , v8::Local<v8::Data> value
  , v8::PropertyAttribute attributes
) {
  SetTemplate(templ->InstanceTemplate(), name, value, attributes);
}

namespace imp {

// Note(@agnat): Helper to distinguish different receiver types. The first
// version deals with receivers derived from v8::Template. The second version
// handles everything else. The final argument only serves as discriminator and
// is unused.
template <typename T>
NAN_INLINE
void
SetMethodAux(T recv,
             v8::Local<v8::String> name,
             v8::Local<v8::FunctionTemplate> tpl,
             v8::Template *) {
  recv->Set(name, tpl);
}

template <typename T>
NAN_INLINE
void
SetMethodAux(T recv,
             v8::Local<v8::String> name,
             v8::Local<v8::FunctionTemplate> tpl,
             ...) {
  recv->Set(name, GetFunction(tpl).ToLocalChecked());
}

}  // end of namespace imp

template <typename T, template <typename> class HandleType>
NAN_INLINE void SetMethod(
    HandleType<T> recv
  , const char *name
  , FunctionCallback callback) {
  HandleScope scope;
  v8::Local<v8::FunctionTemplate> t = New<v8::FunctionTemplate>(callback);
  v8::Local<v8::String> fn_name = New(name).ToLocalChecked();
  t->SetClassName(fn_name);
  // Note(@agnat): Pass an empty T* as discriminator. See note on
  // SetMethodAux(...) above
  imp::SetMethodAux(recv, fn_name, t, static_cast<T*>(0));
}

NAN_INLINE void SetPrototypeMethod(
    v8::Local<v8::FunctionTemplate> recv
  , const char* name, FunctionCallback callback) {
  HandleScope scope;
  v8::Local<v8::FunctionTemplate> t = New<v8::FunctionTemplate>(
      callback
    , v8::Local<v8::Value>()
    , New<v8::Signature>(recv));
  v8::Local<v8::String> fn_name = New(name).ToLocalChecked();
  recv->PrototypeTemplate()->Set(fn_name, t);
  t->SetClassName(fn_name);
}

//=== Accessors and Such =======================================================

inline void SetAccessor(
    v8::Local<v8::ObjectTemplate> tpl
  , v8::Local<v8::String> name
  , GetterCallback getter
  , SetterCallback setter = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None
  , imp::Sig signature = imp::Sig()) {
  HandleScope scope;

  imp::NativeGetter getter_ =
      imp::GetterCallbackWrapper;
  imp::NativeSetter setter_ =
      setter ? imp::SetterCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kAccessorFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter != 0) {
    obj->SetInternalField(
        imp::kSetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetAccessor(
      name
    , getter_
    , setter_
    , obj
    , settings
    , attribute
    , signature);
}

inline bool SetAccessor(
    v8::Local<v8::Object> obj
  , v8::Local<v8::String> name
  , GetterCallback getter
  , SetterCallback setter = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()
  , v8::AccessControl settings = v8::DEFAULT
  , v8::PropertyAttribute attribute = v8::None) {
  EscapableHandleScope scope;

  imp::NativeGetter getter_ =
      imp::GetterCallbackWrapper;
  imp::NativeSetter setter_ =
      setter ? imp::SetterCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kAccessorFieldCount);
  v8::Local<v8::Object> dataobj = NewInstance(otpl).ToLocalChecked();

  dataobj->SetInternalField(
      imp::kGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (!data.IsEmpty()) {
    dataobj->SetInternalField(imp::kDataIndex, data);
  }

  if (setter) {
    dataobj->SetInternalField(
        imp::kSetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  return obj->SetAccessor(
      name
    , getter_
    , setter_
    , dataobj
    , settings
    , attribute);
}

inline void SetNamedPropertyHandler(
    v8::Local<v8::ObjectTemplate> tpl
  , PropertyGetterCallback getter
  , PropertySetterCallback setter = 0
  , PropertyQueryCallback query = 0
  , PropertyDeleterCallback deleter = 0
  , PropertyEnumeratorCallback enumerator = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  imp::NativePropertyGetter getter_ =
      imp::PropertyGetterCallbackWrapper;
  imp::NativePropertySetter setter_ =
      setter ? imp::PropertySetterCallbackWrapper : 0;
  imp::NativePropertyQuery query_ =
      query ? imp::PropertyQueryCallbackWrapper : 0;
  imp::NativePropertyDeleter *deleter_ =
      deleter ? imp::PropertyDeleterCallbackWrapper : 0;
  imp::NativePropertyEnumerator enumerator_ =
      enumerator ? imp::PropertyEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kPropertyFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();
  obj->SetInternalField(
      imp::kPropertyGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter) {
    obj->SetInternalField(
        imp::kPropertySetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (query) {
    obj->SetInternalField(
        imp::kPropertyQueryIndex
      , New<v8::External>(reinterpret_cast<void *>(query)));
  }

  if (deleter) {
    obj->SetInternalField(
        imp::kPropertyDeleterIndex
      , New<v8::External>(reinterpret_cast<void *>(deleter)));
  }

  if (enumerator) {
    obj->SetInternalField(
        imp::kPropertyEnumeratorIndex
      , New<v8::External>(reinterpret_cast<void *>(enumerator)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetHandler(v8::NamedPropertyHandlerConfiguration(
      getter_, setter_, query_, deleter_, enumerator_, obj));
}

inline void SetIndexedPropertyHandler(
    v8::Local<v8::ObjectTemplate> tpl
  , IndexGetterCallback getter
  , IndexSetterCallback setter = 0
  , IndexQueryCallback query = 0
  , IndexDeleterCallback deleter = 0
  , IndexEnumeratorCallback enumerator = 0
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  imp::NativeIndexGetter getter_ =
      imp::IndexGetterCallbackWrapper;
  imp::NativeIndexSetter setter_ =
      setter ? imp::IndexSetterCallbackWrapper : 0;
  imp::NativeIndexQuery query_ =
      query ? imp::IndexQueryCallbackWrapper : 0;
  imp::NativeIndexDeleter deleter_ =
      deleter ? imp::IndexDeleterCallbackWrapper : 0;
  imp::NativeIndexEnumerator enumerator_ =
      enumerator ? imp::IndexEnumeratorCallbackWrapper : 0;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kIndexPropertyFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();
  obj->SetInternalField(
      imp::kIndexPropertyGetterIndex
    , New<v8::External>(reinterpret_cast<void *>(getter)));

  if (setter) {
    obj->SetInternalField(
        imp::kIndexPropertySetterIndex
      , New<v8::External>(reinterpret_cast<void *>(setter)));
  }

  if (query) {
    obj->SetInternalField(
        imp::kIndexPropertyQueryIndex
      , New<v8::External>(reinterpret_cast<void *>(query)));
  }

  if (deleter) {
    obj->SetInternalField(
        imp::kIndexPropertyDeleterIndex
      , New<v8::External>(reinterpret_cast<void *>(deleter)));
  }

  if (enumerator) {
    obj->SetInternalField(
        imp::kIndexPropertyEnumeratorIndex
      , New<v8::External>(reinterpret_cast<void *>(enumerator)));
  }

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetHandler(v8::IndexedPropertyHandlerConfiguration(
      getter_, setter_, query_, deleter_, enumerator_, obj));
}

inline void SetCallHandler(
    v8::Local<v8::FunctionTemplate> tpl
  , FunctionCallback callback
  , v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kFunctionFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kFunctionIndex
    , New<v8::External>(reinterpret_cast<void *>(callback)));

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetCallHandler(imp::FunctionCallbackWrapper, obj);
}


inline void SetCallAsFunctionHandler(
    v8::Local<v8::ObjectTemplate> tpl,
    FunctionCallback callback,
    v8::Local<v8::Value> data = v8::Local<v8::Value>()) {
  HandleScope scope;

  v8::Local<v8::ObjectTemplate> otpl = New<v8::ObjectTemplate>();
  otpl->SetInternalFieldCount(imp::kFunctionFieldCount);
  v8::Local<v8::Object> obj = NewInstance(otpl).ToLocalChecked();

  obj->SetInternalField(
      imp::kFunctionIndex
    , New<v8::External>(reinterpret_cast<void *>(callback)));

  if (!data.IsEmpty()) {
    obj->SetInternalField(imp::kDataIndex, data);
  }

  tpl->SetCallAsFunctionHandler(imp::FunctionCallbackWrapper, obj);
}

//=== Weak Persistent Handling =================================================

#include "nan_weak.h"  // NOLINT(build/include)

//=== ObjectWrap ===============================================================

#include "nan_object_wrap.h"  // NOLINT(build/include)

//=== Export ==================================================================

inline
void
Export(ADDON_REGISTER_FUNCTION_ARGS_TYPE target, const char *name,
    FunctionCallback f) {
  Set(target, New<v8::String>(name).ToLocalChecked(),
      GetFunction(New<v8::FunctionTemplate>(f)).ToLocalChecked());
}

//=== Tap Reverse Binding =====================================================

struct Tap {
  explicit Tap(v8::Local<v8::Value> t) : t_() {
    t_.Reset(To<v8::Object>(t).ToLocalChecked());
  }

  ~Tap() { t_.Reset(); }  // not sure if neccessary

  inline void plan(int i) {
    v8::Local<v8::Value> arg = New(i);
    MakeCallback(New(t_), "plan", 1, &arg);
  }

  inline void ok(bool isOk, const char *msg = NULL) {
    v8::Local<v8::Value> args[2];
    args[0] = New(isOk);
    if (msg) args[1] = New(msg).ToLocalChecked();
    MakeCallback(New(t_), "ok", msg ? 2 : 1, args);
  }

  inline void pass(const char * msg = NULL) {
    v8::Local<v8::Value> hmsg;
    if (msg) hmsg = New(msg).ToLocalChecked();
    MakeCallback(New(t_), "pass", msg ? 1 : 0, &hmsg);
  }

 private:
  Persistent<v8::Object> t_;
};

#define NAN_STRINGIZE2(x) #x
#define NAN_STRINGIZE(x) NAN_STRINGIZE2(x)
#define NAN_TEST_EXPRESSION(expression) \
  ( expression ), __FILE__ ":" NAN_STRINGIZE(__LINE__) ": " #expression

#define NAN_EXPORT(target, function) Export(target, #function, function)

#undef TYPE_CHECK

//=== Generic Maybefication ===================================================

namespace imp {

template <typename T> struct Maybefier;

template <typename T> struct Maybefier<v8::Local<T> > {
  static MaybeLocal<T> convert(v8::Local<T> v) {
    return MaybeLocal<T>(v);
  }
};

template <typename T> struct Maybefier<MaybeLocal<T> > {
  static MaybeLocal<T> convert(MaybeLocal<T> v) {
    return v;
  }
};

}  // end of namespace imp

template <typename T, template <typename> class MaybeMaybe>
MaybeLocal<T>
MakeMaybe(MaybeMaybe<T> v) {
  return imp::Maybefier<MaybeMaybe<T> >::convert(v);
}

//=== TypedArrayContents =======================================================

#include "nan_typedarray_contents.h"  // NOLINT(build/include)

}  // end of namespace Nan

#endif  // NAN_H_
