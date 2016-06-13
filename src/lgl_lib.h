#include "uthash.h"
/*
Name       | ID
Expression = 0
String     = 1
Both       = 2
Boolean    = 3
Menu       = 4
Sprite     = 5
Sound      = 6
Background = 7
Path       = 8
Script     = 9
Object     = 10
Room       = 11
Font       = 12
Color      = 13
Timeline   = 14
Font String= 15
*/
struct lgl_argument_meta
{
  unsigned char *caption; //Pointer for a malloc request for memory for a string
  int kind;
  unsigned char *deflt;
  unsigned char *menu;
};

/*Action Kinds:
0 	Normal
1 	Begin Group
2 	End Group
3 	Else
4 	Exit
5 	Repeat
6 	Variable
7 	Code
8 	-Placeholder
9 	-Separator
10 	-Label
*/
struct lgl_action_meta
{
  //This is the informal name shown to the user
  unsigned char *name; //Pointer for a malloc request for memory for a string
  int libid;
  int id;
  int kind;
  int userelative;
  int useapplyto;
  int isquestion;
  int exetype;
  int interface; //0=Normal, 1=None, 2=Arrows, 5=Code, 6=Text
  unsigned char *functionname; //Pointer for a malloc request for memory for a string
  unsigned char *codestring; //Pointer for a malloc request for memory for a string
  //I won't implement these because they're useless for my use case, but in case
  //you want to:
  //char *desc; //Pointer for a malloc request for memory for a string
  //char *hint_str; //Pointer for a malloc request for memory for a string
  //char *list_str; //Pointer for a malloc request for memory for a string
  int arg_count;
  struct lgl_argument_meta **arguments;
  UT_hash_handle hh; //The hash value for the struct, so that we can access it
                      //quickly, by id
};
/*
struct lgl_lib_meta
{
  int libid;
  char *caption;
  char *author;
  char *ver;
  double last_changed;
  char *info;
  char *init_code;
  UT_hash_handle hh;
};*/
int lgl_action_count;
struct lgl_action_meta *action_meta_dict;
int load_lgl(char *);
int load_lgl_dir(char *);
struct lgl_action_meta *get_action_meta(int);
