#include "CommandExecuter.h"
#include "Server_Main.h"
#include "Logger.h"
#include "Utilities.h"
#include "Server_Main.h"

#include "tracy/Tracy.hpp"

CommandExecuter::CommandExecuter()
{
    running = false;
    _inputStr_size = 0;
    
    //c_thread = NULL;

}

void CommandExecuter::Run(bool multiThread)
{
    running = true;
    _inputStr_size = 0;
    if (multiThread)
        c_thread = std::thread(&CommandExecuter::Loop, this);
}

void CommandExecuter::Loop()
{
    char c, i = 0;

    do
    {
        Process();
        Server_Main::Sleep(1000); // micro seconds (1ms)
    } while (running);

}

void CommandExecuter::Process()
{
    if (!running)
        return;

    ZoneScopedN("Server CommandExecuter");

    char c;

    if (!kbhit())
        return;

    c = getche();

    if (c == '\n' || c == '\r')
    {
        printf("\n");
        //std::string output = Logger::InputStr();
        //Logger::InputStr("", 0);
        
        //Logger::ResetInput(); // TODO


        Server_Main::Instance()->SetCommand(GetCurrentInput());
        memset(_inputStr, 0, INPUT_MAX);
        _inputStr_size = 0;
        Server_Main::Instance()->SetCurrentCommand(GetCurrentInput());
    }
    else
    {
        //printf("%c", c);
        _inputStr[_inputStr_size] = c;
        _inputStr_size++;
        Server_Main::Instance()->SetCurrentCommand(GetCurrentInput());
        //Logger::InputStr(_inputStr, i + 1);
    }
}

void CommandExecuter::Stop()
{
    running = false;

}
