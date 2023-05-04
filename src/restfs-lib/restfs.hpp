#pragma once

#define FUSE_USE_VERSION 35

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <vector>
#include <string.h>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>
#include <fuse.h>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#define RESTFS_NAME "restfs"

// Maxing max content lenght at 2GB. This may
// result in parsing issues where we need to
// separate incoming data in chunks.
// It is suspected some files will be much
// larger than this limit.
#define RESTFS_MAX_CONTENT_LENGTH (size_t)(1000 * 1000 * 1000 * 2);
#define RESTFS_MAX_PATH_LENGTH (size_t)256

namespace restfs
{
    // Hacky type alias to avoid typing this
    // monstrosity over and over again.
    typedef std::shared_ptr<RestClient::Connection> sharedConnection;

    enum RESTfulFileType {
        FT_NONE,
        FT_ROOT,
        FT_FILE,
        FT_FDIR
    };

    // Type aliases for frequently used
    // dependencies.
    enum RESTfulProtocol {
        P_HTTP,
        P_HTTPS,
        P_FILE,
    };

    /** Represents some object in the hosts FS. */
    struct RESTfulFSObject
    {
        RESTfulFileType   info_ft;
        std::string       info_path;
        uint              info_s_atime; // (a,m & c) time in seconds
        uint              info_s_mtime;
        uint              info_s_ctime;
        unsigned long int info_ino;
        uint              info_uid;
        int               info_dtype;
        std::string       info_dname;

        size_t      content_length;
        std::string content;

        std::vector<RESTfulFSObject> nodes;

        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                info_ft,
                info_path,
                info_s_atime,
                info_s_mtime,
                info_s_ctime,
                info_ino,
                info_uid,
                info_dtype,
                info_dname,
                content_length,
                content,
                nodes);
        }
    };

    /** 
     * Correlates the protocol enumeration to the
     * string equivalent.
    */
    std::string protocol2string(RESTfulProtocol);
    int restfs_main(int argc, const char* const* argv);

    /**
     * Pool of `RESTfulConnection` objects.
    */
    class RESTfulPool
    {
        public:
            sharedConnection aquire();
            void release(sharedConnection);

            size_t activeCount();
            size_t unusedCount();
            size_t lastUnused();

            RESTfulPool(
                const std::string&,
                const std::string&,
                const std::string&,
                RESTfulProtocol = P_HTTPS,
                size_t = 0,
                size_t = 256);

            virtual ~RESTfulPool();

        protected:
            /**
             * Returns the base URL used for making RESTful calls.
             * This includes protocol, host name, and port number if
             * provided.
            */
            std::string getFullURL();

            /**
             * Creates a new `RestClient::Connection` object using
             * configuration defined by this object pool.
            */
            void createConnection(
                const std::string&,
                const std::string&);

            /**
             * Removes a connection from the object pool. Performs
             * teardown on `RestClient::Connection` objects.
            */
            void removeConnection(sharedConnection);

        private:
            /* 
             * Configuration constants used on `RestClient::Connection`
             * objects.
            */
            static struct request_conf
            {
                inline static const bool   FOLLOW_REDIRECTS = false;
                inline static const char   HEADER_ACCEPT[17] = "application/json";
                inline static const uint   REQUEST_TTL = 15;
                inline static const char   USER_AGENT[14] = "restfs-v0.0.0";
                inline static const size_t POOL_SIZE = 256;
            } request_conf;

            /**
             * Number of active pools. Used as a reference count to
             * determine control of `RestClient` startup and
             * shutdown.
            */
            inline static uint active_pool_count = 0;

            std::mutex aquisition_mut;
            std::condition_variable aquisition_cv;
            std::string hostname;
            std::string protocol;
            size_t port;
            size_t actual_count;
            size_t unused_count;
            sharedConnection unused_stack[request_conf.POOL_SIZE];
    };

    int doRESTfulGetattr(const char*, struct stat*);
    int doRESTfulRead(
        const char*,
        char*,
        size_t,
        off_t,
        struct fuse_file_info*);
    int doRESTfulReaddir(
            const char*,
            void*,
            fuse_fill_dir_t,
            off_t,
            struct fuse_file_info*);

    static int restGetattr(const char* path, struct stat* stbuffer)
    {
        return doRESTfulGetattr(path, stbuffer);
    }

    static int restRead(
        const char* path,
        char* buffer,
        size_t size,
        off_t offset,
        struct fuse_file_info* fi)
    {
        return doRESTfulRead(path, buffer, size, offset, fi);
    }

    static int restReaddir(
        const char* path,
        void* buffer,
        fuse_fill_dir_t filler,
        off_t offset,
        struct fuse_file_info* fi)
    {
        return doRESTfulReaddir(path, buffer, filler, offset, fi);
    }

    static std::shared_ptr<RESTfulPool> restfs_pool;
    static const struct fuse_operations restfs_operations =
    {
        .getattr = restGetattr,
        .read    = restRead,
        .readdir = restReaddir,
    };
} // namespace restfs
