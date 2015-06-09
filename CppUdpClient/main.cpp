#include "UdpClient.h"
#include <Windows.h>

UdpClient gUdpClient;

int main(void)
{
    while(1) {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            break;
        }
        if (GetAsyncKeyState(VK_UP)) {
            gUdpClient.sendMessageToServer("PPH");
        }
        if (GetAsyncKeyState(VK_DOWN)) {
            gUdpClient.sendMessageToServer("PPL");
        }

    }
    return 0;
}