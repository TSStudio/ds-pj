#ifndef DATA_LOADER_H
#define DATA_LOADER_H
#include <iostream>
#include <fstream>
#include "pugixml.hpp"
#include <unordered_map>
#include <unordered_set>
#include "node.h"
#include "way.h"
#include "quad_tree.h"
#include "edge.h"
#include "cstring"
#include "utils.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

bool data_init_all(char **, unsigned int);  //allow multiple files
#endif