#include <assert.h>
#include <stdio.h>

int main() {
    printf("Before assert\n");
    assert(0);  // 💥 program stops here
    printf("After assert\n");  // ❌ never executed
    return 0;
}
