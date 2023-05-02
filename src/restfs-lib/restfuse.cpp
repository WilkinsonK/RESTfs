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

// TODO: This is primitive and needs to be better
// defined.
restfs::RESTfulFileType restfs::file_type(const std::string& path)
{
    if (strcmp(path.c_str(), "/") == 0)
        return FT_ROOT;
    if (strcmp(path.c_str(), "/" restfs_NAME) == 0)
        return FT_FILE;
    return FT_NONE;
}
