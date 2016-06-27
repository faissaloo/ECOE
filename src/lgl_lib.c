#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "uthash.h"
#include "lgl_lib.h"
struct lgl_action_meta *action_meta_dict=NULL; //The hashtable that holds all loaded
                                          //actions
int lgl_action_count=0;
int read_byte_prefix_str(FILE *file_stream, char **put_in)
{
  int string_len;
  string_len=fgetc(file_stream)+1;
  *put_in=malloc(string_len);
  //Returns true if none of these are NULL
  return fgets(*put_in,string_len,file_stream)&&put_in&&string_len;
}

int load_lgl(char *file_str)
{
  unsigned int action_count;
  unsigned int libid;
  int i,ii;
  unsigned int string_len;
  unsigned char flags;
  FILE *file_stream;
  file_stream=fopen(file_str,"rb");
  if (file_stream)
  {
    //This contains the initial stuff
    //3 (string) "LGL"
    if (fgetc(file_stream)=='L' && fgetc(file_stream)=='G' && fgetc(file_stream)=='L')
    {
      //2 LGM version needed for the following info (160)
      fseek(file_stream, 2, SEEK_CUR);
      //3 Lib ID (if ID < 1000, this file is protected as official)
      libid=((unsigned int) fgetc(file_stream)) + ((unsigned int) fgetc(file_stream)<<8) + ((unsigned int) fgetc(file_stream)<<16);
      //(Skip) 1 Length of Tab Caption { Tab Caption }
      fseek(file_stream, fgetc(file_stream), SEEK_CUR);
      //(Skip) 1 Length of Author { Author }
      fseek(file_stream, fgetc(file_stream), SEEK_CUR);
      //(Skip) 4 Lib Version
      fseek(file_stream, 4, SEEK_CUR);
      //(Skip) 8 (double) Last Changed date
      fseek(file_stream, 8, SEEK_CUR);

      //(Skip) 4 Length of Info { Info }
      fseek(file_stream, ((unsigned int) fgetc(file_stream)) + ((unsigned int) fgetc(file_stream)<<8) + ((unsigned int) fgetc(file_stream)<<16) + ((unsigned int) fgetc(file_stream)<<24), SEEK_CUR);

      //(Skip) 4 Length of Initialization Code { Initialization Code }
      fseek(file_stream, ((unsigned int) fgetc(file_stream)) + ((unsigned int) fgetc(file_stream)<<8) + ((unsigned int) fgetc(file_stream)<<16) + ((unsigned int) fgetc(file_stream)<<24), SEEK_CUR);
      //1 >>
      //  > (Ignore) 1b Advanced (CAUSING MAJOR CRASHES)
      //  > 7b Number of Actions
      action_count=fgetc(file_stream)&0x7F;
      for (i=0;i<action_count;i++)
      {
        struct lgl_action_meta *curr_action=calloc(1,sizeof(struct lgl_action_meta));
        curr_action->libid=libid;
        //(Skip) 2 LGM version needed for the following info (160)
        fseek(file_stream, 2, SEEK_CUR);
        //2 Action Id
        curr_action->id=((int) fgetc(file_stream)) + ((int) fgetc(file_stream)<<8);
        //1 Length of Name { Name }
        read_byte_prefix_str(file_stream, &curr_action->name);
        //(Skip) 1 Length of Description { Description }
        fseek(file_stream, fgetc(file_stream), SEEK_CUR);
        //1 Length of List Text { List Text }
        fseek(file_stream, fgetc(file_stream), SEEK_CUR);
        //1 Length of Hint Text { Hint Text }
        fseek(file_stream, fgetc(file_stream), SEEK_CUR);
        //1 >>
        flags=fgetc(file_stream);
        //(Ignore) > 1b Hidden
        //(Ignore) > 1b Advanced
        //(Ignore) > 1b Registered only
        //> 1b Question
        curr_action->isquestion=(flags&0x10)>>4;
        // > 1b Apply to
        curr_action->useapplyto=(flags&0x8)>>3;
        // > 1b Relative
        curr_action->userelative=(flags&0x4)>>2;
        // > 2b Execution Type (0-2)
        curr_action->exetype=flags&0x3;
        //4 Length of Execution Information[1] { Execution Information }
        string_len=((int) fgetc(file_stream)) + ((int) fgetc(file_stream)<<8) + ((int) fgetc(file_stream)<<16) + ((int) fgetc(file_stream)<<24);
        //Execution Information contains Function Name
        if (curr_action->exetype==1)
        {
          curr_action->functionname=malloc(string_len+1);
          fgets(curr_action->functionname,string_len+1,file_stream);
          curr_action->codestring=calloc(1,sizeof(char));
        }
        //Execution Information contains Code
        else if (curr_action->exetype==2)
        {
          curr_action->codestring=malloc(string_len+1);
          fgets(curr_action->codestring,string_len+1,file_stream);
          curr_action->functionname=calloc(1,sizeof(char));
        }

        //1 >>
        flags=fgetc(file_stream);
        //> 4b Kind[2] (0-10)
        curr_action->kind=(flags&0xF0)>>4;
        //> 4b Interface Kind (0=Normal, 1=None, 2=Arrows, 5=Code, 6=Text)
        curr_action->interface=(flags&0x0F);

        //1 Argument Count
        curr_action->arg_count=fgetc(file_stream);
        curr_action->arguments=malloc(curr_action->arg_count*sizeof(struct lgl_argument_meta));
        //Arguments
        for (ii=0;ii<curr_action->arg_count;ii++)
        {
          struct lgl_argument_meta *curr_arg = (struct lgl_argument_meta*)malloc(sizeof(struct lgl_argument_meta));
          //1 Length of Argument Caption { Argument Caption }
          read_byte_prefix_str(file_stream, &curr_arg->caption);

          //1 Argument Type[3] (0-15)
          curr_arg->kind=fgetc(file_stream);

          //1 Length of Default Argument Value { Default Argument Value }
          read_byte_prefix_str(file_stream, &curr_arg->deflt);

          //1 Length of Menu { Menu }
          read_byte_prefix_str(file_stream, &curr_arg->menu);

          curr_action->arguments[ii]=curr_arg;
        }
        lgl_action_count++;
        //Finally, add it to the dictionary
        HASH_ADD_INT(action_meta_dict, id, curr_action);
      }
      fclose(file_stream); //Close it
      return 1;
    }
    else
    {
      fclose(file_stream);
      printf("[Error] %s is not a valid LGL file\n",file_str);
      return -1;
    }
  }
  printf("[Error] Error opening file %s\n",file_str);
  return -2;
}

