
#define VERSION_MAJOR		1
#define REVISION_MAJOR		0
#define VERSION_MINOR		0
#define REVISION_MINOR		11

#define TOSTRINGTMP(x)		#x
#define TOSTRING(x)		TOSTRINGTMP(x)

#define VERSION_STRING_NZ	TOSTRING(VERSION_MAJOR) "." TOSTRING(REVISION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(REVISION_MINOR) "\0"
