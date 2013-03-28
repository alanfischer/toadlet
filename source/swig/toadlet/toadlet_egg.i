
namespace toadlet{
namespace egg{

// String
%naturalvar String;

class String;

%typemap(jni) String "jstring"
%typemap(jtype) String "String"
%typemap(jstype) String "String"
%typemap(javadirectorin) String "$jniinput"
%typemap(javadirectorout) String "$javacall"

%typemap(in) String 
%{	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$1=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorout) String 
%{	if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null String");
		return $null;
	} 
	const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0); 
	if (!$1_pstr) return $null;
	$result=$1_pstr;
	jenv->ReleaseStringUTFChars($input, $1_pstr); %}

%typemap(directorin,descriptor="Ljava/lang/String;") String
%{ $input = jenv->NewStringUTF($1.c_str()); %}

%typemap(out) String
%{ $result = jenv->NewStringUTF($1.c_str()); %}

%typemap(javain) String "$javainput"

%typemap(javaout) String {
	return $jnicall;
}

%typemap(typecheck) String = char *;

// TUUID
%naturalvar TUUID;

class TUUID;

%typemap(jni) TUUID "jlongArray"
%typemap(jtype) TUUID "long[]"
%typemap(jstype) TUUID "java.util.UUID"
%typemap(javadirectorin) TUUID "new java.util.UUID($jniinput[0],$jniinput[1])"
%typemap(javadirectorout) TUUID "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(in) TUUID 
%{ if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]); %}

%typemap(directorout) TUUID 
%{ if(!$input) {
		SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null UUID");
		return $null;
	}
	jlong $1_bits[2];
	jenv->GetLongArrayRegion($input,0,2,$1_bits);
	$1=TUUID($1_bits[0],$1_bits[1]); %}

%typemap(directorin,descriptor="Ljava/util/UUID;") TUUID
%{ $input = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($input,0,2,$1_bits); %}

%typemap(out) TUUID
%{ $result = jenv->NewLongArray(2);
	jlong $1_bits[2];
	$1_bits[0]=$1.getHighBits();$1_bits[1]=$1.getLowBits();
	jenv->SetLongArrayRegion($result,0,2,$1_bits); %}
	
%typemap(javain) TUUID "new long[]{$javainput.getMostSignificantBits(),$javainput.getLeastSignificantBits()}"

%typemap(javaout) TUUID {
	long temp[]=$jnicall;
	return new java.util.UUID(temp[0],temp[1]);
}

}

typedef float scalar;
typedef long long uint64;
typedef long long int64;
typedef signed char tbyte;

}

using namespace toadlet::egg;
using namespace toadlet;