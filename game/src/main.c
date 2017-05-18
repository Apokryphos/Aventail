#include "args.h"
#include "game.h"

//  ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    InitArgs(argc, argv);
    GameMain();
}