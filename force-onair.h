#ifndef __FORCE_ONAIR_H
#define __FORCE_ONAIR_H__

#include <iostream>
#include "bdaqctrl.h"
#include <signal.h>

using namespace Automation::BDaq;
using namespace std;

int main();
void BDAQCALL OnDiSnapEvent(void *sender, DiSnapEventArgs *args, void *userParam);
int checkBioError(ErrorCode ret);
void signal_handler(int signal);

#endif // __FORCE_ONAIR_H__