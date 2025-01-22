#ifndef LOGGER_H_DEFINED
#define LOGGER_H_DEFINED
#include <stdio.h>
#include <string.h>
// #############################################################################
//                           Logging
// this code has been copied from: https://github.com/aneury1/CelesteClone/blob/master/src/schnitzel_lib.h
// please check because is a fork of another project..
// #############################################################################
enum TextColor
{  
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};

template <typename ...Args>
void log(const char* prefix,const char* msg, TextColor textColor, Args... args)
{
  static const char* TextColorTable[TEXT_COLOR_COUNT] = 
  {    
    "\x1b[30m", // TEXT_COLOR_BLACK
    "\x1b[31m", // TEXT_COLOR_RED
    "\x1b[32m", // TEXT_COLOR_GREEN
    "\x1b[33m", // TEXT_COLOR_YELLOW
    "\x1b[34m", // TEXT_COLOR_BLUE
    "\x1b[35m", // TEXT_COLOR_MAGENTA
    "\x1b[36m", // TEXT_COLOR_CYAN
    "\x1b[37m", // TEXT_COLOR_WHITE
    "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
    "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
    "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
    "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
    "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
    "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
    "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
    "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
  };

  char formatBuffer[8192] = {};
  sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

  char textBuffer[8912] = {};
  sprintf(textBuffer, formatBuffer, args...);

  puts(textBuffer);
}
#include <sstream>
#define SM_TRACE(msg, ...) log("TRACE: ", msg, TEXT_COLOR_GREEN, ##__VA_ARGS__);
#define SM_WARN(msg, ...) log("WARN: ", msg, TEXT_COLOR_YELLOW, ##__VA_ARGS__);
#define SM_ERROR(msg, ...) log("ERROR: ", msg, TEXT_COLOR_RED, ##__VA_ARGS__);
#define SM_INFO(msg, ...) log("ERROR: ", msg, TEXT_COLOR_MAGENTA, ##__VA_ARGS__);

#if 0
#define SM_INFO(msg, ...) \
std::stringstream stream; \
stream <<__FILE__<<":"<<__LINE__<<"=" << msg; \
log("ERROR: ", (char *)stream.str().c_str(), TEXT_COLOR_MAGENTA, ##__VA_ARGS__);
#endif
#define SM_ASSERT(x, msg, ...)    \
{                                 \
  if(!(x))                        \
  {                               \
    SM_ERROR(msg, ##__VA_ARGS__); \
    DEBUG_BREAK();                \
    SM_ERROR("Assertion HIT!")    \
  }                               \
}



#endif