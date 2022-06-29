#include <foo.h>

int main(int argc, char const *argv[])
{
    int a = 3;
    printf("%d * 5 =",a);
    cinq(&a);
    printf("%d\n", a);
    return 0;
}
