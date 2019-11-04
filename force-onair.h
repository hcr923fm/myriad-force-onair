#ifndef __FORCE_ONAIR_H
#define __FORCE_ONAIR_H__

#include "myriad-controller.h"
#include "gpio-handler.h"
#include <vector>
#include <signal.h>
// #include <boost/program-options> // TODO: install program_options

vector<MyriadInstance *> myriadInstances;
int main(int argc, char *argv[]);

void onGPIDetect(int signalID);
bool checkMyriadResponseError(string myriadResponse);

#endif // __FORCE_ONAIR_H__