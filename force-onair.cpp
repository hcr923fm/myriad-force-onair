#include "force-onair.h"

int32 startPort = 0;
int32 portCount = 1;
InstantDiCtrl *instantDiCtrl;

int main()
{
    ErrorCode ret = Success;
    instantDiCtrl = InstantDiCtrl::Create();
    instantDiCtrl->addInterruptHandler(OnDiSnapEvent, nullptr);

    DeviceCtrl *devctrl = DeviceCtrl::Create(0,L"DemoDevice",ModeRead);
    Array<DeviceTreeNode> *installedDevices = devctrl->getInstalledDevices();
    for (int32 i = 0; i < installedDevices->getLength(); i++)
    {
        printf("Device number: %ls; Modules index: %ls; Description: %ws",
               installedDevices->getItem(i).DeviceNumber, installedDevices->getItem(i).ModulesIndex,
               installedDevices->getItem(i).Description);
    }

    DeviceInformation devInfo((int32)1);
    ret = instantDiCtrl->setSelectedDevice(devInfo);
    checkBioError(ret);
    // ret = instantDiCtrl->LoadProfile(L"C:/Advantech/DAQNavi/Examples/profile/DemoDevice.xml");
    // checkBioError(ret);

    Array<DiintChannel> *interruptChannels = instantDiCtrl->getDiintChannels();
    if (interruptChannels == NULL)
    {
        printf("Device does not support Digital Input Interrupts!\n");
        return 2;
    }

    printf("%d channels are supported; we're using #%d.", interruptChannels->getCount(), interruptChannels->getItem(0).getChannel());

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
        printf("DI port %d status:  0x%X\n\n", i, args->PortData[i - startPort]);
    }
}

int checkBioError(ErrorCode ret)
{
    if (BioFailed(ret))
    {
        printf("Error - last code was 0x%x", ret);
        exit(1);
    }
    else
    {
        return 0;
    }
}

void signal_handler(int signal)
{
    instantDiCtrl->Dispose();
    exit(1);
}