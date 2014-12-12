#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include <iostream>
#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>

#include "lyutils.h"

using namespace std;

void gen_oil(ofstream& out, astree* root, int depth);

#endif
