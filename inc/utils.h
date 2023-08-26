#pragma once
#include <string>
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include "logging.h"

bool ToSizeTFromStr(const std::string& str, size_t* val);

bool ExecCmdWithOutput(std::string &cmd, std::string &output);