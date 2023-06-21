#include "restfs.hpp"

std::string restfs::RESTfulPool::getFullURL()
{
    std::stringstream buffer;

    buffer << this->protocol << "://" << this->hostname;
    if (this->port > 0)
        buffer << ":" << this->port;
    buffer << "/" RESTFS_NAME "/";
    return buffer.str();
}

restfs::sharedConnection restfs::RESTfulPool::aquire()
{
    // This assumes that when there are no
    // *available* connections to hold off until
    // another is released.
    std::unique_lock<std::mutex> lock(this->aquisition_mut);
    this->aquisition_cv.wait(lock, [&]{ return this->unusedCount() > 0; });

    auto conn = this->unused_stack[this->lastUnused()];
    this->unused_stack[this->lastUnused()] = nullptr;
    this->unused_count--;

    return conn;
}

void restfs::RESTfulPool::release(restfs::sharedConnection conn)
{
    this->unused_count++;
    this->unused_stack[this->lastUnused()] = conn;
}

void restfs::RESTfulPool::createConnection(
    const std::string& username,
    const std::string& password)
{
    sharedConnection conn(new RestClient::Connection(this->getFullURL()));
    conn->SetBasicAuth(username, password);
    conn->SetUserAgent(this->request_conf.USER_AGENT);
    conn->SetTimeout(this->request_conf.REQUEST_TTL);
    conn->FollowRedirects(this->request_conf.FOLLOW_REDIRECTS);
    conn->AppendHeader("Accept", this->request_conf.HEADER_ACCEPT);

    this->unused_count++;
    this->unused_stack[this->lastUnused()] = conn;
}

size_t restfs::RESTfulPool::activeCount()
{
    return this->actual_count - this->unused_count;
}

size_t restfs::RESTfulPool::unusedCount()
{
    return this->unused_count;
}

size_t restfs::RESTfulPool::lastUnused()
{
    return this->unused_count - 1;
}

restfs::RESTfulPool::RESTfulPool(
    const std::string& hostname,
    const std::string& username,
    const std::string& password,
    RESTfulProtocol protocol,
    size_t port,
    size_t pool_size)
{
    if (!this->active_pool_count++)
    {
        RestClient::init();
    }

    this->hostname = hostname;
    this->port = port;
    this->protocol = protocol2string(protocol);

    this->actual_count = std::min(this->request_conf.POOL_SIZE, pool_size);
    this->unused_count = 0;
    while (this->unused_count < this->actual_count)
    {
        this->createConnection(username, password);
    }
}

restfs::RESTfulPool::~RESTfulPool()
{
    if (!this->active_pool_count--)
    {
        RestClient::disable();
    }
}
