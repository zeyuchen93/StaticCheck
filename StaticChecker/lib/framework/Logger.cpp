#include "framework/Logger.h"

TLogLevel Logger::configurationLevel = LOG_DEBUG_3;
TCheck Logger::checkType = ReturnStackAddr_Checker;
bool Logger::options[2] = {false,false};
//bool Logger::options[6] = {false, false, false, false, false, false};
void Logger::configure(Config &c) {
  auto block = c.getOptionBlock("PrintLog");
  int level = atoi(block.find("level")->second.c_str());
  switch (level) {
  case 1:
    configurationLevel = LOG_DEBUG_1;
    break;
  case 2:
    configurationLevel = LOG_DEBUG_2;
    break;
  case 3:
    configurationLevel = LOG_DEBUG_3;
    break;
  case 4:
    configurationLevel = LOG_DEBUG_4;
    break;
  case 5:
    configurationLevel = LOG_DEBUG_5;
    break;
  default:
    configurationLevel = LOG_DEBUG_3;
  }

  if (block.find("ReturnStackAddrChecker")->second == "true")
    options[ReturnStackAddr_Checker] = true;
  if (block.find("Checker5")->second == "true")
    options[Checker_5] = true;
}
