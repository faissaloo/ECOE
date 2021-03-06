#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "gm_obj_lib.h"
#define max(a,b) (a)<(b)?(b):(a)

//Precomputed hashes
#define H_spriteName 48859
#define H_solid 1358
#define H_visible 5613
#define H_depth 1064
#define H_persistent 43084
#define H_maskName 9563
#define H_parentName 43147
#define H_events 2507
#define H_PhysicsObject 3062
#define H_PhysicsObjectSensor 62368
#define H_PhysicsObjectShape 31153
#define H_PhysicsObjectDensity 58093
#define H_PhysicsObjectRestitution 27172
#define H_PhysicsObjectGroup 30726
#define H_PhysicsObjectLinearDamping 31599
#define H_PhysicsObjectAngularDamping 13679
#define H_PhysicsObjectFriction 49188
#define H_PhysicsObjectAwake 30815
#define H_PhysicsObjectKinematic 32793
#define H_PhysicsShapePoints 6875
#define H_libid 1206
#define H_id 182
#define H_kind 580
#define H_userelative 32349
#define H_useapplyto 47299
#define H_isquestion 36324
#define H_exetype 4817
#define H_functionname 29195
#define H_codestring 35311
#define H_whoName 5499
#define H_relative 10589
#define H_isnot 1050
#define H_arguments 19387
#define H_string 2991
#define H_font 548
#define H_sprite 3033
#define H_sound 1316
#define H_background 34708
#define H_path 644
#define H_script 2800
#define H_object 2486
#define H_room 671
#define H_timeline 10701
#define H_x 120
#define H_y 121

//Perfect hash function for tag names
int hash_tag(char *str)
{
	int result;
	result=0;
	while (*str!='\0')
	{
		result<<=1;
		result^=*str;
		str++;
	}
	return result;
}

//intnc (int from node content)
int intnc(int *int_ptr, xmlNode *node)
{
	char *temp_xml_content_store;
	temp_xml_content_store = (char *)xmlNodeGetContent(node);
	if (temp_xml_content_store==NULL)
	{
		printf("Error reading node %s", node->name);
		return 0;
	}
	else
	{
		*int_ptr = strtol(temp_xml_content_store,0,10);
		free(temp_xml_content_store);
		return 1;
	}
}

//floatnc (float from node content)
void floatnc(float *float_ptr, xmlNode *node)
{
	char *temp_xml_content_store;
	temp_xml_content_store = (char *)xmlNodeGetContent(node);
	if (temp_xml_content_store==NULL)
	{
		printf("Error reading node %s", node->name);
	}
	else
	{
		*float_ptr = strtof(temp_xml_content_store,0);
		free(temp_xml_content_store);
	}
}

struct gm_object *new_object()
{
	struct gm_object *object_to_return = calloc(1,sizeof(struct gm_object));
	object_to_return->events = calloc(object_to_return->event_count,sizeof(struct gm_event));
	return object_to_return;
}
//Opens a .object.gmx file and turns it into a tree of structs
struct gm_object *object_from_file(char *file)
{
	//Iterator vars
	int i;
	int ii;
	int iii;

	char *curr_ename;
	char *curr_enumb;
	char *curr_eventtype;
	struct gm_object *object_to_return = (struct gm_object*)malloc(sizeof(struct gm_object));
	xmlDoc *doc = NULL;
	doc = xmlReadFile(file, NULL, 0);
	if (doc == NULL)
	{
			printf("[Error] Could not parse object GMX: %s\n", file);
			return NULL;
	}
	xmlNode *object_element = NULL;

