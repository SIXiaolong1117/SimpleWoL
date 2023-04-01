#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>

#include "SimpleWoL.h"

using namespace std;

int main(int argc, char *argv[])
{
    char data[102];
    unsigned char macAddress[6];
    char *inputMacAddress;

    if (argv[1])
    {
        // 输入参数
        inputMacAddress = argv[1];
    }
    else
    {
        // 提示输入参数
        std::string strtmp;
        cout << "请输入 Mac 地址（以“:”或“-”分割的格式）：";
        cin >> strtmp;
        inputMacAddress = &strtmp[0];
    }
    // 分割输入的 Mac 地址
    if (splitMac(inputMacAddress, macAddress) != true)
    {
        cout << "初始化WinSock失败！" << endl;
        return 1;
    }

    // 创建 Socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        cout << "创建 Socket 失败！" << endl;
        exit(1);
    }

    // 设置 Socket 选项
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
    {
        cout << "设置 Socket 选项失败！" << endl;
        close(sock);
        exit(1);
    }

    // 设置目标地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9);
    if (inet_aton("255.255.255.255", &addr.sin_addr) == 0)
    {
        cout << "广播地址无效！" << endl;
        close(sock);
        exit(1);
    }

    // 打包 Magic Packet
    packaging(macAddress, data);

    // 输出 Magic Packet
    printMagicPacket(data);

    // 发送数据
    int bytes_sent = sendto(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_sent < 0)
    {
        cout << "发送数据失败！" << endl;
        close(sock);
        exit(1);
    }

    // 发送成功 清理 Socket
    cout << "魔法包已发送！" << endl;
    close(sock);
    return 0;
}