#include "chocolate.h"

bool canBreakSlices(int height, int width, int slicesAmount)
{
    return slicesAmount > 0 && slicesAmount < height * width &&
           (slicesAmount % height == 0 || slicesAmount % width == 0);
}
