
        #include <cstdlib>
        long long unsigned heuristic(const state_t *from) {
            long long unsigned res = 0;
            static unsigned arr[25] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
            int x, y, x_r, y_r;
            for(int i = 0; i<25; ++i){
                x = from->vars[i] % 5;
                y = from->vars[i] / 5;
                x_r = arr[i] % 5;
                y_r = arr[i] / 5;
                res += std::abs(x - x_r) + std::abs(y - y_r);
            }
            return res;
        }
    
