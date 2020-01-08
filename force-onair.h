#ifndef __FORCE_ONAIR_H
#define __FORCE_ONAIR_H__

#include "myriad-controller.h"
#include "parse_opts.h"
#ifdef MYR_ENABLE_GPIO
#include "gpio-handler.h"
#endif
#include <vector>
#include <signal.h>
#include <thread>
#include <boost/asio.hpp>

vector<MyriadInstance *> myriadInstances;
int main(int argc, char *argv[]);
void triggerOnAirChange(int signalID);
string liveMyriadInstance;
#ifdef MYR_ENABLE_GPIO
thread GPIWaiter;
#endif

bool checkMyriadResponseError(string myriadResponse);

#endif // __FORCE_ONAIR_H__