	/*Get the second level*/
	object_element = xmlFirstElementChild(xmlDocGetRootElement(doc));
	//Loop through the top level and assign relevant values
	xmlNode *cur_node = NULL;
	for (cur_node = object_element; cur_node; cur_node = xmlNextElementSibling(cur_node))
	{
		switch (hash_tag((char *)cur_node->name))
		{
			case H_spriteName:
				object_to_return->spriteName = (char *)xmlNodeGetContent(cur_node);
				break;

			case H_solid:
				intnc(&object_to_return->solid, cur_node);
				break;

			case H_visible:
				intnc(&object_to_return->visible, cur_node);
				break;

			case H_depth:
				intnc(&object_to_return->depth, cur_node);
				break;

			case H_persistent:
				intnc(&object_to_return->persistent, cur_node);
				break;

			case H_maskName:
				object_to_return->maskName = (char *)xmlNodeGetContent(cur_node);
				break;

			case H_parentName:
				object_to_return->parentName = (char *)xmlNodeGetContent(cur_node);
				break;

			case H_events:
				object_to_return->event_count = xmlChildElementCount(cur_node);
				object_to_return->events = calloc(object_to_return->event_count,sizeof(struct gm_event));

				xmlNode *event_node = xmlFirstElementChild(cur_node);
				for (i=0;i<(object_to_return->event_count);i++)
				{
					if (event_node->type == XML_ELEMENT_NODE)
					{
						struct gm_event *curr_event = (struct gm_event*)calloc(1,sizeof(struct gm_event));
						//Put all the actions in the event before saving to the event
						curr_event->action_count = xmlChildElementCount(event_node);
						xmlNode *action_node = xmlFirstElementChild(event_node);
						curr_event->actions = calloc(curr_event->action_count,sizeof(struct gm_action));
						for (ii=0;ii<(curr_event->action_count);ii++)
						{
							if (action_node->type == XML_ELEMENT_NODE)
							{
								struct gm_action *curr_action = calloc(1,sizeof(struct gm_action));
								xmlNode *action_property_node;
								for (action_property_node=xmlFirstElementChild(action_node);
									action_property_node;
									action_property_node=xmlNextElementSibling(action_property_node))
								{
									switch (hash_tag((char *) action_property_node->name))
									{
										case H_libid:
											intnc(&curr_action->libid, action_property_node);
											break;

										case H_id:
											intnc(&curr_action->id, action_property_node);
											break;

										case H_kind:
											intnc(&curr_action->kind, action_property_node);
											break;

										case H_userelative:
											intnc(&curr_action->userelative, action_property_node);
											break;

										case H_useapplyto:
											intnc(&curr_action->useapplyto, action_property_node);
											break;

										case H_isquestion:
											intnc(&curr_action->isquestion, action_property_node);
											break;

										case H_exetype:
											intnc(&curr_action->exetype, action_property_node);
											break;

										case H_functionname:
											curr_action->functionname = (char *)xmlNodeGetContent(action_property_node);
											break;

										case H_codestring:
											curr_action->codestring = (char *)xmlNodeGetContent(action_property_node);
											break;

										case H_whoName:
											curr_action->whoName = (char *)xmlNodeGetContent(action_property_node);
											break;

										case H_relative:
											intnc(&curr_action->relative,action_property_node);
											break;

										case H_isnot:
											intnc(&curr_action->isnot,action_property_node);
											break;

										case H_arguments:
											//arguments for the action
											curr_action->arg_count = xmlChildElementCount(action_property_node);
											xmlNode *arg_node = xmlFirstElementChild(action_property_node);
											curr_action->arguments = calloc(curr_action->arg_count, sizeof(struct gm_argument));
											for (iii=0;iii<(curr_action->arg_count);iii++)
											{
												struct gm_argument *curr_arg = calloc(1,sizeof(struct gm_argument));
												xmlNode *arg_property_node;
												for (arg_property_node=xmlFirstElementChild(arg_node);arg_property_node;arg_property_node=xmlNextElementSibling(arg_property_node))
												{
													switch (hash_tag((char *) arg_property_node->name))
													{
														case H_kind:
															intnc(&curr_arg->kind, arg_property_node);
															break;

														case H_string:
														case H_font:
														case H_sprite:
														case H_sound:
														case H_background:
														case H_path:
														case H_script:
														case H_object:
														case H_room:
														case H_timeline:
															curr_arg->string = (char *)xmlNodeGetContent(arg_property_node);
															break;
													}
												}
												arg_node = xmlNextElementSibling(arg_node);
												curr_action->arguments[iii]=curr_arg;
											}
											break;
									}
									curr_event->actions[ii]=curr_action;
								}
							}

							action_node = xmlNextElementSibling(action_node); //Move to the next node
						}

						curr_ename = (char *)xmlGetProp(event_node, "ename");
						curr_enumb = (char *)xmlGetProp(event_node, "enumb");
						if (curr_enumb!=NULL)
						{
							//enumb
							curr_event->enumb = strtol(curr_enumb,0,10);
						}
						else if (curr_ename!=NULL)
						{
							//enumb
							curr_event->ename = curr_ename;
						}
						curr_eventtype = (char *)xmlGetProp(event_node, "eventtype");
						//event type
						curr_event->eventtype = strtol(curr_eventtype,0,10);
						free(curr_eventtype);
						free(curr_enumb);

						object_to_return->events[i]=curr_event;
					}
					event_node = xmlNextElementSibling(event_node); //Move to the next node
				}
				break;

			case H_PhysicsObject:
				intnc(&object_to_return->PhysicsObject, cur_node);
				break;

			case H_PhysicsObjectSensor:
				intnc(&object_to_return->PhysicsObjectSensor, cur_node);
				break;

			case H_PhysicsObjectShape:
				intnc(&object_to_return->PhysicsObjectShape, cur_node);
				break;

			case H_PhysicsObjectDensity:
				floatnc(&object_to_return->PhysicsObjectDensity, cur_node);
				break;

			case H_PhysicsObjectRestitution:
				floatnc(&object_to_return->PhysicsObjectRestitution, cur_node);
				break;

			case H_PhysicsObjectGroup:
				intnc(&object_to_return->PhysicsObjectGroup, cur_node);
				break;

			case H_PhysicsObjectLinearDamping:
				floatnc(&object_to_return->PhysicsObjectLinearDamping, cur_node);
				break;

			case H_PhysicsObjectAngularDamping:
				floatnc(&object_to_return->PhysicsObjectAngularDamping, cur_node);
				break;

			case H_PhysicsObjectFriction:
				floatnc(&object_to_return->PhysicsObjectFriction, cur_node);
				break;

			case H_PhysicsObjectAwake:
				intnc(&object_to_return->PhysicsObjectAwake, cur_node);
				break;

			case H_PhysicsObjectKinematic:
				intnc(&object_to_return->PhysicsObjectKinematic, cur_node);
				break;

			case H_PhysicsShapePoints:
				//Parse points
				object_to_return->point_count = xmlChildElementCount(cur_node);
				object_to_return->PhysicsShapePoints = malloc(sizeof(struct gm_point*)*object_to_return->point_count);
				xmlNode *point_node = xmlFirstElementChild(cur_node);
				xmlChar *string_to_convert;
				for (i=0;i<object_to_return->point_count;i++)
				{
						struct gm_point *curr_point = (struct gm_point*)calloc(1,sizeof(struct gm_point));
						char *axis_str; //Either holds the x or the y
						string_to_convert=xmlNodeGetContent(point_node);
						curr_point->x = strtol(string_to_convert,&axis_str,10);
						curr_point->y = strtol(++axis_str,NULL,10);
						free(string_to_convert);
						point_node = xmlNextElementSibling(point_node);
						object_to_return->PhysicsShapePoints[i]=curr_point;
				}
				break;
		}
	}
	/*free the document */
	xmlFreeDoc(doc);
	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();
	//After we're done
	return object_to_return;
}

