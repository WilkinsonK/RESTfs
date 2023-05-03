#include "restfs.hpp"

int restfs::doRESTfulGetattr(const char* path, struct stat* stbuffer)
{
    auto conn = restfs_pool->aquire();
    auto resp = conn->get(path);
    restfs_pool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    RESTfulFSObject fso;
    ar(fso);

    stbuffer->st_ino  = fso.info_ino;
    stbuffer->st_atim = (timespec)fso.info_atime;
    stbuffer->st_mtim = (timespec)fso.info_mtime;
    stbuffer->st_ctim = (timespec)fso.info_ctime;

    return 0;
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
    (void) size;

    auto conn = restfs_pool->aquire();
    auto resp = conn->get(path);
    restfs_pool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    RESTfulFSObject fso;
    ar(fso);

    if (fso.info_ft != FT_FILE)
        return -EINVAL;

    size = fso.content_length;
    if (size > RESTFS_MAX_CONTENT_LENGTH - offset)
        size = RESTFS_MAX_CONTENT_LENGTH - offset;

    memcpy(buffer, fso.content.c_str() + offset, size);
    return size;
}

int restfs::doRESTfulReaddir(
    const char* path,
    void* buffer,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info* fi)
{
    DIR* dp;

    (void) fi;

    auto conn = restfs_pool->aquire();
    auto resp = conn->get(path);
    restfs_pool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    RESTfulFSObject fso;
    ar(fso);

    if (fso.info_ft != FT_FDIR)
        return -EINVAL;

    for (auto node : fso.nodes)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = node.info_ino;
        st.st_mode = node.info_dtype << 12;
        if (filler(buffer, node.info_dname.c_str(), &st, offset))
            break;
    }

    return 0;
}
