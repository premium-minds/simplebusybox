#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static JNIEnv *env;

static jclass cl_system;
static jclass cl_printstream;
static jclass cl_string;
static jclass cl_simplebusybox;

static jmethodID mid_println;
static jmethodID mid_sbb_update_status;
static jmethodID mid_sbb_get_version;

static jfieldID fid_out;

static int
jni_init(JNIEnv *env_)
{
	env = env_;
#define CLASS(var, id) \
	cl_##var = (*env)->FindClass(env, id); \
	if (!cl_##var) return 0;
#define METHOD(var, mycl, id, sig) \
	mid_##var = (*env)->GetMethodID(env, cl_##mycl, id, sig); \
	if (!mid_##var) return 0;
#define FIELD(var, mycl, id, sig) \
	fid_##var = (*env)->GetFieldID(env, cl_##mycl, id, sig); \
	if (!fid_##var) return 0;
#define STFIELD(var, mycl, id, sig) \
	fid_##var = (*env)->GetStaticFieldID(env, cl_##mycl, id, sig); \
	if (!fid_##var) return 0;

	CLASS(system, "java/lang/System")
	CLASS(printstream, "java/io/PrintStream")
	CLASS(string, "java/lang/String")
	CLASS(simplebusybox, "org/galexander/busybox/SimpleBusyBox")

	METHOD(println, printstream, "println", "(Ljava/lang/String;)V")
	METHOD(sbb_update_status, simplebusybox, "update_status",
			"(Ljava/lang/String;)V")
	METHOD(sbb_get_version, simplebusybox, "get_version", "()I")

	STFIELD(out, system, "out", "Ljava/io/PrintStream;")

	return 1;
}

static void
println(const char *fmt, ...)
{
	char buf[1000];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf,fmt,ap);
	va_end(ap);
	jstring *x = (*env)->NewStringUTF(env, buf);
	if (!x) return;
	jobject out = (*env)->GetStaticObjectField(env, cl_system, fid_out);
	if (!out) return;
	(*env)->CallVoidMethod(env, out, mid_println, x);
	(*env)->DeleteLocalRef(env, out);
	(*env)->DeleteLocalRef(env, x);
}

static void
update_status(jobject this, char *s)
{
	jstring *x = (*env)->NewStringUTF(env, s);
	if (!x) return;
	(*env)->CallVoidMethod(env, this, mid_sbb_update_status, x);
	(*env)->DeleteLocalRef(env, x);
}

static char *
string_from_java(jstring s)
{
	const char *t = (*env)->GetStringUTFChars(env, s, NULL);
	char *ret;
	if (!t) return NULL;
	ret = strdup(t);
	(*env)->ReleaseStringUTFChars(env, s, t);
	return ret;
}

static char *
busybox_fn(char *p)
{
	int len = strlen(p);
	char *ret = malloc(len+40);
	sprintf(ret, "%s/busybox", p);
	return ret;
}

static char *
busybox_verfn(char *p)
{
	int len = strlen(p);
	char *ret = malloc(len+40);
	sprintf(ret, "%s/busybox_version", p);
	return ret;
}

static int
check_present(char *p)
{
	return (access(busybox_fn(p), R_OK|X_OK) == 0);
}

static int
check_version(char *p, int expected)
{
	int fd;
	char buf[100];
	int i;

	fd = open(busybox_verfn(p), O_RDONLY);
	if (fd < 0) {
		return 0;
	}
	i = read(fd, buf, (sizeof buf)-1);
	close(fd);
	if (i <= 0) {
		return 0;
	}
	buf[i] = 0;
	return (strtoul(buf, NULL, 10) == expected);
}

JNIEXPORT void JNICALL
Java_org_galexander_busybox_SimpleBusyBox_determine_1status(JNIEnv *env_,
		jobject this, jstring path)
{
	char *p;
	int ver;

	if (!jni_init(env_)) {
		return;
	}

	p = string_from_java(path);
	ver = (*env)->CallIntMethod(env, this, mid_sbb_get_version);
	if (!check_present(p)) {
		update_status(this, "Not unpacked.");
	} else if (!check_version(p, ver)) {
		update_status(this, "Wrong version is unpacked.");
	} else {
		char *buf = malloc(strlen(p)+100);
		sprintf(buf, "Has been unpacked into:\n%s", p);
		update_status(this, buf);
	}
/* XXX
	if (chmod(p, 00755)) {
		update_status(this, "chmod error");
	} else {
		update_status(this, "succ");
	}
*/
}
