#include <maLibrairie.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int a;
    printf("a before: %d",a);
    incremente(&a);
    printf("a after increment : %d", a);

    return 0;
}
