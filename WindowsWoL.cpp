// MIT License
// by Si Xiaolong (GitHub:@Direct5dom E-mail:sixiaolong2021@gmail.com)

#include <WinSock2.h>
#include <iostream>
#include <string>

#include "SimpleWoL.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main(int argc, char *argv[])
{
    // data[102] 存储 Magic Packet
    char data[102] = {0x00};
    // 字符指针 inputMacAddress，指向输入参数 argv[1]
    char *inputMacAddress;
    // 无符号的 macAddress[6] 字符数组存储分割好的 Mac 地址
    unsigned char macAddress[6] = {0x00};
    // 整型 result，存储参数调用结果
    int result = 0;

    if (argv[1])
    {
        // 输入参数
        inputMacAddress = argv[1];
    }
    else
    {
        // 没有输入参数 则提示输入 Mac 地址
        std::string strtmp;
        cout << "请输入 Mac 地址（以“:”或“-”分割的格式）：";
        cin >> strtmp;
        // 将输入数据存储在 strtmp 中，然后让 inputMacAddress 指向 strtmp[0]
        inputMacAddress = &strtmp[0];
    }

    // 分割输入的 Mac 地址
    result = splitMac(inputMacAddress, macAddress);
    if (result == 1)
    {
        cout << "Mac 地址格式错误" << endl;
        return 1;
    }

    // 初始化 WinSock
    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cout << "初始化WinSock失败！" << endl;
        return 1;
    }

    // 创建 Socket
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        cout << "创建 Socket 失败！" << endl;
        WSACleanup();
        return 1;
    }

    // 设置 Socket 选项
    BOOL broadcast = TRUE;
    result = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&broadcast, sizeof(BOOL));
    if (result == SOCKET_ERROR)
    {
        cout << "设置 Socket 选项失败！" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 设置目标地址
    SOCKADDR_IN addr;
    // 使用IPv4协议
    addr.sin_family = AF_INET;
    // 目标端口号为9
    addr.sin_port = htons(9);
    // 目标地址为广播地址
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    // 打包 Magic Packet
    packaging(macAddress, data);

    // 输出 Magic Packet
    printMagicPacket(data);

    // 发送数据
    result = sendto(sock, data, sizeof(data), 0, (SOCKADDR *)&addr, sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        cout << "发送数据失败！" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 发送成功 关闭 Socket，清理 WinSock
    cout << "魔法包已发送！" << endl;
    closesocket(sock);
    WSACleanup();

    return 0;
}