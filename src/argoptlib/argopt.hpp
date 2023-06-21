#pragma once

#include <iostream>
#include <string>
#include <getopt.h>

namespace argopt
{
    struct varargs
    {
        public:
            int flag_help;

            char* optn_password;

        protected:
            friend void parse(varargs&, int argc, char* const* argv);
    };

    static char sopts[] = "h::";
    static struct option lopts[3];

    void parse(varargs& vargs, int argc, char* const* argv)
    {
        lopts[0] = {"verbose",  no_argument,       0,                  0};
        lopts[1] = {"help",     no_argument,       &(vargs.flag_help), 0};
        lopts[2] = {"password", required_argument, 0,                  0};

        int c, optidx;

        auto handle_lopt = [](int optidx)
        {
            std::cout << lopts[optidx].name << ": '";
            std::cout << optarg << "'" << std::endl;
        };

        while (
            (c = getopt_long
                (
                    argc,
                    argv,
                    sopts,
                    lopts,
                    &optidx
                )) != -1)
        {
            switch(c)
            {
                // parsed is a long option.
                case 0:
                    handle_lopt(optidx);
                    continue;
                case 'h':
                    vargs.flag_help = 1;
                    continue;
                case '?':
                    std::cerr << argv[0] << ": got unknown option."
                        << std::endl;
                    std::exit(-1);
                default:
                    std::cerr << argv[0] << ": unknown parse returns: '" << (char)c << "'."
                        << std::endl;
                    std::exit(-1);
            }
        }

    };
} // namespace argopt