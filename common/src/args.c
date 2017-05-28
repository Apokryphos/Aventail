#include <strings.h>

int	ArgC;
char** ArgV;

//  ---------------------------------------------------------------------------
int get_arg_count()
{
    return ArgC - 1;
}

//  ---------------------------------------------------------------------------
int arg_exists(int index)
{
    return index > 0 && index < ArgC;
}

//  ---------------------------------------------------------------------------
int check_arg(const char *arg)
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
const char* get_arg_by_index(int index)
{
    return ArgV[index];
}

//  ---------------------------------------------------------------------------
void init_args(int argc, char** argv)
{
    ArgC = argc;
    ArgV = argv;
}