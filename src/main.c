#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <locale.h>
#include <ncurses.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <time.h> //For seeding rand()
#include "gm_obj_lib.h"
#include "lgl_lib.h"
#include "settings.h"
#define true 1
#define false 0
#define min(a,b) (a)>(b)?(b):(a)

#define max(a,b) (a)<(b)?(b):(a)

//Globals
int terminal_width=80;
int terminal_height=44;
char *event_type_name[11];
char *other_event_type_name[71]; //For the events under 'other' aka eventtype 7
char *step_event_type_name[3];
char *draw_event_type_name[3];
char *alarm_event_type_name[12];
char *event_enumb_name[11];
char *mouse_button_names[63];
char *keys_names[124];
//Converts the GMX draw event enumb to their index and back because GMX did some stupid stuff
int event_draw_index_to_enumb_array[3]={0,64,65};
#define event_draw_index_to_enumb(a) event_draw_index_to_enumb_array[a]
#define event_draw_enumb_to_index(a) (a==0)?a:(a-63)
//Counts all non-nulls in array
int count_names(char *array[],int size)
{
  int total=0;
  int i;
  for (i=0;i<size;i++)
  {
    if (array[i]!=NULL)
    {
      total++;
    }
  }
  return total;
}

enum event_kind
{
  event_create = 0,
  event_destroy = 1,
  event_alarm = 2,
  event_step = 3,
  event_collision = 4,
  event_key = 5,
  event_mouse = 6,
  event_other = 7,
  event_draw = 8,
  event_key_pressed = 9,
  event_key_released = 10,
};
//All the arg kinds
enum arg_kind
{
  arg_expression = 0,
  arg_string = 1,
  arg_both = 2,
  arg_boolean = 3,
  arg_menu = 4,
  arg_sprite = 5,
  arg_sound = 6,
  arg_background = 7,
  arg_path = 8,
  arg_script = 9,
  arg_object = 10,
  arg_room = 11,
  arg_font = 12,
  arg_color = 13,
  arg_timeline = 14,
  arg_font_str = 15,
};

//All the action kinds
enum action_kind
{
  action_normal = 0,
  action_group_start = 1,
  action_group_end = 2,
  action_else = 3,
  action_exit = 4,
  action_repeat = 5,
  action_var = 6,
  action_code = 7,
  action_placeholder = 8,
  action_seperator = 9,
  action_label = 10,
};

