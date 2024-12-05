#ifndef DATA_LOADER_H
#define DATA_LOADER_H
#include <cstring>
#include <iostream>
#include <fstream>
#include <print>
#include <unordered_map>
#include <unordered_set>

#include "nlohmann/json.hpp"
#include "pugixml.hpp"

#include "node.h"
#include "way.h"
#include "quad_tree.h"
#include "edge.h"
#include "utils.h"

using json = nlohmann::json;

bool data_init_all(char **, unsigned int);  //allow multiple files
#endif