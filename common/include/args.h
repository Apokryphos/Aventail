#ifndef ARGS_HEADER_INCLUDED
#define ARGS_HEADER_INCLUDED

int ArgCount();
int ArgExists(const char *arg);
//  Returns index of specified argument or zero if non-existant
int CheckArg(int index);
const char* GetArg(int index);
void InitArgs(int argc, char** argv);

#endif