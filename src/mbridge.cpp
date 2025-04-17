#include <iostream>
#include <csignal>
#include <vector>

#include "mbBuilder.h"
#include "mbProject.h"
#include "mbServer.h"
#include "mbCommand.h"

volatile bool fRun = true;

struct Options
{
    mb::String file{"mbridge.conf"};
};

Options options;

void parseOptions(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        char *opt = argv[i];
        if (!strcmp(opt, "--version") || !strcmp(opt, "-v"))
        {
            puts("mpbridge : " MBRIDGE_VERSION_STR "\n"
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
    }
}

void signal_handler(int /*signal*/)
{
    fRun = false;
}

int main(int argc, char **argv)
{
    std::cout << "mbridge starts ..." << std::endl;
    parseOptions(argc, argv);
    mbProject *project;
    {
        mbBuilder builder;
        project = builder.load(options.file);
        if (builder.hasError())
        {
            std::cerr << "Error: " << builder.lastError() << std::endl;
            return 1;
        }
        if (project == nullptr)
        {
            std::cerr << "Error: Project is null" << std::endl;
            return 1;
        }
    }
    const mb::List<mbServer*> &servers = project->servers();
    const mb::List<mbCommand*> &commands = project->commands();
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
    std::cout << "mbridge stopped" << std::endl;
}
