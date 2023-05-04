#include "restfs.hpp"

std::string restfs::protocol2string(RESTfulProtocol protocol)
{
    switch (protocol)
    {
        case P_HTTP:
            return "http";
        case P_HTTPS:
            return "https";
        case P_FILE:
            return "file";
        default:
            return "http";
    }
}

int restfs::restfs_main(int argc, const char* const* argv)
{
    return fuse_main(argc, (char**)argv, &restfs_operations, NULL);
}
