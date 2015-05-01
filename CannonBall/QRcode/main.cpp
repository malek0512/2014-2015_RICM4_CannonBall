
#include "car.h"
#include "test_arduino.h"
#include "simulator.h"

#include "pugixml.hpp"
#include "main.h"
#include <iostream>
#include <map>

	std::map<char*, char*, Variables::cmp_str> Variables::config;

int main(int argc, char *argv[]) 
{
	pugi::xml_document doc;
	pugi::xml_parse_result res;
	if ( ! (res = doc.load_file("main_config.xml"))) {
		std::cout << "error loading main_config.xml : "<< res.description() << std::endl;
		return -1;
	}

	pugi::xml_node run = doc.child("Run");
	char* mode = (char*) run.attribute("mode").value();
	
	Variables::fillConfig(&Variables::config, &doc);

	//char* mode = (char*)doc->child("Run").attribute("mode").as_string();
	//pugi::xpath_node args = doc->child("Arguments").find_child_by_attribute("mode", mode);
	//pugi::xpath_node opts = doc->child("Optimisations");

	//cout << args.node().child("Aruco").attribute("dictionary").as_string() << endl;

	//pugi::xpath_node_set tools = doc->select_nodes("Mosquitto[@href='http://www.google.com' and @DeriveCaptionFrom='lastparam']");
	//pugi::xpath_query(".");

	//for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it) {
	//	pugi::xpath_node node = *it;
	//	std::cout << "Attribute Href: " << node.node().attribute("href").value() << endl;
	//	std::cout << "Value: " << node.node().child_value() << endl;
	//	std::cout << "Name: " << node.node().name() << endl;

	//}



	if (!strcmp(mode, "Rabbit") || !strcmp(mode, "Sheep") || !strcmp(mode, "Cannon") || !strcmp(mode, "Map")) {
		(new car())->main(&doc);
	}
	else if (!strcmp(mode, "Simulator")) {
		//(new simulator())->main(&doc);
	}

}