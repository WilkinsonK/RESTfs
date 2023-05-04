#include <iostream>
#include <filesystem>
#include <unistd.h>

#include "restfs-lib/restfs.hpp"

using namespace std;

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

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "restfs: expected at lease one positional argument."
            << std::endl;
        std::exit(-1);
    }

    auto hostname_username = parse_hostname_user(argv[1]);
    auto password = getpass("password: ");
    restfs::restfs_pool = std::make_shared<restfs::RESTfulPool>
    (
        hostname_username.at(0),
        hostname_username.at(1),
        password,
        restfs::P_HTTPS
    );

    // Shift argv values to the left by 1.
    argc--;
    for (size_t i = 1; i < argc; i++)
        argv[i] = argv[i+1];
    return restfs::restfs_main(argc, argv);
}
