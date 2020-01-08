#include "force-onair.h"

/**
 * This should:
 * - Setup connections to Myriad clients - DONE
 * - Wait for GPI triggers to force Myriad on air (in new thread) - DONE
 * - Keep track of which Myriad instance is on air
 * - Poll Myriad for Now Playing data (in new thread, should now playing data be a conditional variable?)
 * - Provide REST interface for Now Playing data (in new thread)
 */

int main(int argc, char *argv[])
{
    /**
     * USAGE:
     *     force-myriad-onair <ADVANTECH-CARD-ID> <IP-OF-MACHINE-TO-FORCE> <IPS-OF-MACHINES-TO-RELEASE>
     *     force-myriad-onair "USB-4750, BID#0" 0 192.168.0.4 192.168.0.2 192.168.0.7
     * would use the Advantech USB4750 with device ID 0 and interrupt pin 0 to force PC with
     * IP 192.168.0.4 on air and force 192.168.0.2 and 192.168.0.7 to release control.
     * */

    parse_options(argv[1]);

    for (int i = 2; i < argc; i++)
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

    liveMyriadInstance = "";

#ifdef MYR_ENABLE_GPIO
    signal(SIG_GPIDETECT, triggerOnAirChange);

    // GPIWaiter = thread(setupAdvGPIO, argv[1], atoi(argv[2]));
    GPIWaiter = thread(triggerOnAirChange, 0);
    GPIWaiter.join();
#endif
    // TESTING ONLY
    // triggerOnAirChange(0);
}

void triggerOnAirChange(int signalID)
{
    printf("Sending ONAIR RELEASE commands to other Myriad Instances...\n");
    myriadInstances[0]->takeCommsLock();
    myriadInstances[0]->sendCommand("AUDIOWALL PLAY 1,1");
    // Sleep(5000);
    myriadInstances[0]->releaseCommsLock();
    Sleep(5000);
    return;

    string reply;
    for (unsigned int i = 1; i < myriadInstances.size(); i++)
    {
        myriadInstances[i]->takeCommsLock();
        reply = myriadInstances[i]->sendCommand("ONAIR RELEASE");
        if (checkMyriadResponseError(reply))
        {
            printf("Failed to release Control of %s: %s", myriadInstances[i]->getIPAddress().c_str(), reply.c_str());
            exit(1);
        }
        myriadInstances[i]->releaseCommsLock();
    }

    try
    {
        printf("Taking Control for On-Air PC...");
        reply = myriadInstances[0]->sendCommand("ONAIR TAKE");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to take Control: " + reply).c_str());
        }

        printf("Dumping currently playing track...");
        reply = myriadInstances[0]->sendCommand("LOG DUMPCURRENT");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to dump current track: " + reply).c_str());
        }

        printf("Synchronizing Log for On-Air PC...");
        reply = myriadInstances[0]->sendCommand("LOG MODE STANDBY");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to set Log mode to STANDBY: " + reply).c_str());
        }

        reply = myriadInstances[0]->sendCommand("LOG VIEW HOME");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to jump Log to HOME: " + reply).c_str());
        }

        printf("Setting news hardware line if appropriate...");
        reply = myriadInstances[0]->sendCommand("IF (<PEP_TIME_MM><=2) HARDWARE SET 17");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to set hardware line: " + reply).c_str());
        }

        reply = myriadInstances[0]->sendCommand("IF (<PEP_TIME_MM>>2) HARDWARE CLEAR 17");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to clear hardware line: " + reply).c_str());
        }

        printf("Setting hour mode...");
        reply = myriadInstances[0]->sendCommand("LOG MODE HOURMODE");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to set Log mode to HOURMODE: " + reply).c_str());
        }

        printf("Executing GO if appropriate...");
        reply = myriadInstances[0]->sendCommand("IF (<PEP_TIME_MM>>2) LOG GO");
        if (checkMyriadResponseError(reply))
        {
            throw new exception(("Failed to execute GO: " + reply).c_str());
        }

        printf("Successfully set On-Air PC");
        myriadInstances[0]->releaseCommsLock();
        liveMyriadInstance = myriadInstances[0]->getIPAddress();
    }
    catch (const exception &e)
    {
        printf("Error: %s", e.what());
        myriadInstances[0]->releaseCommsLock();
        return;
    }
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