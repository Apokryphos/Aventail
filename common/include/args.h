#ifndef ARGS_HEADER_INCLUDED
#define ARGS_HEADER_INCLUDED

int get_arg_count();
//int arg_exists(const char *arg);
//  Returns index of specified argument or zero if non-existant
//int check_arg(int index);
const char* get_arg_by_index(int index);
void init_args(int argc, char** argv);

#endif