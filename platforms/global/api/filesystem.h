#pragma once

#include <sys/stat.h>
#include <asserts.h>
#include <logger.h>
#include <fcntl.h>

#ifndef S_IRUSR
#define S_IRUSR RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IWUSR
#define S_IWUSR RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IXUSR
#define S_IXUSR RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IRWXU
#define S_IRWXU RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IRGRP
#define S_IRGRP RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IWGRP
#define S_IWGRP RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IXGRP
#define S_IXGRP RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IRWXG
#define S_IRWXG RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IROTH
#define S_IROTH RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IWOTH
#define S_IWOTH RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IXOTH
#define S_IXOTH RK_FILE_PERMISSION_NONE
#endif
#ifndef S_IRWXO
#define S_IRWXO RK_FILE_PERMISSION_NONE
#endif

typedef enum {
	RK_FILE_ACCESS_READ_ONLY	= O_RDONLY,
	RK_FILE_ACCESS_WRITE_ONLY	= O_WRONLY,
	RK_FILE_ACCESS_READ_WRITE	= O_RDWR,
	RK_FILE_ACCESS_CREATE		= O_CREAT,
	RK_FILE_ACCESS_TRUNCATE		= O_TRUNC,
} RkFileAccessFlags;

typedef enum {
	RK_FILE_PERMISSION_NONE				= 0,
	RK_FILE_PERMISSION_READ_OWNER		= S_IRUSR,
	RK_FILE_PERMISSION_WRITE_OWNER		= S_IWUSR,
	RK_FILE_PERMISSION_EXECUTE_OWNER	= S_IXUSR,
	RK_FILE_PERMISSION_ALL_OWNER		= S_IRWXU,
	RK_FILE_PERMISSION_READ_GROUP		= S_IRGRP,
	RK_FILE_PERMISSION_WRITE_GROUP		= S_IWGRP,
	RK_FILE_PERMISSION_EXECUTE_GROUP	= S_IXGRP,
	RK_FILE_PERMISSION_ALL_GROUP		= S_IRWXG,
	RK_FILE_PERMISSION_READ_OTHER		= S_IROTH,
	RK_FILE_PERMISSION_WRITE_OTHER		= S_IWOTH,
	RK_FILE_PERMISSION_EXECUTE_OTHER	= S_IXOTH,
	RK_FILE_PERMISSION_ALL_OTHER		= S_IRWXO,
	RK_FILE_PERMISSION_ALL				= S_IRWXU | S_IRWXG | S_IRWXO,
} RkFilePermissionFlags;

typedef struct {
	const char * debugPath;
	int fd;
} RkFile;

RkResult rkOpenFile(RkFile * const restrict file, const char * const path,
					const RkFileAccessFlags access, const RkFilePermissionFlags permissions);
uint_fast32_t rkReadFile(const RkFile * const restrict file, void * const buffer, const uint_fast32_t size);
void rkWriteFile(const RkFile * const restrict file, const void * const buffer, const uint_fast32_t size);
void rkCloseFile(const RkFile * const restrict file);

RkResult rkGetFileSizeByPath(const char * const path, uint_fast32_t * const restrict size);
uint_fast32_t rkReadFileSimple(const char * const path, void * const buffer, const uint_fast32_t size);
void rkCreateFileSimple(const char * const path, const void * const buffer, const uint_fast32_t size,
						const RkFilePermissionFlags permissions);
