#include "gpio-handler.h"

int32 startPort = 0;
int32 portCount = 1;
InstantDiCtrl *instantDiCtrl;

bool setupAdvGPIO(string deviceDescription, int interruptPinID)
{
    ErrorCode ret = Success;
    instantDiCtrl = InstantDiCtrl::Create();
    instantDiCtrl->addInterruptHandler(OnDiSnapEvent, nullptr);

    DeviceCtrl *devctrl = DeviceCtrl::Create(0, (wchar_t *)deviceDescription.c_str(), ModeRead);
    Array<DeviceTreeNode> *installedDevices = devctrl->getInstalledDevices();
    for (int32 i = 0; i < installedDevices->getLength(); i++)
    {
        printf("Device number: %d; Modules index: %d; Description: %d",
               installedDevices->getItem(i).DeviceNumber, *installedDevices->getItem(i).ModulesIndex,
               *installedDevices->getItem(i).Description);
    }

    DeviceInformation devInfo((int32)1);
    ret = instantDiCtrl->setSelectedDevice(devInfo);
    checkBioError(ret);

    Array<DiintChannel> *interruptChannels = instantDiCtrl->getDiintChannels();
    if (interruptChannels == NULL)
    {
        printf("Device does not support Digital Input Interrupts!\n");
        return false;
    }

    printf("%d channels are supported; we're using #%d.\n", interruptChannels->getCount(), interruptChannels->getItem(interruptPinID).getChannel());
    ret = interruptChannels->getItem(0).setEnabled(true);
    checkBioError(ret);
    ret = interruptChannels->getItem(0).setTrigEdge(FallingEdge);
    checkBioError(ret);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    ret = instantDiCtrl->SnapStart();
    checkBioError(ret);
    while (true)
    {
        Sleep(1000);
    }
}

void BDAQCALL OnDiSnapEvent(void *sender, DiSnapEventArgs *args, void *userParam)
{
    printf("DI Interrupt channel is %d\n", args->SrcNum);
    for (int32 i = startPort; i < startPort + portCount; ++i)
    {
        printf("DI port %d status:  0x%X\n", i, args->PortData[i - startPort]);
    }
    raise(SIG_GPIDETECT);
}

void checkBioError(ErrorCode ret)
{
    if (BioFailed(ret))
    {
        printf("Error - last code was 0x%x\n", ret);
        exit(1);
    }
    else
    {
        return;
    }
}

void signal_handler(int signal)
{
    printf("Received keyboard interrupt, shutting down...\n");
    instantDiCtrl->Dispose();
    exit(1);
}