#include <filesystem.h>
#include <sys/stat.h>
#include <asserts.h>
#include <unistd.h>

RkResult rkOpenFile(RkFile * const restrict file, const char * const path,
					const RkFileAccessFlags access, const RkFilePermissionFlags permissions)
{
	const int fd = open(path, access, permissions);

	if (fd < 0) {
		return RK_RESULT_FILE_OPEN;
	}

	file->debugPath = path;
	file->fd = fd;

	return RK_RESULT_SUCCESS;
}

uint_fast32_t rkReadFile(const RkFile * const restrict file, void * const buffer, const uint_fast32_t size) {
	const int_fast32_t result = read(file->fd, buffer, size);
	RK_ASSERT(result < 0, "Read file %s!", file->debugPath);
	return result;
}

void rkWriteFile(const RkFile * const restrict file, const void * const buffer, const uint_fast32_t size) {
	const uint_fast32_t result = write(file->fd, buffer, size);
	RK_ASSERT(result != size, "Write file %s! (size=%u, result=%d)", file->debugPath, size, (int)result);
}

void rkCloseFile(const RkFile * const restrict file) {
	const int result = close(file->fd);
	RK_ASSERT(result, "Close file %s!", file->debugPath);
}

RkResult rkGetFileSizeByPath(const char * const path, uint_fast32_t * const restrict size) {
	struct stat info;
	const int result = stat(path, &info);

	if (result != RK_RESULT_SUCCESS) {
		return RK_RESULT_FILE_STAT;
	}

	*size = info.st_size;
	return RK_RESULT_SUCCESS;
}

uint_fast32_t rkReadFileSimple(const char * const path, void * const buffer, const uint_fast32_t size) {
	RkFile file;

	const RkResult result = rkOpenFile(&file, path, RK_FILE_ACCESS_READ_ONLY, RK_FILE_PERMISSION_NONE);
	RK_ASSERT(result, "Open file %s!", path);

	const uint_fast32_t readSize = rkReadFile(&file, buffer, size);
	rkCloseFile(&file);

	return readSize;
}

void rkCreateFileSimple(const char * const path, const void * const buffer, const uint_fast32_t size,
						const RkFilePermissionFlags permissions)
{
	RkFile file;
	const RkFileAccessFlags flags = RK_FILE_ACCESS_CREATE |
		RK_FILE_ACCESS_TRUNCATE | RK_FILE_ACCESS_WRITE_ONLY;
	const RkResult result = rkOpenFile(&file, path, flags, permissions);
	RK_ASSERT(result, "Open file %s!", path);

	rkWriteFile(&file, buffer, size);
	rkCloseFile(&file);
}
