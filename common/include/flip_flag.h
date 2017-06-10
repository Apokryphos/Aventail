#ifndef FLIP_FLAGS_HEADER_INCLUDED
#define FLIP_FLAGS_HEADER_INCLUDED

enum FlipFlag
{
    FLIP_FLAG_NONE = 0,
    FLIP_FLAG_HORZ = (1 << 0),
    FLIP_FLAG_VERT = (1 << 1),
    FLIP_FLAG_ROTATE_RIGHT = (1 << 2),
    FLIP_FLAG_ROTATE_LEFT = (1 << 3),
};

#endif