#include <strings.h>

int	ArgC;
char** ArgV;

//  ---------------------------------------------------------------------------
int ArgCount()
{
    return ArgC - 1;
}

//  ---------------------------------------------------------------------------
int ArgExists(int index)
{
    return index > 0 && index < ArgC;
}

//  ---------------------------------------------------------------------------
int CheckArg(const char *arg)
{
    for (int a = 1; a < ArgC; a++)
    {
	    if (strcasecmp(arg, ArgV[a]) == 0)
        {
	        return a;
        }
    }
    return 0;
}

//  ---------------------------------------------------------------------------
const char* GetArg(int index)
{
    return ArgV[index];
}

//  ---------------------------------------------------------------------------
void InitArgs(int argc, char** argv)
{
    ArgC = argc;
    ArgV = argv;
}