//To initialise all the names+icons we'll use
void init_names()
{
  //Informal type names
  event_type_name[event_create] =  "☀  Create";
  event_type_name[event_destroy] =  "♻  Destroy";
  event_type_name[event_alarm] =  "⌛  Alarm";
  event_type_name[event_step] =  "🚶  Step";
  event_type_name[event_collision] =  "↸  Collision";
  event_type_name[event_key] =  "⌨  Key";
  event_type_name[event_mouse] =  "┬⃣  Mouse";
  event_type_name[event_other] =  "…  Other";
  event_type_name[event_draw] =  "✏  Draw";
  event_type_name[event_key_pressed] =  "⌨  Pressed";
  event_type_name[event_key_released] = "⌨  Released";
  //Informal enumb names
  event_enumb_name[event_create] =  NULL;
  event_enumb_name[event_destroy] =  NULL;
  event_enumb_name[event_alarm] =  "Alarm number:"; //note: >11 unsupported by many implementations, put this in documentation
  event_enumb_name[event_step] =  "Step type:";
  event_enumb_name[event_collision] =  "Object:";
  event_enumb_name[event_key] =  "Key number:";
  event_enumb_name[event_mouse] =  "Mouse button:";
  event_enumb_name[event_other] =  "Other event:";
  event_enumb_name[event_draw] =  "Draw type:";
  event_enumb_name[event_key_pressed] =  "Key number:";
  event_enumb_name[event_key_released] = "Key number:";
  //Informal enumb names for other aka [7]
  other_event_type_name[0] = "Outside Room";
  other_event_type_name[1] = "Intersect Boundary";
  other_event_type_name[2] = "Game Start";
  other_event_type_name[3] = "Game End";
  other_event_type_name[4] = "Room Start";
  other_event_type_name[5] = "Room End";
  other_event_type_name[6] = "Out of lives";
  other_event_type_name[7] = "Animation End";
  other_event_type_name[8] = "End of Path";
  other_event_type_name[9] = "Out of Health";
  other_event_type_name[10] = "User Defined 0";
  other_event_type_name[11] = "User Defined 1";
  other_event_type_name[12] = "User Defined 2";
  other_event_type_name[13] = "User Defined 3";
  other_event_type_name[14] = "User Defined 4";
  other_event_type_name[15] = "User Defined 5";
  other_event_type_name[16] = "User Defined 6";
  other_event_type_name[17] = "User Defined 7";
  other_event_type_name[18] = "User Defined 8";
  other_event_type_name[19] = "User Defined 9";
  other_event_type_name[20] = "User Defined 10";
  other_event_type_name[21] = "User Defined 11";
  other_event_type_name[22] = "User Defined 12";
  other_event_type_name[23] = "User Defined 13";
  other_event_type_name[24] = "User Defined 14";
  other_event_type_name[25] = "User Defined 15";
  //They skip to 30 here
  other_event_type_name[30] = "Close Button";
  //Then to 40
  other_event_type_name[40] = "Outside View 0";
  other_event_type_name[41] = "Outside View 1";
  other_event_type_name[42] = "Outside View 2";
  other_event_type_name[43] = "Outside View 3";
  other_event_type_name[44] = "Outside View 4";
  other_event_type_name[46] = "Outside View 5";
  other_event_type_name[47] = "Outside View 6";
  other_event_type_name[48] = "Outside View 7";
  //Skips to 50 here
  other_event_type_name[50] = "Boundary View 0";
  other_event_type_name[51] = "Boundary View 1";
  other_event_type_name[52] = "Boundary View 2";
  other_event_type_name[53] = "Boundary View 3";
  other_event_type_name[54] = "Boundary View 4";
  other_event_type_name[55] = "Boundary View 5";
  other_event_type_name[56] = "Boundary View 6";
  other_event_type_name[57] = "Boundary View 7";
  //Skips to 60 here
  other_event_type_name[60] = "Image Loaded";
  other_event_type_name[61] = "Sound Loaded";
  other_event_type_name[62] = "HTTP";
  other_event_type_name[63] = "Dialog";
  //Skips, probably because yoyo decided to throw the new draw events in 64 and 65
  other_event_type_name[66] = "IAP";
  other_event_type_name[67] = "Cloud";
  other_event_type_name[68] = "Networking";
  other_event_type_name[69] = "Steam";
  other_event_type_name[70] = "Social";
  //Step event Types
  step_event_type_name[0] = "Step";
  step_event_type_name[1] = "Pre-step";
  step_event_type_name[2] = "Post-step";
  //Draw event Types
  draw_event_type_name[0] = "Draw";
  //Because we put them in the other_event_type_name array
  draw_event_type_name[1] = "GUI"; //In the GMX format this is enumb 64 of draw
  draw_event_type_name[2] = "Resize"; //In the GMX format this is enumb 65 of draw
  //Mouse event types
  mouse_button_names[0] = "Left";
  mouse_button_names[1] = "Right";
  mouse_button_names[2] = "Middle";
  mouse_button_names[3] = "None";
  mouse_button_names[4] = "Left Pressed";
  mouse_button_names[5] = "Right Pressed";
  mouse_button_names[6] = "Middle Pressed";
  mouse_button_names[7] = "Left Released";
  mouse_button_names[8] = "Right Released";
  mouse_button_names[9] = "Middle Released";
  mouse_button_names[10] = "Enters";
  mouse_button_names[11] = "Leaves";
  mouse_button_names[16] = "Gamepad 1 Left";
  mouse_button_names[17] = "Gamepad 1 Right";
  mouse_button_names[18] = "Gamepad 1 Up";
  mouse_button_names[19] = "Gamepad 1 Down";
  mouse_button_names[21] = "Gamepad 1 Button 1";
  mouse_button_names[22] = "Gamepad 1 Button 2";
  mouse_button_names[23] = "Gamepad 1 Button 3";
  mouse_button_names[24] = "Gamepad 1 Button 4";
  mouse_button_names[25] = "Gamepad 1 Button 5";
  mouse_button_names[26] = "Gamepad 1 Button 6";
  mouse_button_names[27] = "Gamepad 1 Button 7";
  mouse_button_names[28] = "Gamepad 1 Button 8";
  mouse_button_names[31] = "Gamepad 2 Left";
  mouse_button_names[32] = "Gamepad 2 Right";
  mouse_button_names[33] = "Gamepad 2 Up";
  mouse_button_names[34] = "Gamepad 2 Down";
  mouse_button_names[36] = "Gamepad 2 Button 1";
  mouse_button_names[37] = "Gamepad 2 Button 2";
  mouse_button_names[38] = "Gamepad 2 Button 3";
  mouse_button_names[39] = "Gamepad 2 Button 4";
  mouse_button_names[40] = "Gamepad 2 Button 5";
  mouse_button_names[41] = "Gamepad 2 Button 6";
  mouse_button_names[42] = "Gamepad 2 Button 7";
  mouse_button_names[43] = "Gamepad 2 Button 8";
  mouse_button_names[50] = "Global Left";
  mouse_button_names[51] = "Global Right";
  mouse_button_names[52] = "Global Middle";
  mouse_button_names[53] = "Global Left Pressed";
  mouse_button_names[54] = "Global Right Pressed";
  mouse_button_names[55] = "Global Middle Pressed";
  mouse_button_names[56] = "Global Left Released";
  mouse_button_names[57] = "Global Right Released";
  mouse_button_names[58] = "Global Middle Released";
  mouse_button_names[61] = "Wheel Up";
  mouse_button_names[62] = "Wheel Down";
  //Keys event types
  //https://github.com/IsmAvatar/LateralGM/blob/220dc7da6dc358bde0d691f52100c852283831a3/org/lateralgm/resources/sub/Event.java
  keys_names[0] = "No key";
  keys_names[1] = "Any key";
  keys_names[13] = "Enter";
  keys_names[16] = "Shift";
  keys_names[17] = "Control";
  keys_names[18] = "Alt";
  keys_names[32] = "Space";
  keys_names[37] = "Left";
  keys_names[38] = "Up";
  keys_names[39] = "Right";
  keys_names[40] = "Down";
  keys_names[48] = "0";
  keys_names[49] = "1";
  keys_names[50] = "2";
  keys_names[51] = "3";
  keys_names[52] = "4";
  keys_names[53] = "5";
  keys_names[54] = "6";
  keys_names[55] = "7";
  keys_names[56] = "8";
  keys_names[57] = "9";
  keys_names[65] = "A";
  keys_names[66] = "B";
  keys_names[67] = "C";
  keys_names[68] = "D";
  keys_names[69] = "E";
  keys_names[70] = "F";
  keys_names[71] = "G";
  keys_names[72] = "H";
  keys_names[73] = "I";
  keys_names[74] = "J";
  keys_names[75] = "K";
  keys_names[76] = "L";
  keys_names[77] = "M";
  keys_names[78] = "N";
  keys_names[79] = "O";
  keys_names[80] = "P";
  keys_names[81] = "Q";
  keys_names[82] = "R";
  keys_names[83] = "S";
  keys_names[84] = "T";
  keys_names[85] = "U";
  keys_names[86] = "V";
  keys_names[87] = "W";
  keys_names[88] = "X";
  keys_names[89] = "Y";
  keys_names[90] = "Z";
}
//Converts an argument kind to a textbox character entry limit
int arg_to_textbox_limit[16]={0,0,0,0,0,1,1,1,1,1,1,1,1,2,1,1};
//From: https://stackoverflow.com/questions/1306727/way-to-get-number-of-digits-in-an-int
int int_len(int n)
{
  if (n < 100000)
  {
        // 5 or less
        if (n < 100){
            // 1 or 2
            if (n < 10)
                return 1;
            else
                return 2;
        }
        else
        {
            // 3 or 4 or 5
            if (n < 1000)
            {
              return 3;
            }
            else
            {
                // 4 or 5
                if (n < 10000)
                {
                  return 4;
                }
                else
                {
                  return 5;
                }
            }
        }
    }
    else
    {
        // 6 or more
        if (n < 10000000)
        {
            // 6 or 7
            if (n < 1000000)
            {
                return 6;
            }
            else
            {
              return 7;
            }
        }
        else
        {
            // 8 to 10
            if (n < 100000000)
            {
              return 8;
            }
            else
            {
                // 9 or 10
                if (n < 1000000000)
                {
                  return 9;
                }
                else
                {
                  return 10;
                }
            }
        }
    }
}

