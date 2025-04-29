#include <iostream>
#include <csignal>
#include <vector>

#include "pmb_log.h"
#include "pmbBuilder.h"
#include "pmbProject.h"
#include "pmbServer.h"
#include "pmbCommand.h"

volatile bool fRun = true;

struct Options
{
    pmb::String   file      {"pmbridge.conf"};
    pmb::LogFlags log_flags {static_cast<pmb::LogFlags>(pmb::Log_All)};
    pmb::String   log_format{"[%time] %cat: %text"};
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
            //puts(help_options);
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
        if (!std::strcmp(opt, "--log-flags"))
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
        if (!std::strcmp(opt, "--log-time"))
        {
            if (++i >= argc)
            {
                //printHelp();
                std::exit(1);
            }
            options.log_time = argv[i];
            continue;
        }
        if (!std::strcmp(opt, "--log-format"))
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
    std::cout << "pmbridge starts ..." << std::endl;
    parseOptions(argc, argv);
    pmb::setLogFlags(options.log_flags);
    pmb::setLogFormat(options.log_format);
    pmb::setLogTimeFormat(options.log_time);
    pmbProject *project;
    {
        pmbBuilder builder;
        project = builder.load(options.file);
        if (builder.hasError())
        {
            mbLogError("%s", builder.lastError().data());
            return 1;
        }
        if (project == nullptr)
        {
            mbLogError("Project is null");
            return 1;
        }
    }
    const pmb::List<pmbServer*> &servers = project->servers();
    const pmb::List<pmbCommand*> &commands = project->commands();
    auto cmdit = commands.begin();
    std::signal(SIGINT, signal_handler);
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