//Print the contents of a GM obj
void print_obj(struct gm_object *obj)
{
	printf("Sprite name: %s\n",obj->spriteName);
	printf("Solid: %i\n",obj->solid);
	printf("Visible: %i\n",obj->visible);
	printf("Depth: %i\n",obj->depth);
	printf("Persistent: %i\n",obj->persistent);
	printf("Mask name: %s\n",obj->maskName);
	printf("Parent name: %s\n",obj->parentName);
	printf("Number of events: %i\n",obj->event_count);
	int i;
	int ii;
	int iii;
	for (i=0;i<obj->event_count;i++)
	{
		printf("\tEvent number: %i\n",obj->events[i]->enumb);
		printf("\tEvent type: %i\n",obj->events[i]->eventtype);
		printf("\tNumber of actions: %i\n", obj->events[i]->action_count);
		for (ii=0;ii<obj->events[i]->action_count;ii++)
		{
			printf("\t\tLibrary ID: %i\n",obj->events[i]->actions[ii]->libid);
			printf("\t\tAction ID: %i\n",obj->events[i]->actions[ii]->id);
			printf("\t\tKind: %i\n",obj->events[i]->actions[ii]->kind);
			printf("\t\tUse relative: %i\n",obj->events[i]->actions[ii]->userelative);
			printf("\t\tApply to: %i\n",obj->events[i]->actions[ii]->useapplyto);
			printf("\t\tIs question: %i\n",obj->events[i]->actions[ii]->isquestion);
			printf("\t\tExecution type: %i\n",obj->events[i]->actions[ii]->exetype);
			printf("\t\tFunction name: %s\n",obj->events[i]->actions[ii]->functionname);
			printf("\t\tCode string: %s\n",obj->events[i]->actions[ii]->codestring);
			printf("\t\tWho: %s\n",obj->events[i]->actions[ii]->whoName);
			printf("\t\tRelative: %i\n",obj->events[i]->actions[ii]->relative);
			printf("\t\tNot: %i\n",obj->events[i]->actions[ii]->isnot);
			printf("\t\tNumber of arguments: %i\n", obj->events[i]->actions[ii]->arg_count);
			for (iii=0;iii<obj->events[i]->actions[ii]->arg_count;iii++)
			{
				printf("\t\t\tKind: %i\n",obj->events[i]->actions[ii]->arguments[iii]->kind);
				//printf("\t\t\tString: %s\n",obj->events[i]->actions[ii]->arguments[iii]->string);
			}
		}
	}
	printf("PhysicsObject: %i\n",obj->PhysicsObject);
	printf("PhysicsObjectSensor: %i\n",obj->PhysicsObjectSensor);
	printf("PhysicsObjectShape: %i\n",obj->PhysicsObjectShape);
	printf("PhysicsObjectDensity: %f\n",obj->PhysicsObjectDensity);
	printf("PhysicsObjectRestitution: %f\n",obj->PhysicsObjectRestitution);
	printf("PhysicsObjectGroup: %i\n",obj->PhysicsObjectGroup);
	printf("PhysicsObjectLinearDamping: %f\n",obj->PhysicsObjectLinearDamping);
	printf("PhysicsObjectAngularDamping: %f\n",obj->PhysicsObjectAngularDamping);
	printf("PhysicsObjectFriction: %f\n",obj->PhysicsObjectFriction);
	printf("PhysicsObjectAwake: %i\n",obj->PhysicsObjectAwake);
	printf("PhysicsObjectKinematic: %i\n",obj->PhysicsObjectKinematic);
	printf("Number of points: %i\n", obj->point_count);
	for (i=0;i<obj->point_count;i++)
	{
		printf("\tPoint: %i,%i\n",obj->PhysicsShapePoints[i]->x,obj->PhysicsShapePoints[i]->y);
	}
}