void draw_bool(int y, int x, int val)
{
  //Thanks to the anon on 8chan in the asciipunk thread for coming up with this UI
  //It's much clearer than the checkboxes I was going to use
  if (val)
  {
    mvaddstr(y, x, "■");
  }
  else
  {
    mvaddstr(y, x, "□");
  }
}

//Returns if there was a change or not
int handle_menu_ctrls(int key,int *selection,int *scope,int menu_size,int height)
{
  if ((key==KEY_DOWN)|(key==KEY_RIGHT))
  {
    if (*selection<menu_size-1)
    {
      (*selection)++;
      if ((*selection-*scope>height)&(*scope<menu_size-height))
      {
        (*scope)++;
      }
    }
    else
    {
      *selection=0;
      *scope=0;
    }
  }
  else if ((key==KEY_UP)|(key==KEY_LEFT))
  {
    if (*selection>0)
    {
      (*selection)--;
      if (*selection<*scope)
      {
        (*scope)--;
      }
    }
    else
    {
      *selection=menu_size-1;
      *scope=max(menu_size-height-1,0);
    }
  }
  else
  {
    return false; //No keys were pressed=no change happened
  }
  return true;
}


//Types:
//0 = Normal string, no limits
//1 = a-z, A-Z, 0-9 and _ (resource names etc)
//2 = 0-9 and - (ints)
//3 = 0-9 and - and . (reals)
int textbox(WINDOW *curr_screen,char **src,int y, int x, int len,int on,char *deflt,int limit)
{
    int i;
    int attr_backup;
    char *text_buffer;
    if (*src==NULL)
    {
      *src=calloc(1,1);
    }
    int cursor_pos=0;
    int scope_pos=0;
    text_buffer=malloc(strlen(*src)+1);
    stpcpy(text_buffer, *src);
    int lastchar=0;
    mvwaddnstr(curr_screen,y,x,*src,min(strlen(*src)+1,len));
    for (i=strlen(*src)-scope_pos;i<len;i++)
    {
      //I got a blank space baby, and I'll write spaces
      //...So that if there are characters left over from before they'll
      //get cleared
      mvwaddstr(curr_screen,y, x+i, " ");
    }
    if (on)
    {
      curs_set(true);
      wmove(curr_screen,y,x);
      wrefresh(curr_screen);
      while (true)
      {
        lastchar=getch();
        if ((lastchar=='\n')|(lastchar==27))
        {
          break;
        }
        else if (lastchar==KEY_DC) //DEL
        {
          if (cursor_pos<strlen(text_buffer))
          {
            //Keep in mind that strlen doesn't count the NULL
            memmove(&text_buffer[cursor_pos], &text_buffer[cursor_pos+1], (strlen(text_buffer)-cursor_pos)+2);
            //text_buffer=realloc(text_buffer,strlen(text_buffer)+1); //Only reduce the length of the buffer after we've shortened the string
          }
        }
        else if (lastchar==KEY_BACKSPACE)
        {
          if (cursor_pos>0)
          {
            //Keep in mind that strlen doesn't count the NULL
            cursor_pos--;
            memmove(&text_buffer[cursor_pos], &text_buffer[cursor_pos+1], (strlen(text_buffer)-cursor_pos)+2);
            //text_buffer=realloc(text_buffer,strlen(text_buffer)+1); //Only reduce the length of the buffer after we've shortened the string

          }
        }
        else if (lastchar==KEY_LEFT)
        {
          if (cursor_pos>0)
          {
            cursor_pos--;
          }
        }
        else if (lastchar==KEY_RIGHT)
        {
          if (cursor_pos<strlen(text_buffer))
          {
            cursor_pos++;
          }
        }
        //Limit to allowed characters
        else if ((limit==0)|
                ((limit==1)&(
                  ((lastchar>=48)&(lastchar<=57))|
                  ((lastchar>=65)&(lastchar<=90))|
                  (lastchar==95)|
                  ((lastchar>=97)&(lastchar<=122))))
                |(((limit==2)|(limit==3))&
                  (((lastchar>=48)&
                  (lastchar<=57))|
                  (lastchar=='-')|
                  ((lastchar=='.')&(limit==3)))))
        {
          text_buffer=realloc(text_buffer,(strlen(text_buffer)+2));
          memmove(&text_buffer[cursor_pos+1], &text_buffer[cursor_pos], (strlen(text_buffer)-cursor_pos)+1);
          text_buffer[cursor_pos++]=lastchar; //Add the char and move the cursor
        }
        //Adjust the area displayed
        if (scope_pos+len<cursor_pos)
        {
          scope_pos++;
        }
        else if (scope_pos>cursor_pos)
        {
          scope_pos--;
        }

        for (i=strlen(text_buffer)-scope_pos;i<len;i++)
        {
          mvwaddstr(curr_screen,y, x+i, " ");
        }

        mvwaddnstr(curr_screen,y,x,text_buffer+scope_pos,min(strlen(text_buffer)+1,len));
        wmove(curr_screen,y,x+cursor_pos-scope_pos);
        wrefresh(curr_screen);

      }
      curs_set(false);
      attr_backup=wattr_get(curr_screen);
      attr_set(A_NORMAL);

      for (i=strlen(text_buffer)-scope_pos;i<len;i++)
      {
        mvwaddstr(curr_screen,y, x+i, " ");
      }

      attr_set(attr_backup);
      if (lastchar=='\n')
      {
        //save
        if (strlen(text_buffer)==0)
        {
          //Default when empty
          *src=realloc(*src, strlen(deflt)+1);
          strncpy(*src, deflt, strlen(deflt)+1);
        }
        else
        {
          *src=realloc(*src,strlen(text_buffer)+1);
          strncpy(*src, text_buffer, strlen(text_buffer)+1);
        }
        mvwaddnstr(curr_screen,y,x,*src,min(strlen(*src)+1,len));
        free(text_buffer);
        return 1; //Saved to src
      }
      else
      {
        free(text_buffer);
        return 0; //Discarded
      }
    }
    free(text_buffer);
    return -1;
}
#define resource_name_box(curr_screen,src,y, x, len, on) textbox(curr_screen,src, y, x, len, on, "<undefined>", 1)
#define file_box(curr_screen,src,y, x, len, on) textbox(curr_screen,src, y, x, len, on, "/", 0)
int int_box(WINDOW *curr_screen, int *src,int y, int x, int len,int on)
{
  int retval;
  char *str_int_converted=malloc((CHAR_BIT * sizeof(int) - 1) / 3 + 2);
  sprintf(str_int_converted, "%i", *src);
  retval=textbox(curr_screen, &str_int_converted,y, x, len,on,"0",2);
  *src=atoi(str_int_converted);
  free(str_int_converted);
  return retval;
}

