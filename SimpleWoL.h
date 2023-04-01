#include <iostream>

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

// 输出 Magic Packet
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