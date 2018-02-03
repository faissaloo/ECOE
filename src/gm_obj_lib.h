enum arg_type
{
	ARG_EXPRESSION,
	ARG_STRING,
	ARG_URI,
	ARG_BOOLEAN,
	ARG_DROPDOWN,
	ARG_SPRITE,
	ARG_SOUND,
	ARG_BACKGROUND,
	ARG_PATH,
	ARG_SCRIPT,
	ARG_OBJECT,
	ARG_ROOM,
	ARG_FONT,
	ARG_COLOR,
	ARG_TIMELINE
};

enum action_kind
{
	ACT_NORMAL,
  ACT_BLOCK_START,
  ACT_BLOCK_END,
  ACT_ELSE,
  ACT_EXIT,
  ACT_REPEAT,
  ACT_VAR,
  ACT_CODE,
  ACT_PLACEHOLDER,
  ACT_SEPERATOR,
  ACT_LABEL
};

enum event_kind
{
  EV_CREATE,
  EV_DESTROY,
  EV_ALARM,
  EV_STEP,
  EV_COLLISION,
  EV_KEY,
  EV_MOUSE,
  EV_OTHER,
  EV_DRAW,
  EV_KEY_PRESS,
  EV_KEY_RELEASE
};
/*
<PhysicsShapePoints>
  <point>0,5</point>
</PhysicsShapePoints>
*/
struct gm_point
{
  int x;
  int y;
};
/*
<argument>
  <kind>0</kind>

  <string>0</string>
  or
  <background>0</background>
</argument>
*/
struct gm_argument
{
  int kind;
  char *string;
};

/*
<action>
  <libid>1</libid>
  <id>603</id>
  <kind>7</kind>
  <userelative>0</userelative>
  <useapplyto>-1</useapplyto>
  <isquestion>0</isquestion>
  <exetype>2</exetype>
  <functionname/>
  <codestring/>
  <whoName>self</whoName>
  <relative>0</relative>
  <isnot>0</isnot>
  <arguments>
    ...
  </arguments>
</action>
*/
struct gm_action
{
  int libid;
  int id;
  int kind;
  int userelative;
  int useapplyto;
  int isquestion;
  int exetype;
  char *functionname; //Pointer for a malloc request for memory for a string
  char *codestring; //Pointer for a malloc request for memory for a string
  char *whoName; //Pointer for a malloc request for memory for a string
  int relative;
  int isnot;
  int arg_count;
  struct gm_argument **arguments; //Pointer for a malloc request for memory for an array of arguments
};

struct gm_event
{
  int enumb;
  int eventtype;
  char *ename;
  int action_count;
  struct gm_action **actions; //Pointer for a malloc request for memory for an array of actions
};

struct gm_object
{
  char *spriteName; //Pointer for a malloc request for memory for a string
  int solid;
  int visible;
  int depth;
  int persistent;
  char *maskName; //Pointer for a malloc request for memory for a string
  char *parentName; //Pointer for a malloc request for memory for a string
  int event_count;
  struct gm_event **events; //Pointer for a malloc request for memory for an array of events
  //Physics crap
  int PhysicsObject;
  int PhysicsObjectSensor;
  int PhysicsObjectShape;
  float PhysicsObjectDensity;
  float PhysicsObjectRestitution;
  int PhysicsObjectGroup;
  float PhysicsObjectLinearDamping;
  float PhysicsObjectAngularDamping;
  float PhysicsObjectFriction;
  int PhysicsObjectAwake;
  int PhysicsObjectKinematic;
  int point_count;
  struct gm_point **PhysicsShapePoints; //Pointer for a malloc request for memory for an array of points
};

struct gm_object *new_object();
struct gm_object *object_from_file(char *);
void print_obj(struct gm_object *);
int save_obj(struct gm_object *, char *);
void obj_free(struct gm_object *);

struct gm_event *new_event(struct gm_object *,int);
int delete_event(struct gm_object *,int);
void event_free(struct gm_event *);

struct gm_action *new_action(struct gm_event *, int);
int move_action(struct gm_event *, int,int);
int delete_action(struct gm_event *,int);
void action_free(struct gm_action *);
