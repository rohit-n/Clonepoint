#include <vector>
#include "tinyxml/tinyxml.h"

#define FIRST_GUARD_GID 1025
#define FIRST_PROFESSIONAL_GID 1093
#define FIRST_ENFORCER_GID 1113

typedef struct
{
	int ent;
	int x;
	int y;
} entity_s;

std::vector<entity_s> linkable_ents;
std::vector<entity_s> circuit_masks;

char* file_read(const char* filename)
{
	FILE* input = fopen(filename, "rb");

	if(input == NULL) return NULL;

	if(fseek(input, 0, SEEK_END) == -1) return NULL;
	long size = ftell(input);
	if(size == -1) return NULL;
	if(fseek(input, 0, SEEK_SET) == -1) return NULL;

	char* content = new char[(size_t)size+1];
	if(content == NULL)
		return NULL;

	fread(content, 1, (size_t)size, input);
	if(ferror(input))
	{
		delete [] content;
		return NULL;
	}

	fclose(input);
	content[size] = '\0';
	return content;
}

TiXmlElement* get_linkable_info(int ent, int x, int y)
{
	TiXmlElement* entity = new TiXmlElement("object");
	TiXmlElement* property_list = NULL;
	TiXmlElement* property = NULL;
	switch (ent)
	{
		case 53:
			entity->SetAttribute("type", "Door");
			entity->SetAttribute("gid", 1069);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 88);
			break;
		case 54:
			entity->SetAttribute("type", "Switch");
			entity->SetAttribute("gid", 1088);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "handscanner");
			property->SetAttribute("value", "yes");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			break;
		case 55:
			entity->SetAttribute("type", "Door");
			entity->SetAttribute("gid", 1070);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 88);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "open");
			property->SetAttribute("value", "yes");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			break;
		case 56:
			entity->SetAttribute("type", "Switch");
			entity->SetAttribute("gid", 1072);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			break;
		case 69:
			entity->SetAttribute("type", "LightFixture"); //right facing camera
			entity->SetAttribute("gid", 1089);
			entity->SetAttribute("x", x - 48); //FIXME: Unsure about this
			entity->SetAttribute("y", y + 48);
			break;
		case 114:
			entity->SetAttribute("type", "SecurityCamera"); //right facing camera
			entity->SetAttribute("gid", 1078);
			entity->SetAttribute("x", x - 48); //FIXME: Unsure about this
			entity->SetAttribute("y", y + 80);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "direction");
			property->SetAttribute("value", "right");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			break;
		case 116:
			entity->SetAttribute("type", "Alarm");
			entity->SetAttribute("gid", 1085);
			entity->SetAttribute("x", x - 72);
			entity->SetAttribute("y", y + 56);
			break;
		case 131:
			entity->SetAttribute("type", "Socket");
			entity->SetAttribute("gid", 1073);
			entity->SetAttribute("x", x - 48); //FIXME: Unsure about this
			entity->SetAttribute("y", y + 64);
		break;
		case 132:
			entity->SetAttribute("type", "VaultDoor");
			entity->SetAttribute("gid", 1079);
			entity->SetAttribute("x", x - 64); //FIXME: Unsure about this
			entity->SetAttribute("y", y + 48);
			break;
		case 150:
			entity->SetAttribute("type", "SecurityCamera"); //right facing camera
			entity->SetAttribute("gid", 1077);
			entity->SetAttribute("x", x - 72); //FIXME: Unsure about this
			entity->SetAttribute("y", y + 80);
			break;
		case 294:
			entity->SetAttribute("type", "Scanner");
			entity->SetAttribute("gid", 1074);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 88);
			break;
		case 412:
			entity->SetAttribute("type", "TrapDoor");
			entity->SetAttribute("gid", 1075);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 70);
			break;
		case 608:
			entity->SetAttribute("type", "Elevator");
			entity->SetAttribute("x", x - 104);
			entity->SetAttribute("y", y + 72);
			entity->SetAttribute("gid", 1049);
			break;
	}

	return entity;
}

