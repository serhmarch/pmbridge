#include <iostream>
#include <csignal>
#include <vector>

#include "mbBuilder.h"
#include "mbProject.h"
#include "mbServer.h"
#include "mbCommand.h"

volatile bool fRun = true;

void signal_handler(int /*signal*/)
{
    fRun = false;
}

int main(int argc, char **argv)
{
    std::cout << "mbridge starts ..." << std::endl;
    mbProject *project;
    {
        mbBuilder builder;
        project = builder.load("mbridge.conf");
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