int draw_window_border(WINDOW *window, char *title)
{
  int i;
  int width;
  int height;
  getmaxyx(window, height, width);
  height--;
  width--;
  //border+titlebar
  mvwaddstr(window,0,0,"╭");
  mvwaddstr(window,0,width,"╮");
  mvwaddstr(window,height,0,"╰");
  mvwaddstr(window,height,width,"╯");
  for (i=1;i<width;i++)
  {
    mvwaddstr(window,0,i,"─");
    mvwaddstr(window,height,i,"─");
  }
  for (i=1;i<height;i++)
  {
    mvwaddstr(window,i,0,"│");
    mvwaddstr(window,i,width,"│");
  }
  mvwprintw(window,0,(width-2-strlen(title))/2,"[%s]",title);
  return 1;
}

int edit_event_menu(struct gm_event *event)
{
  WINDOW *event_window;
  int last_key=0;
  int i;
  int array_range=0;
  int array_chosen=0;
  int array_draw_y;
  int array_scope=0;
  int array_index=0;
  char **choice_array;
  if (event->eventtype>1)//Check that it's not one of the events where the enumb isn't used
  {
    event_window=newwin(terminal_height-4, terminal_width-4, 2,2);
    draw_window_border(event_window, event_type_name[event->eventtype]);
    if ((event->eventtype>=5)|(event->eventtype==3))
    {
      switch (event->eventtype)
      {
        case event_step:
          choice_array=step_event_type_name;
          array_range=sizeof(step_event_type_name) / sizeof(step_event_type_name[0]);
          break;
        case event_key:
        case event_key_pressed:
        case event_key_released:
          choice_array=keys_names;
          array_range=sizeof(keys_names) / sizeof(keys_names[0]);
          break;
        case event_mouse:
          choice_array=mouse_button_names;
          array_range=sizeof(mouse_button_names) / sizeof(mouse_button_names[0]);
          break;
        case event_other:
          choice_array=other_event_type_name;
          array_range=sizeof(other_event_type_name) / sizeof(other_event_type_name[0]);
          break;
        case event_draw:
          choice_array=draw_event_type_name;
          array_range=sizeof(draw_event_type_name) / sizeof(draw_event_type_name[0]);
          break;
      }
      int non_null_count = count_names(choice_array,array_range);
      while ((last_key!=27)&(last_key!='\n'))
      {
        wclear(event_window);
        handle_menu_ctrls(last_key, &array_chosen,&array_scope,non_null_count,terminal_height-7);
        for (i=array_draw_y=0;(i<array_range)&(array_draw_y-array_scope<=terminal_height-6);i++)
        {
          if (choice_array[i]!=NULL)
          {
            if (array_draw_y==array_chosen)
            {
              //Save the actual index of the selected item, not the position it's drawn at
              array_index=i;
              wattron(event_window,A_REVERSE);
            }
            mvwaddstr(event_window,(++array_draw_y)-array_scope,2,choice_array[i]);
            wattroff(event_window,A_REVERSE);
          }
        }
        draw_window_border(event_window, event_type_name[event->eventtype]);
        wrefresh(event_window);
        last_key=getch();
      }
      if (last_key=='\n')
      {
        if (event->eventtype==event_draw)
        {
          event->enumb=event_draw_index_to_enumb(array_index);
        }
        else
        {
          event->enumb=array_index;
        }
      }
    }
    else if (event->eventtype==event_collision)
    {
      while (last_key!=27)
      {
        mvwaddstr(event_window,1,2,event_enumb_name[event->eventtype]);
        wattron(event_window,A_REVERSE);
        textbox(event_window, &event->ename, 2, 2, terminal_width-7,last_key=='\n',"",0);
        wattroff(event_window,A_REVERSE);
        wrefresh(event_window);
        last_key=getch();
      }
    }
    else
    {
      while (last_key!=27)
      {
        mvwaddstr(event_window,1,2,event_enumb_name[event->eventtype]);
        wattron(event_window,A_REVERSE);
        int_box(event_window, &event->enumb, 2, 2, terminal_width-7,last_key=='\n');
        wattroff(event_window,A_REVERSE);
        wrefresh(event_window);
        last_key=getch();
      }
    }
    delwin(event_window);
  }
  clear();
}

