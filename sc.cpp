#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
#include "Game.hpp"
#include "math.h"
#include <list>
#include <bits/stdc++.h>

using namespace std;
int width, height;
sf::RenderWindow window;
Game g;
list<fakeAction> fa;
int stx, sty;


void drawbg(){
    window.clear();
    sf::RectangleShape rect(sf::Vector2f(57, 57));
    stx = (int) g.camera.first / 60;
    sty = (int) g.camera.second / 60;
    rect.setOutlineThickness(3);
    rect.setOutlineColor(sf::Color(200, 200, 200, 200));
    for(int a = 0; a < width / 60 + 2; a ++){
        if(stx + a < 0){
            a = -stx - 1;
            continue;
        }
        if(stx + a >= 60) break;
        for(int b = 0; b < height / 60 + 2; b ++){
            if(sty + b >= 140) break;
            if(sty + b < 0){
                b = -sty - 1;
                continue;
            }
            if(g.terrain[stx + a][sty + b] >= 0){
                rect.setFillColor(sf::Color(0, 120 + g.terrain[stx + a][sty + b], 0));
                rect.setPosition(a * 60 + stx * 60 - g.camera.first, b * 60 + sty * 60 - g.camera.second);
                window.draw(rect);
            }else{
                if(g.terrain[stx + a][sty + b] == -1){
                    rect.setFillColor(sf::Color(60, 0, 0));
                    rect.setPosition(a * 60 + stx * 60 - g.camera.first, b * 60 + sty * 60 - g.camera.second);
                    window.draw(rect);
                }
            }
        }
    }
    sf::CircleShape cir(25);
    cir.setFillColor(sf::Color::Red);
    for(int a = 0; a < 15; a ++){
        for(int b = 0; b < 4; b ++){
            if(g.hasExit[a][b]){
                cir.setPosition(sf::Vector2f((g.exits[a][b].px) * 60 - g.camera.first - 25, (g.exits[a][b].py) * 60 - g.camera.second - 25));
                window.draw(cir);
            }
        }
    }
    vector<sf::Vertex> v;
    
    for(auto it = fa.begin(); it != fa.end(); ++ it){
        v.push_back(sf::Vertex(sf::Vector2f(it->px * 60 - g.camera.first, it->py * 60 - g.camera.second)));
    }
    window.draw(v.data(), v.size(), sf::LineStrip);
    //cout << sizeof v;
}

void draw(){
    drawbg();
    for(auto t: g.stuff){
        if(abs(t->px - stx - width / 120) < width / 120 && abs(t->py - sty - height / 120) < height / 120){
            t->draw(window, g.camera);
        }
    }
    window.display();
};
void input(){
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed)
            window.close();
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
    }
    sf::Vector2i mp = sf::Mouse::getPosition();
    int mx = mp.x, my = mp.y;
    if(mx < 120){
        g.camera.first -= (120.0 - mx) / 10.0;
    }else if(mx > width - 120){
        g.camera.first += (mx - width + 120.0) / 10.0;
    }
    if(my < 120){
        g.camera.second -= (120.0 - my) / 10.0;
    }else if(my > height - 120){
        g.camera.second += (my - height + 120.0) / 10.0;
    }
    
};
void update(){
    stx = (int) g.camera.first / 60;
    sty = (int) g.camera.second / 60;

    g.update();
};


int main(){
    window.create(sf::VideoMode::getFullscreenModes()[0], "My window", sf::Style::Fullscreen);
    width = window.getSize().x;
    height = window.getSize().y;
    window.setFramerateLimit(60);
    g = Game();
    Marine m = Marine(20, 10, 1);
    g.stuff.insert(&m);
    cout << m.px <<"\n";
    fa = {fakeAction(20.5, 1.5, 1), fakeAction(51.5, 112.5, 1)};
    g.findPathWhole(fa);
    while (window.isOpen()){
        update();
        input();
        draw();
    }
    return 0;
}