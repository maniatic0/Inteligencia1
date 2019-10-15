
#include <cstdlib>
long long unsigned heuristic(const state_t *from) {
    long long unsigned res = 0;
    int x, y, x_r, y_r;
    for(int i = 0; i <9; ++i){
        if (from->vars[i] == 0) {
            continue;
        }
        x = from->vars[i] % 3;
        y = from->vars[i] / 3;
        x_r = i % 3;
        y_r = i / 3;
        res += std::abs(x - x_r) + std::abs(y - y_r);
    }
    return res;
}
    