int edit_action_menu(struct gm_action *selected_action)
{
  WINDOW *action_window;
  int last_key=0;
  int i;
  int selected_argument=0;
  struct lgl_action_meta *curr_action_meta;
  curr_action_meta=get_action_meta(selected_action->id);
  if (curr_action_meta!=NULL)
  {
    //If it's only code we want it to open in our favourite code editor
    if (curr_action_meta->kind==action_code)
    {
      char *temp_file_name;
      char *command;
      int fileid=rand();
      temp_file_name=malloc(strlen("/tmp/obj_.gml")+strlen(curr_action_meta->name)+int_len(fileid)+1);
      //Generate a random file name
      sprintf(temp_file_name,"/tmp/obj_%s%i.gml",curr_action_meta->name,fileid);
      //Set the command used to open that file
      command=malloc(strlen(code_editor)+strlen(temp_file_name)+1);
      sprintf(command,code_editor,temp_file_name);
      //Write the contents of the buffer
      FILE *file_pointer = fopen(temp_file_name, "w");
      if (file_pointer!=NULL)
      {
        if (selected_action->arguments[0]->string!=NULL)
        {
          fputs(selected_action->arguments[0]->string, file_pointer);
        }
        fclose(file_pointer);
        def_prog_mode();
        endwin();
        printf("Waiting for text editor to close...\n");
        if (system(command)<0)
        {
          printf("Error opening text editor\n");
        }
        reset_prog_mode();
        refresh();
        //Get the contents of the buffer again
        FILE *file_pointer = fopen(temp_file_name, "rb");
        if (file_pointer!=NULL)
        {
          fseek(file_pointer, 0L, SEEK_END);
          //Get the size of the file
          long long size = ftell(file_pointer);
          rewind(file_pointer);
          //Make sure there's enough space in the buffer
          selected_action->arguments[0]->string=realloc(selected_action->arguments[0]->string,size+1);
          fread(selected_action->arguments[0]->string,sizeof(char), size, file_pointer);
          selected_action->arguments[0]->string[size]=0;
          fclose(file_pointer);
        }
      }
      free(command);
      free(temp_file_name);
    }
    else if (curr_action_meta->arg_count>0)
    {
      action_window = newwin(terminal_height-4, terminal_width-4, 2,2);
      //border+titlebar
      draw_window_border(action_window, curr_action_meta->name);

      last_key=0;
      //Exit the action window when the use presses ESC
      while (last_key!=27)
      {
        if ((last_key==KEY_RIGHT) | (last_key==KEY_DOWN))
        {
          if (selected_argument<selected_action->arg_count-1)
          {
            selected_argument++;
          }
          else
          {
            selected_argument=0;
          }
        }
        else if ((last_key==KEY_LEFT) | (last_key==KEY_UP))
        {
          if (selected_argument>0)
          {
            selected_argument--;
          }
          else
          {
            selected_argument=selected_action->arg_count-1;
          }
        }
        //Actual UI stuff
        for (i=0;i<selected_action->arg_count;i++)
        {
          wattron(action_window, A_BOLD);
          mvwaddstr(action_window,1+(i*3),
            2,
            curr_action_meta->arguments[i]->caption);
          wattroff(action_window, A_BOLD);
          if (i==selected_argument)
          {
            wattron(action_window, A_REVERSE);
          }
          textbox(action_window,
            &selected_action->arguments[i]->string,2+(i*3),
            2,
            terminal_width-7,
            (last_key=='\n')&(i==selected_argument),
            curr_action_meta->arguments[i]->deflt,
            arg_to_textbox_limit[selected_action->arguments[i]->kind]);
          wattroff(action_window,A_REVERSE);
        }
        refresh();
        wrefresh(action_window);
        last_key=getch();
      }
      delwin(action_window);
      clear();
    }
  }
}

int new_event_menu(struct gm_object *obj,int *selected_event)
{
  int selected_event_to_add=0;
  int event_scope_to_add=0;
  int last_key=0;
  int i;
  WINDOW *new_event_window;
  new_event_window=newwin(terminal_height-4, terminal_width-4, 2,2);
  while ((last_key!='\n')&(last_key!=27))
  {
    wclear(new_event_window);
    handle_menu_ctrls(last_key,&selected_event_to_add,&event_scope_to_add,11,terminal_height-7);
    for (i=event_scope_to_add;(i<11)&(i<event_scope_to_add+terminal_height-4);i++)
    {
      if (i==selected_event_to_add)
      {
        wattron(new_event_window,A_REVERSE);
      }
      mvwaddstr(new_event_window,1+i-event_scope_to_add, 2, event_type_name[i]);
      wattroff(new_event_window,A_REVERSE);
    }
    draw_window_border(new_event_window, "New Event");
    wrefresh(new_event_window);
    last_key=getch();
  }
  if (last_key=='\n')
  {
    new_event(obj,selected_event_to_add);
    edit_event_menu(obj->events[obj->event_count-1]);
    *selected_event=obj->event_count-1;
    return true;
  }
  else
  {
    clear();
    return false;
  }
}

