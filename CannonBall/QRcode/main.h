#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "pugixml.hpp"

class Variables {

/* Some weird code to obtain a string of an emum element*/
std::vector<std::string> split(const std::string &text, char sep) {
	std::vector<std::string> tokens;
	int start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

#define ENUM(name, ...)\
enum name \
{\
__VA_ARGS__\
};\
std::vector<std::string> name##Map = split(#__VA_ARGS__, ',');\
    std::string toString(const name v) { return name##Map.at(v);}

/*
* This type enum is for retrieving the variable names and avoid mispelling error.
* Be sure to add all the variables from the main_config.xml file to this enum type
* However it is let to the programer the possibility of not using the config table 
* and direclty accessing to the variable trough the pugi::xml_document object
*/

ENUM(Variable, 
	DisableMicroController,
	DisableMicroControllerCheck,
	DisableFps,
	DisableMosquitto,
	DisableVideoServer,
	DisableVideoScreen,

	//Optimisation variables
	min_qrcode_distance,
	dictionary,
	marker_size,
	camera,
	video_capturer,
	serial, 	//Micro controller port

	//Mosquitto variables
	host,
	port,

	);




	public:
		struct cmp_str
		{
			bool operator()(char const *a, char const *b)
			{
				return std::strcmp(a, b) < 0;
			}
		};
		static std::map<char*, char*, cmp_str> config;
		static int* tmp;
		static char* tmpChar;

	static int getValueAsInt(char* key) {
		std::map<char*, char*>::iterator it = config.find(key);
		if (it != config.end())
			return atoi(it->second);
		return 0;
	}

	static float getValueAsFloat(char* key) {
		std::map<char*, char*>::iterator it = config.find(key);
		if (it != config.end())
			return atof(it->second);
		return 0;
	}

	static char* getValueAsString(char* key) {
		std::map<char*,char*>::iterator it = config.find(key);
		if (it != config.end())
			return it->second;
		return NULL;
	}

		
	static void Variables::fillConfig(std::map<char*, char*, cmp_str>* table, pugi::xml_document* doc) {
		pugi::xml_node run = doc->child("Run");
		const pugi::char_t* mode = run.attribute("mode").value();

		//int argc = std::distance(doc.child("Arguments").find_child_by_attribute("mode", mode).children.begin(), doc.child("Arguments").find_child_by_attribute("mode", mode).children.end());
		//char *argv[15]; //argc should be < 15
	
		//Reading the optimisations
		pugi::xml_node optimisations = doc->child("Optimisations");
		for (pugi::xml_node option = optimisations.first_child(); option; option = option.next_sibling())
		{
			table->insert(std::pair<char*, char*>((char*)option.name(), (char*)option.attribute("value").as_string()));
			std::cout << table->find((char*)option.name())->first << " " << table->find((char*)option.name())->second << std::endl;
		}

		//Reading the arguments
		pugi::xml_node args = doc->child("Arguments").find_child_by_attribute("mode", mode);
		for (pugi::xml_node option = args.first_child(); option; option = option.next_sibling())
		{
			for (pugi::xml_attribute_iterator it = option.attributes_begin(); it != option.attributes_end(); ++it)
			{
				char* attr_name = (char*)it->name(); char* attr_value = (char*)it->as_string();
				table->insert(std::pair<char*, char*>(attr_name, attr_value));
			
				std::cout << attr_name << " " << attr_value << std::endl;
			}
		}
	}

};

//An example how to use this magic trick
//int main(int c, char**v)
//{
//	std::cout << toString(Red) << toString(Blue);
//	return 0;
//}