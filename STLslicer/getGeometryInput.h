#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "vertex.h"

int getGeometryInput(std::string STL_filename, std::vector<Triangle>& facet);
int read_binary_STL_file(std::string STL_filename, std::vector<Triangle>& facet);

int read_ascii_STL_file(std::string STL_filename, std::vector<Triangle>& facet);