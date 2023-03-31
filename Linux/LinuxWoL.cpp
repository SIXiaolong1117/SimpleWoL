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
bool sendMagicPacket(int sock, char data[102])
{
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

    int bytes_sent = sendto(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_sent < 0)
    {
        cout << "发送数据失败！" << endl;
        close(sock);
        exit(1);
    }

    cout << "魔法包已发送！" << endl;
    close(sock);
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

    // 打包 Magic Packet
    packaging(macAddress, data);
    // 发送数据
    sendMagicPacket(sock, data);
    return 0;
}