int new_action_menu(struct gm_object *obj, int selected_event, int selected_action, int *section)
{
  struct lgl_action_meta action_to_add_meta;
  int selected_id;
  int selected_action_to_add=0;
  int action_scope_to_add=0;
  int i,ii;
  WINDOW *new_action_window;
  new_action_window=newwin(terminal_height-4, terminal_width-4, 2,2);
  struct lgl_action_meta *action_list;
  int last_key=0;
  while ((last_key!=27)&(last_key!='\n'))
  {
    wclear(new_action_window);
    //Controls
    handle_menu_ctrls(last_key,&selected_action_to_add,&action_scope_to_add,lgl_action_count,terminal_height-7);
    for (action_list=action_meta_dict, i = 0;(action_list != NULL)&(i-action_scope_to_add<terminal_height-6)&
      (i<action_scope_to_add+terminal_height - 6);
      action_list=action_list->hh.next, i++)
    {
      //Only draw if it's within the scope
      if ((i+1>action_scope_to_add))
      {
        if (i==selected_action_to_add)
        {
          selected_id=action_list->id;
          wattron(new_action_window,A_REVERSE);
        }
        //If the kind is 10 it's a label, not an actual action
        if (action_list->kind==10)
        {
          wattron(new_action_window,A_BOLD);
          mvwprintw(new_action_window,i-action_scope_to_add+1,2,"[%s]",action_list->name);
          wattroff(new_action_window,A_BOLD);
        }
        //If the kind is 9 it's a separator
        else if (action_list->kind==9)
        {
          for (ii=1;ii<terminal_width-5;ii++)
          {
            mvwaddstr(new_action_window,i-action_scope_to_add+1,ii,"─");
          }
        }
        else
        {
          mvwaddstr(new_action_window,i-action_scope_to_add+1,2,action_list->name);
        }
        wattroff(new_action_window,A_REVERSE);
      }
    }
    draw_window_border(new_action_window, "New action");
    wrefresh(new_action_window);
    last_key=getch();
  }
  action_to_add_meta=*get_action_meta(selected_id);
  if ((last_key=='\n')&(action_to_add_meta.kind!=action_seperator)&(action_to_add_meta.kind!=action_label))
  {
    //Make space in the array for the new action, selected_action++
    new_action(obj->events[selected_event], selected_action);
    //Copy stuff from template
    obj->events[selected_event]->actions[selected_action]->libid=action_to_add_meta.libid;
    obj->events[selected_event]->actions[selected_action]->id=action_to_add_meta.id;
    obj->events[selected_event]->actions[selected_action]->kind=action_to_add_meta.kind;
    obj->events[selected_event]->actions[selected_action]->userelative=action_to_add_meta.userelative;
    obj->events[selected_event]->actions[selected_action]->useapplyto=action_to_add_meta.useapplyto;
    obj->events[selected_event]->actions[selected_action]->isquestion=action_to_add_meta.isquestion;
    obj->events[selected_event]->actions[selected_action]->exetype=action_to_add_meta.exetype;
    obj->events[selected_event]->actions[selected_action]->functionname=action_to_add_meta.functionname;
    obj->events[selected_event]->actions[selected_action]->codestring=action_to_add_meta.codestring;
    //Because this won't be filled by the template, this will always be the default anyways
    obj->events[selected_event]->actions[selected_action]->whoName = malloc((strlen("self")+1)*sizeof(char));
    strcpy(obj->events[selected_event]->actions[selected_action]->whoName,"self");
    //
    obj->events[selected_event]->actions[selected_action]->arg_count=action_to_add_meta.arg_count;
    obj->events[selected_event]->actions[selected_action]->arguments = calloc(obj->events[selected_event]->actions[selected_action]->arg_count, sizeof(struct gm_argument));
    for (i=0;i<obj->events[selected_event]->actions[selected_action]->arg_count;i++)
    {
      struct gm_argument *curr_arg = calloc(1,sizeof(struct gm_argument));
      curr_arg->kind=action_to_add_meta.arguments[i]->kind;
      obj->events[selected_event]->actions[selected_action]->arguments[i]=curr_arg;
    }
    //Add an action created from the get_action_meta(ID RETURNED FROM BEFORE)
    edit_action_menu(obj->events[selected_event]->actions[selected_action]);
    *section=2;
  }
  clear();
}

void draw_events_and_actions(struct gm_object *obj, int event_scope, int action_scope, int selected_event, int selected_action, int section)
{
  int i, ii;
  for (i=event_scope;i<obj->event_count;i++)
  {
    if (selected_event==i)
    {
      int ii;
      for (ii=action_scope;ii<obj->events[i]->action_count;ii++)
      {
        struct lgl_action_meta *curr_action_meta=get_action_meta(obj->events[i]->actions[ii]->id);
        if ((selected_action==ii)&(section==2))
        {
          attron(A_REVERSE);
        }
        //Clear the space so that there aren't characters left over from before
        mvprintw((ii-action_scope)+2, 9+33, "%i                                 ", ii);
        //We're using event count so that the level of the │ and the function
        //name doesn't change when you're scrolling because that's ugly
        if (curr_action_meta!=NULL)
        {
          mvprintw((ii-action_scope)+2, 9+34+int_len(obj->event_count), "│%s", curr_action_meta->name);
        }
        else
        {
          //Fallback to the functionname provided by the GMX if we have no name for it
          mvprintw((ii-action_scope)+2, 9+34+int_len(obj->event_count), "│%s (missing)", obj->events[i]->actions[ii]->functionname);
        }
        attroff(A_REVERSE);
      }
      if (section==1)
      {
        attron(A_REVERSE);
      }
    }
    mvaddstr((i-event_scope)+2, 19, "                       ");

    switch (obj->events[i]->eventtype)
    {
      case event_alarm:
        mvprintw((i-event_scope)+2,19,"│⌛  Alarm  %i",obj->events[i]->enumb);
        break;
      case event_step:
        mvprintw((i-event_scope)+2, 19,"│🚶  %s",step_event_type_name[obj->events[i]->enumb]);
        break;
      case event_collision:
        mvprintw((i-event_scope)+2, 19,"│↸  %.18s",obj->events[i]->ename);
        break;
      case event_key:
        mvprintw((i-event_scope)+2, 19,"│️ ⃣️  %s", keys_names[obj->events[i]->enumb]);
        break;
      case event_mouse:
        mvprintw((i-event_scope)+2, 19,"│️┬⃣️  %s", mouse_button_names[obj->events[i]->enumb]);
        break;
      case event_other:
        mvprintw((i-event_scope)+2, 19,"│…  %s",other_event_type_name[obj->events[i]->enumb]);
        break;
      case event_draw:
        mvprintw((i-event_scope)+2, 19,"│️✏  %s", draw_event_type_name[event_draw_enumb_to_index(obj->events[i]->enumb)]);
        break;
      case event_key_pressed:
        mvprintw((i-event_scope)+2, 19,"│️⬇️⃣️  %s", keys_names[obj->events[i]->enumb]);
        break;
      case event_key_released:
        mvprintw((i-event_scope)+2, 19,"│️⬆️⃣️  %s", keys_names[obj->events[i]->enumb]);
        break;
      default:
        mvprintw((i-event_scope)+2, 19,"│%s",event_type_name[obj->events[i]->eventtype]);
        break;
    }
    attroff(A_REVERSE);
  }
  for (ii=(
      ((selected_event<obj->event_count)&(selected_event>=0))?
      (obj->events[selected_event]->action_count):
      (0)
    )+2;ii<terminal_height;ii++)
  {
    //If we're not filling the whole screen with actions then clear those parts
    mvaddstr(ii, 9+33, "                                 ");
  }
  for (ii=obj->event_count+2;ii<terminal_height;ii++)
  {
    //If we're not filling the whole screen with events then clear those parts
    mvaddstr(ii, 19, "                       ");
  }
}

