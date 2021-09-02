#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Thing.hpp"

class Marine: public Thing{
    public:
        static const struct thingstat stats[3];
        int type = 2;
        Marine(float x, float y, bool te){
            px = x;
            py = y;
            isfriend = te;
        };
        void draw(sf::RenderWindow &w, pair<float, float> p){
            drawig(w, px - p.first, py - p.first, isfriend);
        }
    private:
        static sf::CircleShape c;
        static void drawig(sf::RenderWindow &w, float px, float py, bool team){
            if(team) c.setFillColor(sf::Color::Blue);
            else c.setFillColor(sf::Color::Red);
            c.setPosition(px - 22.5, py - 22.5);
            w.draw(c);
        }
};

const struct thingstat Marine::stats[0] = {1000, 46, 3.15, 1, {0, 1}, {45, 55}, "Marine", {1, 0, 0, 1, 0, 0, 0, 0}, {1}, 18, 1, 0.375};
static sf::CircleShape c = sf::CircleShape(22.5);