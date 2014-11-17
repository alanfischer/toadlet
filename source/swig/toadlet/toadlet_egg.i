
namespace toadlet{
namespace egg{

%naturalvar String;

class String;

// String
%typemap(jni) String "jstring"
%typemap(jtype) String "String"
%typemap(jstype) String "String"
%typemap(javadirectorin) String "$jniinput"
%typemap(javadirectorout) String "$javacall"

%typemap(in) String %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$1=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorout) String %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$result=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorin,descriptor="Ljava/lang/String;") String %{ $input = jenv->NewStringUTF($1.c_str()); %}

%typemap(out) String %{ $result = jenv->NewStringUTF($1.c_str()); %}

%typemap(javain) String "$javainput"

%typemap(javaout) String {
	return $jnicall;
}

// const String &
%typemap(jni) const String & "jstring"
%typemap(jtype) const String & "String"
%typemap(jstype) const String & "String"
%typemap(javadirectorin) const String & "$jniinput"
%typemap(javadirectorout) const String & "$javacall"

%typemap(in) const String & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$1=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorout) const String & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$result=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorin,descriptor="Ljava/lang/String;") const String & %{ $input = jenv->NewStringUTF($1.c_str()); %}

%typemap(out) const String & %{ $result = jenv->NewStringUTF($1.c_str()); %}

%typemap(javain) const String & "$javainput"

%typemap(javaout) const String & {
	return $jnicall;
}

%typemap(typecheck) String = char *;


%naturalvar TUUID;

class TUUID;

// TUUID
%typemap(jni) TUUID "jlongArray"
%typemap(jtype) TUUID "long[]"
%typemap(jstype) TUUID "java.util.UUID"
%typemap(javadirectorin) TUUID "new java.util.UUID($jniinput[0],$jniinput[1])"
%typemap(javadirectorout) TUUID "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(in) TUUID %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]);
%}

%typemap(directorout) TUUID %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]);
%}

%typemap(directorin,descriptor="Ljava/util/UUID;") TUUID %{
	$input = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($input,0,2,$1_bits);
%}

%typemap(out) TUUID %{
	$result = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($result,0,2,$1_bits);
%}
	
%typemap(javain) TUUID "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(javaout) TUUID {
	long temp[]=$jnicall;
	return new java.util.UUID(temp[0],temp[1]);
}

// const TUUID &
%typemap(jni) const TUUID & "jlongArray"
%typemap(jtype) const TUUID & "long[]"
%typemap(jstype) const TUUID & "java.util.UUID"
%typemap(javadirectorin) const TUUID & "new java.util.UUID($jniinput[0],$jniinput[1])"
%typemap(javadirectorout) const TUUID & "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(in) const TUUID & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]);
%}

%typemap(directorout) const TUUID & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]);
%}

%typemap(directorin,descriptor="Ljava/util/UUID;") const TUUID & %{
	$input = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($input,0,2,$1_bits);
%}

%typemap(out) const TUUID & %{
	$result = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($result,0,2,$1_bits);
%}
	
%typemap(javain) const TUUID & "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(javaout) const TUUID & {
	long temp[]=$jnicall;
	return new java.util.UUID(temp[0],temp[1]);
}


%naturalvar Exception;

class Exception;

// Exception
%typemap(jni) Exception "jstring"
%typemap(jtype) Exception "String"
%typemap(jstype) Exception "java.lang.Exception"
%typemap(javadirectorin) Exception "new java.lang.Exception($jniinput)"
%typemap(javadirectorout) Exception "$javainput.getMessage()"

%typemap(in) Exception %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null Exception");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	*$1=Exception($1_pstr);
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorout) Exception %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null Exception");
		return $null;
	}
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	*$result=Exception($1_pstr);
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorin,descriptor="Ljava/lang/Exception;") Exception %{
	$input = jenv->NewStringUTF($1.what());
%}

%typemap(out) Exception %{
	$input = jenv->NewStringUTF($1.what());
%}
	
%typemap(javain) Exception "$javainput.getMessage()"

%typemap(javaout) Exception {
	return new java.lang.RuntimeException($jnicall);
}

// const Exception &
%typemap(jni) const Exception & "jstring"
%typemap(jtype) const Exception & "String"
%typemap(jstype) const Exception & "java.lang.Exception"
%typemap(javadirectorin) const Exception & "new java.lang.Exception($jniinput)"
%typemap(javadirectorout) const Exception & "$javainput.getMessage()"

%typemap(in) const Exception & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null Exception");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	*$1=Exception($1_pstr);
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorout) const Exception & %{
	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null Exception");
		return $null;
	}
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	*$result=Exception($1_pstr);
	jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorin,descriptor="Ljava/lang/Exception;") const Exception & %{
	$input = jenv->NewStringUTF($1.what());
%}

%typemap(out) const Exception & %{
	$input = jenv->NewStringUTF($1.what());
%}
	
%typemap(javain) const Exception & "$javainput.getMessage()"

%typemap(javaout) const Exception & {
	return new java.lang.RuntimeException($jnicall);
}

}

typedef float scalar;
typedef long long uint64;
typedef long long int64;
typedef signed char tbyte;

}

using namespace toadlet::egg;
using namespace toadlet;

%inline %{
template<typename RangeType>
struct RangeIterator {
	RangeIterator(const RangeType& r) : range(r), it(range.begin()) {}
	bool hasNext() const { return it != range.end(); }
	typename RangeType::value_type nextImpl() { return *it++; }
	
private:
	RangeType range;    
	typename RangeType::iterator it;
};
%}
