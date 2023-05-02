#pragma once

#define FUSE_USE_VERSION 35

#include <condition_variable>
#include <mutex>
#include <sstream>
#include <vector>
#include <string.h>

#include <fuse.h>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#define RESTFS_NAME "restfs"

namespace restfs
{
    // Hacky type alias to avoid typing this
    // monstrosity over and over again.
    typedef std::shared_ptr<RestClient::Connection> sharedConnection;

    enum RESTfulFileType {
        FT_NONE,
        FT_ROOT,
        FT_FILE,
    };

    // Type aliases for frequently used
    // dependencies.
    enum RESTfulProtocol {
        P_HTTP,
        P_HTTPS,
        P_FILE,
    };

    std::string protocol2string(RESTfulProtocol protocol);
    RESTfulFileType file_type(const std::string& path);

    /**
     * Pool of `RESTfulConnection` objects.
    */
    class RESTfulPool
    {
        public:
            sharedConnection aquire();
            void release(sharedConnection conn);

            size_t activeCount();
            size_t unusedCount();
            size_t lastUnused();

            RESTfulPool(
                const std::string& hostname,
                const std::string& username,
                const std::string& password,
                RESTfulProtocol protocol = P_HTTPS,
                size_t port = 0,
                size_t pool_size = 256);

            virtual ~RESTfulPool();

        protected:
            std::string getFullURL();

            /**
             * Creates a new `RestClient::Connection` object using
             * configuration defined by this object pool.
            */
            void createConnection(
                const std::string& username,
                const std::string& password);

            /**
             * Removes a connection from the object pool. Performs
             * teardown on `RestClient::Connection` objects.
            */
            void removeConnection(sharedConnection conn);

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

            std::mutex aquisition_mut;
            std::condition_variable aquisition_cv;
            std::string hostname;
            std::string protocol;
            size_t port;
            size_t actual_count;
            size_t unused_count;
            sharedConnection unused_stack[request_conf.POOL_SIZE];
    };

    int doRESTfulGetattr(const char* path, struct stat* stbuffer);
    int doRESTfulRead(
        const char* path,
        char* buffer,
        size_t size,
        off_t offset,
        struct fuse_file_info* fi);
    int doRESTfulReaddir(
            const char* path,
            void* buffer,
            fuse_fill_dir_t filler,
            off_t offset,
            struct fuse_file_info* fi);

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
