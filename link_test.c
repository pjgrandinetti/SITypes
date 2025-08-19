#include "OCTypes/OCTypes.h"

int main() {
    OCStringRef str = OCStringMakeConstantString("test");
    OCAutorelease(str);
    return 0;
}
