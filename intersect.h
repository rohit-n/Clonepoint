#include "global.h"
#include "vec.h"

bool vec2InRect(vec2f v, Rect r);
bool vec2InRect2(vec2f v, Rect r);
bool check_collision(Rect A, Rect B);
bool check_collision2(Rect A, Rect B);
bool rectInCircle(Rect r, vec2f center, float radius);