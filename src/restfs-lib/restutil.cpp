#include "restfs.hpp"

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
