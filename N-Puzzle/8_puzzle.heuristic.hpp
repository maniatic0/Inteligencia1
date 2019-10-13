
        #include <cstdlib>
        long long unsigned heuristic(const state_t *from) {
            long long unsigned res = 0;
            static unsigned arr[9] = {0,1,2,3,4,5,6,7,8};
            int x, y, x_r, y_r;
            for(int i = 0; i<9; ++i){
                x = from->vars[i] % 3;
                y = from->vars[i] / 3;
                x_r = arr[i] % 3;
                y_r = arr[i] / 3;
                res += std::abs(x - x_r) + std::abs(y - y_r);
            }
            return res;
        }
    