//Save an obiect as in xml/gm format
int save_obj(struct gm_object *obj, char *file_str)
{
	int i;
	xmlTextWriterPtr writer;
	// Create a new XmlWriter for uri, with no compression.
	writer = xmlNewTextWriterFilename(file_str, 0);
	if (writer == NULL)
	{
		return 1; //1 = error
	}
	xmlTextWriterStartDocument(writer, NULL, "UTF-16", "no");
	//Root
	xmlTextWriterStartElement(writer, BAD_CAST "object");
	xmlTextWriterWriteElement(writer, BAD_CAST "spriteName", BAD_CAST obj->spriteName);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "solid","%i",obj->solid);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "visible","%i",obj->visible);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "depth","%i",obj->depth);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "persistent","%i",obj->persistent);
	xmlTextWriterWriteElement(writer, BAD_CAST "maskName", BAD_CAST obj->maskName);
	xmlTextWriterWriteElement(writer, BAD_CAST "parentName", BAD_CAST obj->parentName);
	xmlTextWriterStartElement(writer, BAD_CAST "events");
	for (i = 0; i<obj->event_count;i++)
	{
		xmlTextWriterStartElement(writer, BAD_CAST "event");
		xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "eventtype","%i",obj->events[i]->eventtype);
		if (obj->events[i]->ename)
		{
			xmlTextWriterWriteAttribute(writer, BAD_CAST "ename",obj->events[i]->ename);
		}
		else
		{
			xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "enumb","%i",obj->events[i]->enumb);
		}
		int ii;
		for (ii = 0; ii<obj->events[i]->action_count;ii++)
		{
			xmlTextWriterStartElement(writer, BAD_CAST "action");
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "libid", "%i", obj->events[i]->actions[ii]->libid);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "id","%i", obj->events[i]->actions[ii]->id);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "kind","%i", obj->events[i]->actions[ii]->kind);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "userelative","%i", obj->events[i]->actions[ii]->userelative);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "useapplyto", "%i", obj->events[i]->actions[ii]->useapplyto);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "isquestion", "%i", obj->events[i]->actions[ii]->isquestion);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "exetype","%i", obj->events[i]->actions[ii]->exetype);
			xmlTextWriterWriteElement(writer, BAD_CAST "functionname", BAD_CAST obj->events[i]->actions[ii]->functionname);
			xmlTextWriterWriteElement(writer, BAD_CAST "codestring", BAD_CAST obj->events[i]->actions[ii]->codestring);
			xmlTextWriterWriteElement(writer, BAD_CAST "whoName", BAD_CAST obj->events[i]->actions[ii]->whoName);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "relative","%i", obj->events[i]->actions[ii]->relative);
			xmlTextWriterWriteFormatElement(writer, BAD_CAST "isnot","%i", obj->events[i]->actions[ii]->isnot);
			xmlTextWriterStartElement(writer, BAD_CAST "arguments");
			int iii;
			for (iii = 0; iii<obj->events[i]->actions[ii]->arg_count;iii++)
			{
				xmlTextWriterStartElement(writer, BAD_CAST "argument");
				xmlTextWriterWriteFormatElement(writer, BAD_CAST "kind", "%i", obj->events[i]->actions[ii]->arguments[iii]->kind);
				switch (obj->events[i]->actions[ii]->arguments[iii]->kind)
				{
					case ARG_SPRITE:
						xmlTextWriterWriteElement(writer, BAD_CAST "sprite", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_SOUND:
						xmlTextWriterWriteElement(writer, BAD_CAST "sound", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_BACKGROUND:
						xmlTextWriterWriteElement(writer, BAD_CAST "background", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_PATH:
						xmlTextWriterWriteElement(writer, BAD_CAST "path", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_SCRIPT:
						xmlTextWriterWriteElement(writer, BAD_CAST "script", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_OBJECT:
						xmlTextWriterWriteElement(writer, BAD_CAST "object", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_ROOM:
						xmlTextWriterWriteElement(writer, BAD_CAST "room", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_FONT:
						xmlTextWriterWriteElement(writer, BAD_CAST "font", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					case ARG_TIMELINE:
						xmlTextWriterWriteElement(writer, BAD_CAST "timeline", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
					default:
						xmlTextWriterWriteElement(writer, BAD_CAST "string", obj->events[i]->actions[ii]->arguments[iii]->string);
						break;
				}
				xmlTextWriterEndElement(writer); //Close <argument>
			}
			xmlTextWriterEndElement(writer); //Close <arguments>
			xmlTextWriterEndElement(writer); //Close <action>
		}
		xmlTextWriterEndElement(writer); //Close <event>
	}
	xmlTextWriterEndElement(writer); //Close <events>
	//Put physics stuff here
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObject","%i", obj->PhysicsObject);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectSensor","%i", obj->PhysicsObjectSensor);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectShape","%i", obj->PhysicsObjectShape);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectDensity","%f", obj->PhysicsObjectDensity);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectRestitution","%f", obj->PhysicsObjectRestitution);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectGroup","%i", obj->PhysicsObjectGroup);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectLinearDamping","%f", obj->PhysicsObjectLinearDamping);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectAngularDamping","%f", obj->PhysicsObjectAngularDamping);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectFriction","%f", obj->PhysicsObjectFriction);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectAwake","%i", obj->PhysicsObjectAwake);
	xmlTextWriterWriteFormatElement(writer, BAD_CAST "PhysicsObjectKinematic","%i", obj->PhysicsObjectKinematic);
	xmlTextWriterStartElement(writer, BAD_CAST "PhysicsShapePoints");
	for (i=0;i<obj->point_count;i++)
	{
		xmlTextWriterWriteFormatElement(writer, BAD_CAST "point","%i,%i", obj->PhysicsShapePoints[i]->x, obj->PhysicsShapePoints[i]->y);
	}
	xmlTextWriterEndElement(writer); //Close <PhysicsShapePoints>
	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	return 1;
}

void action_free(struct gm_action *action)
{
	int i;
	free(action->functionname);
	free(action->codestring);
	free(action->whoName);
	//Args
	for (i=0;i<action->arg_count-1;i++)
	{
		free(action->arguments[i]->string);
		free(action->arguments[i]);
	}
	free(action->arguments);
	free(action);
}

void event_free(struct gm_event *event)
{
	int i;
	//Actions
	for (i=0;i<event->action_count-1;i++)
	{
		action_free(event->actions[i]);
	}
	free(event->ename);
	free(event->actions);
	free(event);
}
//Frees the memory allocated to a gm_object
void obj_free(struct gm_object *obj)
{
	free(obj->spriteName);
	free(obj->maskName);
	free(obj->parentName);
	int i;
	//Events
	for (i=0;i<obj->event_count-1;i++)
	{
		event_free(obj->events[i]);
	}
	free(obj->events);
	//Points
	for (i=0;i<obj->point_count;i++)
	{
		free(obj->PhysicsShapePoints[i]);
	}
	free(obj->PhysicsShapePoints);
	free(obj);
}
//Creates a blank event in an object
struct gm_event *new_event(struct gm_object *obj,int eventtype)
{
	struct gm_event *event;
	obj->events=realloc(obj->events,sizeof(struct gm_event)*(obj->event_count+1)); //I think this is returning null...
	event=calloc(1,sizeof(struct gm_event));
	event->eventtype=eventtype;
	obj->events[obj->event_count++]=event;
	return event;
}

int delete_event(struct gm_object *obj,int where)
{
	if (where<obj->event_count)
	{
		event_free(obj->events[where]);
		memmove(&obj->events[where], &obj->events[where+1], sizeof(struct gm_event)*((obj->event_count--)-where));
		return 1;
	}
	return 0;
}

//Creates a blank action in an event
struct gm_action *new_action(struct gm_event *event, int where)
{
	//WE'RE GETTING NULLS AGAIN!!!
	struct gm_action *action;
	event->actions=realloc(event->actions,sizeof(struct gm_action)*(event->action_count+1));
	memmove(&event->actions[where+1], &event->actions[where], sizeof(struct gm_action)*((event->action_count++)-where));
	action=calloc(1,sizeof(struct gm_action));
	event->actions[where++]=action;
	return action;
}

//Moves an action
int move_action(struct gm_event *event, int source,int dest)
{
	if ((source<event->action_count)&(source>=0)&(dest<event->action_count)&(dest>=0))
	{
		struct gm_action *action;
		//Copy the source action
		action=event->actions[source];
		//Remove the space for the action
		memmove(&event->actions[source], &event->actions[source+1], sizeof(struct gm_action*)*((event->action_count)-source));
		//Create space at the destination
		memmove(&event->actions[dest+1], &event->actions[dest], sizeof(struct gm_action*)*((event->action_count)-dest));
		//Put it in the destination
		event->actions[dest]=action;
		return 1;
	}
	else
	{
		return 0;
	}
}

int delete_action(struct gm_event *event,int where)
{
	if (where<event->action_count)
	{
		action_free(event->actions[where]);
		memmove(&event->actions[where], &event->actions[where+1], sizeof(struct gm_action*)*((event->action_count--)-where));
		return 1;
	}
	return 0;
}