void add_linkable_ents(TiXmlElement* red_group, TiXmlElement* blue_group,
TiXmlElement* green_group, TiXmlElement* yellow_group, TiXmlElement* violet_group)
{
	size_t i, j;
	bool is_red = true;
	for (i = 0; i < linkable_ents.size(); i++)
	{
		TiXmlElement* entity = get_linkable_info(linkable_ents[i].ent, linkable_ents[i].x, linkable_ents[i].y);
		for (j = 0; j < circuit_masks.size(); j++)
		{
			if (linkable_ents[i].x == circuit_masks[j].x && 
				linkable_ents[i].y == circuit_masks[j].y)
			{
				is_red = false;
				switch(circuit_masks[j].ent)
				{
					case 104:
						blue_group->LinkEndChild(entity);
					break;
					case 112:
						yellow_group->LinkEndChild(entity);
					break;
					case 113:
						green_group->LinkEndChild(entity);
					break;
					case 284:
						violet_group->LinkEndChild(entity);
					break;
					default:
					break;
				}
				break;
			}
		}

		if (is_red)
		{
			red_group->LinkEndChild(entity);
		}
		else
		{
			is_red = true;
		}
	}
}

void parse_map_entity(int ent, int x, int y, char* string, TiXmlElement* collision_group, TiXmlElement* entity_group,
TiXmlElement* player_group, TiXmlElement* objective_group, TiXmlElement* prop_group)
{
	TiXmlElement* entity = new TiXmlElement("object");
	TiXmlElement* property_list = NULL;
	TiXmlElement* property = NULL;
	TiXmlElement* selected_group = NULL;

	entity->SetAttribute("x", x);
	entity->SetAttribute("y", y);

	/* http://gunpointwiki.net/wiki/Level_editor */
	switch (ent)
	{
		case 6:
			entity->SetAttribute("type", "Guard");
			entity->SetAttribute("gid", FIRST_GUARD_GID);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "direction");
			property->SetAttribute("value", "right");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			selected_group = entity_group;
		break;
		case 53:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 54:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 55:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 56:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 69:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 70:
			entity->SetAttribute("width", 16);
			entity->SetAttribute("height", 128);
			selected_group = collision_group;
		break;
		case 72:
			//FIXME: Position X needs offset
			entity->SetAttribute("type", "Terminal");
			entity->SetAttribute("gid", 1046);
			entity->SetAttribute("x", x - 56);
			entity->SetAttribute("y", y + 80);
			selected_group = objective_group;
		break;
		case 76:
			entity->SetAttribute("type", "Guard");
			entity->SetAttribute("gid", "2147484673");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			selected_group = entity_group;
		break;
		case 104:
			circuit_masks.push_back((entity_s){ent, x, y});
		break;
		case 107:
			entity->SetAttribute("type", "CircuitBoxB");
			entity->SetAttribute("gid", "1053");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 80);
			selected_group = entity_group;
		break;
		case 110:
			entity->SetAttribute("type", "CircuitBoxG");
			entity->SetAttribute("gid", "1054");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 80);
			selected_group = entity_group;
		break;
		case 111:
			entity->SetAttribute("type", "CircuitBoxY");
			entity->SetAttribute("gid", "1056");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 80);
			selected_group = entity_group;
		break;
		case 112:
			circuit_masks.push_back((entity_s){ent, x, y});
		break;
		case 113:
			circuit_masks.push_back((entity_s){ent, x, y});
		break;
		case 114:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 116:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 119:
			entity->SetAttribute("width", 4); /* FIXME: Unsure about this */
			entity->SetAttribute("height", 128);
			entity->SetAttribute("type", "Glass");
			entity->SetAttribute("x", x - 4);
			entity->SetAttribute("y", y - 32);
			selected_group = collision_group;
		break;
		case 123:
			entity->SetAttribute("width", 4);
			entity->SetAttribute("height", 128);
			entity->SetAttribute("type", "Glass");
			entity->SetAttribute("y", y - 32);
			selected_group = collision_group;
		break;
		case 131:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 132:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 136:
			entity->SetAttribute("width", 512);
			entity->SetAttribute("height", 32);
			selected_group = collision_group;
		break;
		case 150:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 166:
			entity->SetAttribute("type", "Professional");
			entity->SetAttribute("gid", FIRST_PROFESSIONAL_GID);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "direction");
			property->SetAttribute("value", "right");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			selected_group = entity_group;
		break;
		case 169:
			entity->SetAttribute("type", "Enforcer");
			entity->SetAttribute("gid", FIRST_ENFORCER_GID);
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 64);
			property_list = new TiXmlElement("properties");
			property = new TiXmlElement("property");
			property->SetAttribute("name", "direction");
			property->SetAttribute("value", "right");
			property_list->LinkEndChild(property);
			entity->LinkEndChild(property_list);
			selected_group = entity_group;
		break;
		case 211:
			entity->SetAttribute("width", 128);
			entity->SetAttribute("height", 4);
			entity->SetAttribute("type", "Glass");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y);
			selected_group = collision_group;
		break;
		case 214: /* door frame */
			entity->SetAttribute("width", 16);
			entity->SetAttribute("height", 80);
			selected_group = collision_group;
		break;
		case 215:
			entity->SetAttribute("type", "Subway");
			entity->SetAttribute("gid", "1048");
			entity->SetAttribute("y", y + 32);
			selected_group = objective_group;
		break;
		case 248:
			entity->SetAttribute("type", "Couch");
			entity->SetAttribute("gid", 1060);
			entity->SetAttribute("x", x - 56); //FIXME: Unsure
			entity->SetAttribute("y", y + 84);
			selected_group = prop_group;
		break;
		case 250:
			entity->SetAttribute("type", "Plant1");
			entity->SetAttribute("gid", 1061);
			entity->SetAttribute("x", x - 56);
			entity->SetAttribute("y", y + 104);
			selected_group = prop_group;
		break;
		case 257:
			entity->SetAttribute("width", 768);
			entity->SetAttribute("height", 32);
			selected_group = collision_group;
		break;
		case 270:
			entity->SetAttribute("type", "Plant2");
			entity->SetAttribute("gid", 1062);
			entity->SetAttribute("x", x - 56);
			entity->SetAttribute("y", y + 88);
			selected_group = prop_group;
		break;
		case 272:
			entity->SetAttribute("type", "Shelf");
			entity->SetAttribute("gid", 1058);
			entity->SetAttribute("x", x - 56);
			entity->SetAttribute("y", y + 112);
			selected_group = prop_group;
		break;
		case 282:
			entity->SetAttribute("width", 128);
			entity->SetAttribute("height", 32);
			selected_group = collision_group;
		break;
		case 283:
			entity->SetAttribute("type", "CircuitBoxV");
			entity->SetAttribute("gid", "1055");
			entity->SetAttribute("x", x - 64);
			entity->SetAttribute("y", y + 80);
			selected_group = entity_group;
		break;
		case 284:
			circuit_masks.push_back((entity_s){ent, x, y});
		break;
		case 294:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 412:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 489:
			entity->SetAttribute("width", 256);
			entity->SetAttribute("height", 256);
			selected_group = collision_group;
		break;
		case 606:
			entity->SetAttribute("type", "Player");
			selected_group = player_group;
		break;
		case 608:
			linkable_ents.push_back((entity_s){ent, x, y});
		break;
		case 610:
			entity->SetAttribute("type", "Stairs");
			selected_group = entity_group;
			entity->SetAttribute("x", x - 68);
			entity->SetAttribute("y", y + 76);
			entity->SetAttribute("gid", 1045);
		break;
		default:
		// printf("Warning: Unable to handle entity %i\n", ent);
		break;
	}

	if (selected_group)
	{
		selected_group->LinkEndChild(entity);
	}
	else
	{
		delete entity;
	}
}

