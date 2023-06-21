#define FUSE_USE_VERSION 35

#include <iostream>
#include <filesystem>
#include <unistd.h>

#include <cereal/cereal.hpp>
#include <fuse.h>

#include "argoptlib/argopt.hpp"
#include "restfslib/restfs.hpp"

static std::shared_ptr<restfs::RESTfulPool> restPool;

static int restGetattr(const char* path, struct stat* stbuffer)
{
    auto conn = restPool->aquire();
    auto resp = conn->get(path);
    restPool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    restfs::RESTfulFSObject fso;
    ar(fso);

    stbuffer->st_ino = fso.info_ino;
    stbuffer->st_uid = fso.info_uid;
    stbuffer->st_atim.tv_sec = fso.info_s_atime;
    stbuffer->st_mtim.tv_sec = fso.info_s_mtime;
    stbuffer->st_ctim.tv_sec = fso.info_s_ctime;

    return 0;
}

static int restRead(
    const char* path,
    char* buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info* fi)
{
    // void out unused values.
    (void) fi;
    (void) size;

    auto conn = restPool->aquire();
    auto resp = conn->get(path);
    restPool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    restfs::RESTfulFSObject fso;
    ar(fso);

    if (fso.info_ft != restfs::FT_FILE)
        return -EINVAL;

    size = fso.content_length;
    if (size > RESTFS_MAX_CONTENT_LENGTH - offset)
        size = RESTFS_MAX_CONTENT_LENGTH - offset;

    memcpy(buffer, fso.content.c_str() + offset, size);
    return size;
}

static int restReaddir(
    const char* path,
    void* buffer,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info* fi)
{
    (void) fi;

    auto conn = restPool->aquire();
    auto resp = conn->get(path);
    restPool->release(conn);

    if (resp.code == 401)
        return -EACCES;
    if (resp.code == 404)
        return -ENOENT;

    std::stringstream ss(resp.body);
    cereal::JSONOutputArchive ar(ss);

    restfs::RESTfulFSObject fso;
    ar(fso);

    if (fso.info_ft != restfs::FT_FDIR)
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


static const struct fuse_operations restOperations
{
    .getattr = restGetattr,
    .read    = restRead,
    .readdir = restReaddir,
};

static int restMain(
    int argc,
    const char* const* argv,
    std::string& username,
    std::string& hostname,
    std::string& password)
{
    restPool = std::make_shared<restfs::RESTfulPool>
    (
        username,
        hostname,
        password,
        restfs::P_HTTPS
    );

    return fuse_main(argc, (char**)argv, &restOperations, NULL);
};

std::vector<std::string> split(
    const std::string& value,
    const char splitchar = ' ')
{
    std::stringstream victim(value);
    std::string buffer;
    std::vector<std::string> segments;

    if (value.empty())
        return segments;

    while (std::getline(victim, buffer, splitchar))
        segments.push_back(buffer);

    return segments;
}

std::vector<std::string> parse_hostname_user(const std::string& value)
{
    std::vector<std::string> parsed = split(value, ':');
    if (parsed.size() != 2 || parsed.at(0).empty() || parsed.at(1).empty())
    {
        std::cerr << "restfs: expected first argument as <hostname>:<user>"
            << std::endl;
        std::exit(-1);
    }
    return parsed;
}

int main(int argc, const char** argv)
{
    argopt::varargs vargs;
    argopt::parse(vargs, argc, (char**)argv);

    std::vector<std::string> parsedHU;
    std::string password;

    if (argc < 2)
    {
        std::cerr << "restfs: expected at least one positional argument."
            << std::endl;
        std::exit(-1);
    }

    // parsedHU = parse_hostname_user((std::string)args[1]);
    // // password = (std::string)getpass("password: ");

    // // Shift argv values to the left by 1.
    // for (size_t i = 1; i < (args.size() - 1); i++)
    // {
    //     args[i] = args[i+1];
    // }
    // args.resize(args.size() - 1);
}
