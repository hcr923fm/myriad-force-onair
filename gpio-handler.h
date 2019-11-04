#ifndef __GPIO_HANDLER_H__
#define __GPIO_HANDLER_H__

#include <iostream>
#include "bdaqctrl.h"
#include <signal.h>

using namespace Automation::BDaq;
using namespace std;

constexpr int SIG_GPIDETECT = 0xf0000000;

bool setupAdvGPIO(string deviceDescription, int interruptPinID);
void BDAQCALL OnDiSnapEvent(void *sender, DiSnapEventArgs *args, void *userParam);
void checkBioError(ErrorCode ret);
void signal_handler(int signal);

#endif //__GPIO_HANDLER_H__