void make_clonepoint_map(char* lvl_string, char* filename)
{
	char* delim = (char*)"\r\n";
	char* token = strtok(lvl_string, delim);
	char ent[8];
	char posX[8];
	char posY[8];

	if (!token)
	{
		printf("Error, couldn't parse level.\n");
		return;
	}

	if (strcmp(token, "1"))
	{
		printf("Level doesn't start with 1.\n");
		return;
	}
	size_t i;
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );

	TiXmlElement* map_property_list = new TiXmlElement("properties");
	TiXmlElement* map_property_bgy = new TiXmlElement("property");
	map_property_bgy->SetAttribute("name", "backgroundyoff");
	map_property_bgy->SetAttribute("value", "60");
	map_property_list->LinkEndChild(map_property_bgy);

	TiXmlElement* mapElement = new TiXmlElement("map");
	TiXmlElement* tile1 = new TiXmlElement("tileset");
	TiXmlElement* tile1_image = new TiXmlElement("image");

	TiXmlElement* spriteSets[5];
	TiXmlElement* spriteImages[5];

	TiXmlElement* tile_layer = new TiXmlElement("layer");
	TiXmlElement* tile_data = new TiXmlElement("data");
	tile_layer->SetAttribute("name", "Tile Layer 1");
	tile_layer->SetAttribute("width", "128");
	tile_layer->SetAttribute("height", "76");
	tile_data->SetAttribute("encoding", "csv");
	tile_layer->LinkEndChild(tile_data);

	/* Entity groups */
	TiXmlElement* collision_group = new TiXmlElement("objectgroup");
	collision_group->SetAttribute("name", "Collision");

	TiXmlElement* red_group = new TiXmlElement("objectgroup");
	red_group->SetAttribute("name", "RedLinkable");
	red_group->SetAttribute("color", "#ff0000");

	TiXmlElement* green_group = new TiXmlElement("objectgroup");
	green_group->SetAttribute("name", "GreenLinkable");
	green_group->SetAttribute("color", "#00ff00");
	
	TiXmlElement* blue_group = new TiXmlElement("objectgroup");
	blue_group->SetAttribute("name", "BlueLinkable");
	blue_group->SetAttribute("color", "#0000ff");
	
	TiXmlElement* yellow_group = new TiXmlElement("objectgroup");
	yellow_group->SetAttribute("name", "YellowLinkable");
	yellow_group->SetAttribute("color", "#0000ff");
	
	TiXmlElement* violet_group = new TiXmlElement("objectgroup");
	violet_group->SetAttribute("name", "VioletLinkable");
	violet_group->SetAttribute("color", "#0000ff");

	TiXmlElement* link_group = new TiXmlElement("objectgroup");
	link_group->SetAttribute("name", "Links");
	link_group->SetAttribute("color", "#008080");

	TiXmlElement* light_link_group = new TiXmlElement("objectgroup");
	light_link_group->SetAttribute("name", "LightLinks");

	TiXmlElement* prop_group = new TiXmlElement("objectgroup");
	prop_group->SetAttribute("name", "Props");

	TiXmlElement* entity_group = new TiXmlElement("objectgroup");
	entity_group->SetAttribute("name", "Entities");

	TiXmlElement* player_group = new TiXmlElement("objectgroup");
	player_group->SetAttribute("name", "Player");

	TiXmlElement* objective_group = new TiXmlElement("objectgroup");
	objective_group->SetAttribute("name", "Objectives");

	for (i = 0; i < 5; i++)
	{
		spriteSets[i] = new TiXmlElement("tileset");
		spriteImages[i] = new TiXmlElement("image");
		spriteSets[i]->SetAttribute("tilewidth", "128");
		spriteSets[i]->SetAttribute("tileheight", "128");
		spriteImages[i]->SetAttribute("trans", "ff00ff");
		spriteSets[i]->LinkEndChild(spriteImages[i]);
	}

	mapElement->SetAttribute("version", "1.0");
	mapElement->SetAttribute("orientation", "orthogonal");
	mapElement->SetAttribute("width", "128");
	mapElement->SetAttribute("height", "76");
	mapElement->SetAttribute("tilewidth", "16");
	mapElement->SetAttribute("tileheight", "16");

	tile1->SetAttribute("firstgid", "1");
	tile1->SetAttribute("name", "set01");
	tile1->SetAttribute("tilewidth", "16");
	tile1->SetAttribute("tileheight", "16");

	tile1_image->SetAttribute("source", "tilesets/tile1.png");
	tile1_image->SetAttribute("width", "512");
	tile1_image->SetAttribute("height", "512");

	i = 0;

	spriteSets[i]->SetAttribute("firstgid", FIRST_GUARD_GID);
	spriteSets[i]->SetAttribute("name", "GuardSet");

	spriteImages[i]->SetAttribute("source", "sprites/guard.png");
	spriteImages[i]->SetAttribute("width", "512");
	spriteImages[i]->SetAttribute("height", "640");

	i++;

	spriteSets[i]->SetAttribute("firstgid", "1045");
	spriteSets[i]->SetAttribute("name", "objects");

	spriteImages[i]->SetAttribute("source", "sprites/objects.png");
	spriteImages[i]->SetAttribute("width", "512");
	spriteImages[i]->SetAttribute("height", "768");

	i++;

	spriteSets[i]->SetAttribute("firstgid", "1069");
	spriteSets[i]->SetAttribute("name", "linkables");

	spriteImages[i]->SetAttribute("source", "sprites/linkable.png");
	spriteImages[i]->SetAttribute("width", "512");
	spriteImages[i]->SetAttribute("height", "768");

	i++;

	spriteSets[i]->SetAttribute("firstgid", FIRST_PROFESSIONAL_GID);
	spriteSets[i]->SetAttribute("name", "ProSet");

	spriteImages[i]->SetAttribute("source", "sprites/professional.png");
	spriteImages[i]->SetAttribute("width", "512");
	spriteImages[i]->SetAttribute("height", "640");

	i++;

	spriteSets[i]->SetAttribute("firstgid", FIRST_ENFORCER_GID);
	spriteSets[i]->SetAttribute("name", "EnfSet");

	spriteImages[i]->SetAttribute("source", "sprites/enforcer.png");
	spriteImages[i]->SetAttribute("width", "512");
	spriteImages[i]->SetAttribute("height", "640");

	tile1->LinkEndChild(tile1_image);

	mapElement->LinkEndChild(map_property_list);
	mapElement->LinkEndChild(tile1);

	for (i = 0; i < 5; i++)
	{
		mapElement->LinkEndChild(spriteSets[i]);
	}

	mapElement->LinkEndChild(tile_layer);

	mapElement->LinkEndChild(collision_group);
	mapElement->LinkEndChild(red_group);
	mapElement->LinkEndChild(green_group);
	mapElement->LinkEndChild(blue_group);
	mapElement->LinkEndChild(yellow_group);
	mapElement->LinkEndChild(violet_group);
	mapElement->LinkEndChild(link_group);
	mapElement->LinkEndChild(light_link_group);
	mapElement->LinkEndChild(prop_group);
	mapElement->LinkEndChild(entity_group);
	mapElement->LinkEndChild(player_group);
	mapElement->LinkEndChild(objective_group);

	token = strtok(NULL, delim);

	while (token)
	{
		strcpy(ent, token);
		token = strtok(NULL, delim);
		strcpy(posX, token);
		token = strtok(NULL, delim);
		strcpy(posY, token);

		parse_map_entity(atoi(ent), atoi(posX), atoi(posY), lvl_string, collision_group, entity_group, player_group, objective_group, prop_group);

		if (token)
			token = strtok(NULL, delim);
	}

	add_linkable_ents(red_group, blue_group, green_group, yellow_group, violet_group);
	doc.LinkEndChild(decl);
	doc.LinkEndChild(mapElement);
	doc.SaveFile(filename);
}

int main(int argc, char** argv)
{
	char* lvl_string;
	if (argc < 3)
	{
		printf("usage: gun2clone input.lvl output.tmx\n");
		return 1;
	}

	lvl_string = file_read(argv[1]);
	if (!lvl_string)
	{
		printf("Couldn't load %s!\n", argv[1]);
		return 1;
	}

	make_clonepoint_map(lvl_string, argv[2]);

	free(lvl_string);
	return 0;
}