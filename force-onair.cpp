#include "force-onair.h"

int main(int argc, char *argv[])
{
    /**
     * USAGE:
     *     force-myriad-onair <ADVANTECH-CARD-ID> <IP-OF-MACHINE-TO-FORCE> <IPS-OF-MACHINES-TO-RELEASE>
     *     force-myriad-onair "USB-4750, BID#0" 0 192.168.0.4 192.168.0.2 192.168.0.7
     * would use the Advantech USB4750 with device ID 0 and interrupt pin 0 to force PC with
     * IP 192.168.0.4 on air and force 192.168.0.2 and 192.168.0.7 to release control.
     * */

    for (int i = 3; i < argc; i++)
    {
        MyriadInstance *m = new MyriadInstance(argv[i], TCP);
        printf("Connecting to %s...\n", argv[i]);
        bool connect_success = m->connectInstance();
        if (!connect_success)
        {
            printf("Could not connect to %s!\n", argv[i]);
            return 1;
        }
        myriadInstances.push_back(m);
    }

    signal(SIG_GPIDETECT, onGPIDetect);

    setupAdvGPIO(argv[1], atoi(argv[2]));
}

void onGPIDetect(int signalID)
{
    printf("Detected GPI state change!\n");
    printf("Sending ONAIR RELEASE commands to other Myriad Instances...");

    string reply;
    for (unsigned int i = 1; i < myriadInstances.size(); i++)
    {
        reply = myriadInstances[i]->sendCommand("ONAIR RELEASE");
        if (checkMyriadResponseError(reply))
        {
            printf("Failed to release Control of %s: %s", myriadInstances[i]->getIPAddress(), reply);
            exit(1);
        }
    }

    printf("Taking Control for On-Air PC...");
    reply = myriadInstances[0]->sendCommand("ONAIR TAKE");
    if (checkMyriadResponseError(reply))
    {
        printf("Failed to take Control: %s", reply);
        return;
    }

    printf("Synchronizing Log for On-Air PC...");
    reply = myriadInstances[0]->sendCommand("LOG MODE STANDBY");
    if (checkMyriadResponseError(reply))
    {
        printf("Failed to set Log mode to STANDBY: %s", reply);
        return;
    }

    reply = myriadInstances[0]->sendCommand("LOG HOME");
    if (checkMyriadResponseError(reply))
    {
        printf("Failed to jump Log to HOME: %s", reply);
        return;
    }

    reply = myriadInstances[0]->sendCommand("LOG MODE HOURMODE");
    if (checkMyriadResponseError(reply))
    {
        printf("Failed to set Log mode to HOURMODE: %s", reply);
        return;
    }

    reply = myriadInstances[0]->sendCommand("LOG GO");
    if (checkMyriadResponseError(reply))
    {
        printf("Failed to execute GO: %s", reply);
        return;
    }

    printf("Successfully set On-Air PC");
}

bool checkMyriadResponseError(string myriadResponse)
{
    if (myriadResponse.find("Success") == string::npos)
    {
        return false;
    }
    else
    {
        return true;
    }
}