void toolbar_active(struct gm_object *obj, char **obj_file, int *selected_event, int *selected_action, int *section)
{
  int toolbar_option=0;
  int last_key=0;
  int toolbar_max=2;
  attron(A_BOLD|A_REVERSE);
  while (last_key!=27)
  {
    mvaddstr(0, 0, "New│Save│Load│");
    if (last_key==KEY_RIGHT)
    {
      if (toolbar_option>=toolbar_max)
      {
        toolbar_option=0;
      }
      else
      {
        toolbar_option++;
      }
    }
    else
    {
      if (last_key==KEY_LEFT)
      {
        if (toolbar_option<=0)
        {
          toolbar_option=toolbar_max;
        }
        else
        {
          toolbar_option--;
        }
      }
    }

    if (toolbar_option==0)
    {
      attroff(A_REVERSE);
      mvaddstr(0, 0, "New");
      attron(A_REVERSE);
      if (last_key=='\n')
      {
        mvaddstr(0, strlen("New|Save|Load|"), "WARNING: Any unsaved changes will be destroyed! [Enter]");
        mvaddstr(0, terminal_width-strlen(*obj_file), *obj_file);
        refresh();
        //Any other key acts like a no... Something Macroshaft doesn't seem to understand
        if (getch()=='\n')
        {
          obj_free(obj);
          obj=new_object();
          //Let's just use an empty string
          *obj_file=calloc(1,sizeof(char));
        }
        last_key=0;
        clear();
        break;
      }
    }
    else if (toolbar_option==1)
    {
      attroff(A_REVERSE);
      mvaddstr(0, strlen("New|"), "Save");
      attron(A_REVERSE);
      if (last_key=='\n')
      {
        if (file_box(stdscr,&(*obj_file),0, strlen("New|Save|Load|"), terminal_width-(strlen("New|Save|Load|")+strlen(*obj_file)), true)==1)
        {
          save_obj(obj,*obj_file);
        }
        break;
      }
    }
    else if (toolbar_option==2)
    {
      attroff(A_REVERSE);
      mvaddstr(0, strlen("New|Save|"), "Load");
      attron(A_REVERSE);
      if (last_key=='\n')
      {
        mvaddstr(0, strlen("New|Save|Load|"), "WARNING: Any unsaved changes will be destroyed! [Enter]");
        mvaddstr(0, terminal_width-strlen(*obj_file), *obj_file);
        refresh();
        if (getch()=='\n')
        {
          if (file_box(stdscr,&(*obj_file),0, strlen("New|Save|Load|"), terminal_width-(strlen("New|Save|Load|")+strlen(*obj_file)), true)==1)
          {
            //Reload!
            obj_free(obj);
            obj=object_from_file(*obj_file);
            if (obj==NULL)
            {
              exit(1);
            }
            //Reset this stuff
            selected_event=0;
            selected_action=0;
            section=0;
          }
        }
        last_key=0;
        clear();
        break;

      }
    }
    last_key=getch();
    refresh();
  }
  attroff(A_BOLD|A_REVERSE);
}

void draw_toolbar(char *obj_file)
{
  int i;
  attron(A_BOLD|A_REVERSE);
  mvaddstr(0, 0, "New│Save│Load│");
  for (i=strlen("New|Save|Load|");i<terminal_width;i++){mvaddstr(0, i, " ");}
  mvaddstr(0, terminal_width-strlen(obj_file), obj_file);
  attroff(A_BOLD|A_REVERSE);
}