int load_lgl_dir(char *dir_str)
{
  int loaded_count;
  char *full_path;
  DIR *dir;
  dir = opendir(dir_str);
  struct dirent *file;
  loaded_count=0;
  if (dir)
  {
    while ((file = readdir(dir)) != NULL)
    {
      full_path=malloc(strlen(dir_str)+strlen(file->d_name)+1);
      sprintf(full_path,"%s%s",dir_str,file->d_name);
      if (load_lgl(full_path))
      {
        loaded_count++;
      }
      free(full_path);
    }
    closedir(dir);
  }
  return loaded_count;
}
//Takes an action id, gets the action's metadata
struct lgl_action_meta *get_action_meta(int id)
{
      struct lgl_action_meta *action;
      HASH_FIND_INT(action_meta_dict, &id, action);
      return action;
}

void lgl_free_all()
{
  struct lgl_action_meta *curr_action, *tmp;
  int i;
  HASH_ITER(hh, action_meta_dict, curr_action, tmp)
  {
    for (i=0;i<curr_action->arg_count;i++)
    {
      free(curr_action->arguments[i]->caption);
      free(curr_action->arguments[i]->deflt);
      free(curr_action->arguments[i]->menu);
      free(curr_action->arguments[i]);
    }
    free(curr_action->arguments);
    HASH_DEL(action_meta_dict,curr_action);  /* delete; users advances to next */
    free(curr_action);            /* optional- if you want to free  */
  }
}
