// MIT License
// by Si Xiaolong (GitHub:@Direct5dom E-mail:sixiaolong2021@gmail.com)

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

// 分割 Mac 地址
bool splitMac(char *input, unsigned char macAddress1[6])
{
    // 输入的 Mac 地址
    cout << "输入的 Mac 地址：" << input << endl;
    char *delimiters;
    // 判断输入 Mac 格式
    if (input[2] == ':' && input[5] == ':' && input[8] == ':' && input[11] == ':' && input[14] == ':')
    {
        delimiters = ":";
    }
    else if (input[2] == '-' && input[5] == '-' && input[8] == '-' && input[11] == '-' && input[14] == '-')
    {
        delimiters = "-";
    }
    else
    {
        cout << "Mac 地址格式错误" << endl;
        return false;
    }
    char *p;
    p = strtok(input, delimiters);
    for (int i = 0; i < 6; i++)
    {
        // 将 char p 以 hex 保存在 macAddress 中
        sscanf(p, "%hhx", &macAddress1[i]);
        p = strtok(NULL, delimiters);
    }
    return true;
}

// 数据包打包
void packaging(unsigned char macAddress1[6], char data1[102])
{
    // 添加前导码 6 个 0xFF
    for (int i = 0; i < 6; i++)
    {
        data1[i] = 0xFF;
    }

    // 填充重复 16 次的 Mac 地址
    for (int i = 6; i < 102; i = i + 6)
    {
        data1[i] = macAddress1[0];
        data1[i + 1] = macAddress1[1];
        data1[i + 2] = macAddress1[2];
        data1[i + 3] = macAddress1[3];
        data1[i + 4] = macAddress1[4];
        data1[i + 5] = macAddress1[5];
    }

    // 输出 Magic Packet
    printf("Magic Packet：");
    unsigned char udata[102];
    for (int i = 0; i < 102; i++)
    {
        udata[i] = data1[i];
        printf("%x ", udata[i]);
    }
    printf("\n");
}

// 发送 Magic Packet
bool sendMagicPacket(SOCKET sock, char data[102])
{
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    int result = sendto(sock, data, sizeof(data), 0, (SOCKADDR *)&addr, sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        cout << "发送数据失败！" << endl;
        closesocket(sock);
        WSACleanup();
        return false;
    }
    cout << "魔法包已发送！" << endl;
    closesocket(sock);
    WSACleanup();
    return true;
}

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
        // 中断退出
        return 1;
    }

    // 初始化 WinSock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
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

    // 打包 Magic Packet
    packaging(macAddress, data);
    // 发送数据
    sendMagicPacket(sock, data);
    return 0;
}