/*
    pmbridge
    
    Created: 2025    
    Author: Serhii Marchuk, https://github.com/serhmarch
    
    Copyright (C) 2025  Serhii Marchuk

    Distributed under the MIT License (http://opensource.org/licenses/MIT)
    
*/
#include <iostream>
#include <csignal>
#include <cstring>
#include <vector>

#include <pmb_log.h>

#include <project/pmbBuilder.h>
#include <project/pmbProject.h>
#include <project/pmbServer.h>
#include <project/pmbCommand.h>

const char* help(int argc, char** argv);

volatile bool fRun = true;

struct Options
{
    pmb::String   file      {"pmbridge.conf"};
    pmb::LogFlags log_flags {static_cast<pmb::LogFlags>(pmb::Log_All)};
    pmb::String   log_format{"[%time] %text"};
    pmb::String   log_time  {"%Y-%M-%D %h:%m:%s.%f"};
};

Options options;

void parseOptions(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        char *opt = argv[i];
        if (!strcmp(opt, "--version") || !strcmp(opt, "-v"))
        {
            puts("pmbridge : " PMBRIDGE_VERSION_STR "\n"
                 "ModbusLib: " MODBUSLIB_VERSION_STR);
            exit(0);
        }
        if (!strcmp(opt, "--help") || !strcmp(opt, "-?"))
        {
            std::cout << (help(argc-i-1, &argv[i+1])) << std::endl;
            exit(0);
        }
        if (!std::strcmp(opt, "--file") || !std::strcmp(opt, "-f"))
        {
            if (++i >= argc)
            {
                //printHelp();
                std::exit(1);
            }
            options.file = argv[i];
            continue;
        }
        if (!std::strcmp(opt, "--log-flags") || !std::strcmp(opt, "-lc"))
        {
            if (++i >= argc)
            {
                //printHelp();
                std::exit(1);
            }
            options.log_flags = 0;
            options.log_flags = pmb::toLogFlags(pmb::String(argv[i]));
            continue;
        }
        if (!std::strcmp(opt, "--log-time") || !std::strcmp(opt, "-lt"))
        {
            if (++i >= argc)
            {
                //printHelp();
                std::exit(1);
            }
            options.log_time = argv[i];
            continue;
        }
        if (!std::strcmp(opt, "--log-format") || !std::strcmp(opt, "-lf"))
        {
            if (++i >= argc)
            {
                //printHelp();
                std::exit(1);
            }
            options.log_format = argv[i];
            continue;
        }
    }
}

void signal_handler(int /*signal*/)
{
    fRun = false;
}

int main(int argc, char **argv)
{
    parseOptions(argc, argv);
    pmb::setLogFlags(options.log_flags);
    pmb::setLogFormat(options.log_format);
    pmb::setLogTimeFormat(options.log_time);
    std::cout << "pmbridge starts ..." << std::endl;
    pmbProject *project;
    {
        pmbBuilder builder;
        project = builder.load(options.file);
        if (builder.hasError())
        {
            pmbLogError("Error processing '%s': %s", options.file.data(), builder.lastError().data());
            return 1;
        }
        if (project == nullptr)
        {
            pmbLogError("Project is null");
            return 1;
        }
    }
    const pmb::List<pmbServer*> &servers = project->servers();
    const pmb::List<pmbCommand*> &commands = project->commands();
    if(std::signal(SIGINT, signal_handler) == SIG_ERR)
        pmbLogWarning("Unable to set SIGINT handler");
    if(std::signal(SIGTERM, signal_handler) == SIG_ERR)
        pmbLogWarning("Unable to set SIGTERM handler");
    auto serverit = servers.begin();
    auto cmdit = commands.begin();
    while (fRun)
    {
        if (cmdit != commands.end())
        {
            bool res = (*cmdit)->run();
            if (res)
                ++cmdit;
            if (cmdit == commands.end())
                cmdit = commands.begin();
        }
        for (auto server : servers)
            server->run();
        Modbus::msleep(1);
    }
    delete project;
    std::cout << "pmbridge stopped" << std::endl;
}
