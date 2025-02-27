#include <stdio.h>
int main() {
    printf("你好\n请输入字符串：");
    char temp[100];
    scanf("%s", temp);
    printf("你输入的字符串是：%s\n", temp);
    printf("你好\n请再次输入字符串：");
    scanf("%s", temp);
    printf("你输入的字符串是：%s\n", temp);
}