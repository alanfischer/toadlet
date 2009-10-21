-renamesourcefileattribute SourceFile

-keepattributes Exceptions,InnerClasses,Signature,Deprecated,SourceFile,LineNumberTable,*Annotation*,EnclosingMethod

-keep public class * {
	public protected *;
}

-keepclassmembernames class * {
	java.lang.Class class$(java.lang.String);
	java.lang.Class class$(java.lang.String, boolean);
}

-keepclasseswithmembernames class * {
	native <methods>;
	}

-keepclassmembers class * extends java.lang.Enum {
	public static **[] values();
	public static ** valueOf(java.lang.String);
}

-keepclassmembers class * implements java.io.Serializable {
	static final long serialVersionUID;
	private void writeObject(java.io.ObjectOutputStream);
	private void readObject(java.io.ObjectInputStream);
	java.lang.Object writeReplace();
	java.lang.Object readResolve();
}

