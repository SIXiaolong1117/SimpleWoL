// MIT License
// by Si Xiaolong (GitHub:@Direct5dom E-mail:sixiaolong2021@gmail.com)

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

// 分割 Mac 地址
int splitMac(char *input, unsigned char macAddress1[6])
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
        // Mac 地址格式错误
        return 1;
    }
    char *p;
    p = strtok(input, delimiters);
    for (int i = 0; i < 6; i++)
    {
        // 将 char p 以 hex 保存在 macAddress 中
        sscanf(p, "%hhx", &macAddress1[i]);
        p = strtok(NULL, delimiters);
    }
    return 0;
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
}

//
void printMagicPacket(char data[102])
{
    printf("Magic Packet：");
    unsigned char udata[102];
    for (int i = 0; i < 102; i++)
    {
        udata[i] = data[i];
        printf("%X ", udata[i]);
    }
    printf("\n");
}

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