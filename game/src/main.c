#include "args.h"
#include "game.h"

//  ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    init_args(argc, argv);
    GameMain();
}