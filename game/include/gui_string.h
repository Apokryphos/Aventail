#ifndef GUI_STRING
#define GUI_STRING

/*
    Compares a GUI string with another string to check if the GUI string needs
    to be updated to the new value.
    Returns non-zero on match or zero on mismatch.
*/
int gui_string_compare(const char* guiString, const char* value);

/*
    Compares a GUI string with the specified string using gui_string_compare
    and sets the GUI string to match, allocating and/or freeing the GUI string
    as needed.
*/
void set_gui_string(char **string, const char* value);

#endif