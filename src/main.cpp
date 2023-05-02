#include <iostream>

#include "restfs-lib/restfs.hpp"

using namespace std;

int main(int argc, const char *const *argv)
{
    restfs::RESTfulPool pool(
        "<hostname>",
        "<username>",
        "<password>");

    auto conn1 = pool.aquire();
    std::cout << "connection @" << conn1 << " & connections left: " << std::to_string(pool.unusedCount()) << std::endl;
    pool.release(conn1);

    auto conn2 = pool.aquire();
    std::cout << "connection @" << conn2 << " & connections left: " << std::to_string(pool.unusedCount()) << std::endl;

    conn2->AppendHeader("Accept", "text");
    auto r = conn2->get("data/JSESSION");
    std::cout << "Response:"
        << "\t\nstatus_code: " << std::to_string(r.code)
        << "\t\nfrom body:   " << r.body << std::endl;
    conn2->AppendHeader("Accept", "application/json");

    pool.release(conn2);
    std::cout << "connections left: " << std::to_string(pool.unusedCount()) << std::endl;
}
