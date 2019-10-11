
        #include <cstdlib>
        long long unsigned heuristic(const state_t *from) {
            long long unsigned res = 0;
            unsigned arr[4] = {0,1,2,3};
            int x, y, x_r, y_r;
            for(int i = 0; i<4; ++i){
                x = from->vars[i] % 2;
                y = from->vars[i] / 2;
                x_r = arr[i] % 2;
                y_r = arr[i] / 2;
                res += std::abs(x - x_r) + std::abs(y - y_r);
            }
            return res;
        }
    
