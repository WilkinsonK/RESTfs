#include "restfs.hpp"

int restfs::doRESTfulGetattr(const char* path, struct stat* stbuffer)
{
    // TODO: implement this callback.
    return -1;
}

int restfs::doRESTfulRead(
    const char* path,
    char* buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info* fi)
{
    // void out unused values.
    (void) fi;
    (void) offset;

    auto conn = restfs_pool->aquire();
    auto resp = conn->get(path);
    restfs_pool->release(conn);

    if (resp.code == 404)
        return -ENOENT;
    if (restfs::file_type(path) != FT_FILE)
        return -EINVAL;

    memcpy(buffer, resp.body.c_str(), size);
    return sizeof(resp.body);
}

int restfs::doRESTfulReaddir(
    const char* path,
    void* buffer,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info* fi)
{
    // TODO: implement this callback.
    return -1;
}
