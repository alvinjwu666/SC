#include "Thing.hpp"

class CommandCenter: public Structures{
    public:
        static const struct thingstat stats[3];
        int type = 0;
        void cast(int typ);
        CommandCenter(float x, float y, bool te){
            px = x;
            py = y;
            isfriend = te;
        }
};

const struct thingstat CommandCenter::stats[0] = {0, 0, 0, 1, {1, 2}, {1500, 1500}, "Command Center", {0, 1, 0, 0, 1, 0, 1, 0}, {}, 71, 0, 2};