int main(int argc, char **argv)
{
  char lib_location[256]="./lib/";
  int i;
  int ii;
  init_names();
  //We don't use any function keys, so don't bother with escape delay
  ESCDELAY=0;
  srand(time(NULL));
  //We're doing this rather than a simple "./lib" because "." gets the working
  //directory, not the directory of the executable file itself.
  if (readlink("/proc/self/exe", lib_location, 256)==-1)
  {
    printf("[Error] Unable to get executable location, are you running something other than Linux?");
  }
  dirname(lib_location);
  strcat(lib_location,"/lib/");
  load_lgl_dir(lib_location);

  int last_key=0;
  setlocale(LC_ALL, "en_US.UTF-8");
  int v_option=0;
  int selected_event=0;
  int selected_action=0;
  int event_scope=0;
  int action_scope=0;
  #define obj_settings_max 7
  int section=0;
  struct gm_object *obj;
  #define sections_max 2
  //We need to have our own stored file name that can be realloced later
  //And if we don't have a file name right now, use a default
  char *obj_file;
  if (argc>=2)
  {
    obj_file=malloc(strlen(argv[1])+1);
    strcpy(obj_file,argv[1]);
    obj=object_from_file(obj_file);
    if (obj==NULL)
    {
      exit(1);
    }
  }
  else
  {
    obj_file=calloc(strlen(" ")+1,sizeof(char));
    obj=new_object();
  }
  initscr();
  keypad(stdscr, TRUE);
  curs_set(false);
  noecho();
  while (true)
  {
    getmaxyx(stdscr, terminal_height, terminal_width);

    //Don't try to do any of this if there's no event to change or it WILL segfault
    if ((selected_event<obj->event_count)&(selected_event>=0))
    {
      //These control the scrolling mechanism
      //Events
      if (section==1)
      {
        //If there was a change we need to jump to the first item in the list
        if (handle_menu_ctrls(last_key,&selected_event,&event_scope,obj->event_count,terminal_height-3))
        {
          selected_action=0;
          action_scope=0;
        }
      }
      //Actions
      else if ((section==2) & (selected_action<obj->events[selected_event]->action_count))
      {
        handle_menu_ctrls(last_key,&selected_action,&action_scope,obj->events[selected_event]->action_count,terminal_height-3);
      }

      if (last_key=='\n')
      {
        //Show action editing window
        if (section==1)
        {
          edit_event_menu(obj->events[selected_event]);
        }
        else if ((section==2) & (selected_action<obj->events[selected_event]->action_count)&(obj->events[selected_event]->actions!=NULL))
        {
          edit_action_menu(obj->events[selected_event]->actions[selected_action]);
        }
      }

      if (last_key==delete_item_key)
      {
        if (section==1)
        {
          delete_event(obj,selected_event--);
          if (selected_event<0)
          {
            selected_event=0;
          }
        }
        else if ((section==2) & (selected_action<=obj->events[selected_event]->action_count))
        {
          delete_action(obj->events[selected_event],selected_action--);
          if (selected_action<0)
          {
            selected_action=0;
          }
        }
      }
      
      //Move action
      if (last_key==move_up_key)
      {
        section=2;
        if (move_action(obj->events[selected_event], selected_action,selected_action+1))
        {
          selected_action++;
          if ((selected_action>=(terminal_height-2)+action_scope)&
            (action_scope<obj->events[selected_event]->action_count-(terminal_height-2))) //To stop it leaving an ugly trail of stuff
          {
            action_scope++;
          }
        }
      }
      else if (last_key==move_down_key)
      {
        section=2;
        //Only move the selection if the move was successful
        if (move_action(obj->events[selected_event], selected_action,selected_action-1))
        {
          selected_action--;
          if (selected_action<action_scope)
          {
            action_scope--;
          }
        }
      }
    }

    //Add action, make sure there are events to add the action to
    if ((last_key==create_new_action_key)&(selected_event<obj->event_count))
    {
      new_action_menu(obj, selected_event, selected_action, &section);
    }
    //Add event
    if (last_key==create_new_event_key)
    {
      new_event_menu(obj, &selected_event);
    }
    //Changing sections
    if (last_key=='\t')
    {
      if (section<sections_max)
      {
        section++;
      }
      else
      {
        section=0;
      }
    }
    //toolbar
    //We're doing this first so that everything is up to date when we draw it

    if (last_key==27)
    {
      toolbar_active(obj, &obj_file, &selected_event, &selected_action, &section);
    }
    draw_toolbar(obj_file);

    //Basic options
    if (section==0)
    {
      if ((last_key==KEY_DOWN)|(last_key==KEY_RIGHT))
      {
        if (v_option>=obj_settings_max)
        {
          v_option=0;
        }
        else
        {
          v_option++;
        }
      }
      else if ((last_key==KEY_UP)|(last_key==KEY_LEFT))
      {
        if (v_option<=0)
        {
          v_option=obj_settings_max;
        }
        else
        {
          v_option--;
        }
      }
    }

    //Box
    mvaddstr(1, 0,                      "┌───────────────┐");
    for (i=2;i<16;i++)
    {
      mvaddstr(i, 0, "│");
      mvaddstr(i, 16,"│");
    }
    mvaddstr(16, 0,                     "└───┴───┴───┴───┘");
    //UI
    attron(A_BOLD);
    mvaddstr(3-1, 1,   "Sprite:");
    mvaddstr(3+2, 1,   "Mask:");
    mvaddstr(3+5, 1,   "Parent:");
    mvaddstr(3+5+3, 1, "Depth:");
    mvaddstr(3+8+3, 1, "Vis│Sol│Per│Phy");
    for (i=4;i<16;i+=4)
    {
      mvaddstr(3+9+3, i,  "│");
    }

    attroff(A_BOLD);

    if ((v_option==0)&(section==0))
    {
      attron(A_REVERSE);
    }
    resource_name_box(stdscr,&obj->spriteName,3, 1, 15, (v_option==0)&(last_key=='\n')&(section==0));
    attroff(A_REVERSE);
    if ((v_option==1)&(section==0))
    {
      attron(A_REVERSE);
    }
    resource_name_box(stdscr,&obj->maskName,3+3, 1, 15, (v_option==1)&(last_key=='\n')&(section==0));
    attroff(A_REVERSE);
    if ((v_option==2)&(section==0))
    {
      attron(A_REVERSE);
    }
    resource_name_box(stdscr,&obj->parentName,3+6, 1, 15, (v_option==2)&(last_key=='\n')&(section==0));
    attroff(A_REVERSE);
    if ((v_option==3)&(section==0))
    {
      attron(A_REVERSE);
    }
    int_box(stdscr,&obj->depth,3+9, 1, 15, (v_option==3)&(last_key=='\n')&(section==0));
    attroff(A_REVERSE);
    if ((v_option==4)&(section==0))
    {
      if (last_key=='\n')
      {
        //For some reason yyg decided to use -1 and 0 rather than 1 and 0
        //like normal people
        obj->visible^=-1;
      }
      attron(A_REVERSE);
    }
    draw_bool(3+9+3,2,obj->visible);
    attroff(A_REVERSE);
    if ((v_option==5)&(section==0))
    {
      if (last_key=='\n')
      {
        obj->solid^=1;
      }
      attron(A_REVERSE);
    }
    draw_bool(3+9+3,6,obj->solid);
    attroff(A_REVERSE);
    if ((v_option==6)&(section==0))
    {
      if (last_key=='\n')
      {
        obj->persistent^=1;
      }
      attron(A_REVERSE);
    }
    draw_bool(3+9+3,10,obj->persistent);
    attroff(A_REVERSE);
    if ((v_option==7)&(section==0))
    {
      if (last_key=='\n')
      {
        obj->PhysicsObject^=1;
      }
      attron(A_REVERSE);
    }
    draw_bool(3+9+3,14,obj->PhysicsObject);
    attroff(A_REVERSE);

    draw_events_and_actions(obj, event_scope, action_scope, selected_event, selected_action, section);

    refresh();
    last_key=getch();
  }
  endwin();
  return 0;
}
