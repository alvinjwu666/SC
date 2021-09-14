#include <list>
#include <vector>
#include <unordered_set>
#include <cstring>
#include "math.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace std;


template <typename T,typename U>
std::pair<T,U> operator+(const std::pair<T,U> & l,const std::pair<T,U> & r) {
    return {l.first+r.first,l.second+r.second};
}
template <typename T,typename U>
pair<T, U> operator -(const pair<T, U> &l, const pair<T, U> &r){
    return {l.first - r.first, l.second - r.second};
}
template <typename T,typename U>
double dot(pair<T, U> p1, pair<T, U> p2){
    return p1.first * p2.first + p1.second * p2.second;
}
double magsq(pair<double, double> p1){
    return dot(p1, p1);
}




const int FRAME = 60;
const int tagnum = 8;

struct dmg{
    int damage[2];
    double range;
    bool targets[3];
    int bonus, bonusd;
    char name[20];
    bool autoenemy;
};

struct thingstat{
    double acc, lacc;
    double top;
    bool collide;
    int armor[2];
    int maxhealth[2];
    char name[20];
    bool tag[8];//light, armored, massive, bio, mech, psonic, structure, heroic
    vector<int> damtype;//types of attacks
    int buildtime;
    int cargo;
    double rad;
};

class Thing{
    public:
        static const int tagn = 10, statusn = 8;
        static const thingstat stats[];
        static const dmg damages[5];
        const bool isUnit = false;
        double s;
        pair <double, double> v;
        double px, py;
        vector<int> group;
        int health;
        int erg, dur;//energy and duration
        int state, curact;//current stat type etc, and the current action like walking, shooting, etc
        //curact: 0 idle, 1 move, 2 chase, 3 fire
        int btype;//big type: 0 struct, 1 ground, 2 air
        //list<fakeAction> alis;//actions linked list
        vector<int> dtypes;//damage types, index of the damage which is to be in the array
        vector<int> reload, relo;
        int status[statusn];//stimmed, armor reduced, immobile, stunned, slowed, no damage or cancel, stunned cancelable
        bool seen;
        bool cloak, isfriend, canselect;
        int type;
        Thing *targ;//shoots this
        virtual void draw(sf::RenderWindow &w, pair<double, double> p);

        void statusact(){
            for(int i = 0; i < statusn; i ++){
                status[i] --;
            }
        }
        int priority(int typ);
        void cast(int typ);
        void spell(int typ);
        Thing(){}
        void fire(Thing* other);
        void firedon(dmg d);
        void abilitydmg(int d);
        void update();
};


struct fakeAction{
    double px, py;
    double dist;
    Thing *targ;
    fakeAction(double x, double y, double dist = 1): px(x), py(y), dist(dist){}
    fakeAction(){}
    fakeAction(fakeAction* o): px(o->px), py(o->py), dist(o->dist){}
};

struct actio{
    int type;//0 for normal move, 1 attack move, 2 for hold, 3 build, 4 patrol, 5 collect, 6 for stop
    list<fakeAction> ac;
    actio(list<fakeAction> f, int t = 0): type(t), ac(f){}
    actio(){}
    actio(actio* a): type(a->type), ac(a->ac){}
};

class Structures: public Thing{
    public:
        list<fakeAction> rally, build, alis;
};

class Player{
    public:
        bool isfriend;
        int upgrades[3];
};

void regularize(pair<double, double> *fp, double mag = 1){
    double tot = sqrt(pow(fp->first, 2) + pow(fp->second, 2));
    fp->first /= tot / mag;
    fp->second /= tot / mag;
}
double distp(pair<double, double> p){
    return sqrt(pow(p.first, 2) + pow(p.second, 2));
}



class Unit: public Thing{
    public:
        const bool isUnit = true;
        list<actio> alis;
        void assignMove(list<actio> a){
            alis = a;
        }
        void queueMove(list<actio> a){
            alis.splice(alis.end(), a);
        }
        void update(unordered_set<Thing*> se){
            if(!alis.empty()){
                actio ac = alis.front();
                if(!ac.ac.empty()){
                    fakeAction fa = ac.ac.front();
                    if(s < stats[state].top){
                        s += stats[state].acc / FRAME;
                    }
                    curact = 1;

                }
            }

            if(curact == 1){
                if(s < stats[state].top){
                    s += stats[state].acc / FRAME;
                }
                regularize(&v, s / FRAME);
                px += v.first;
                py += v.second;
            }
        }
};

class Ground: public Unit{
    public:
        
};


/*actio * findPath(Thing* th, actio* to){
    
}*/

class SCV: public Ground{
    public:
        static const struct thingstat stats[1];
        int type = 1;
        SCV(double x, double y, bool te){
            px = x;
            py = y;
            isfriend = te;
        };
};

const struct thingstat SCV::stats[1] = {{3.5, 46, 3.94, 1, {0, 1}, {45, 45}, "SCV", {1, 0, 0, 1, 1, 0, 0, 0}, {0}, 12, 1, 0.375}};


class Marine: public Ground{
    public:
        static const struct thingstat stats[1];
        int type = 2;
        Marine(double x, double y, bool te){
            px = x;
            py = y;
            isfriend = te;
        };
        void draw(sf::RenderWindow &w, pair<double, double> p){
            drawig(w, px * 60 - p.first, py * 60 - p.second, isfriend);
        }
    private:
        static sf::CircleShape c;
        static void drawig(sf::RenderWindow &w, double px, double py, bool team){
            if(team) c.setFillColor(sf::Color::Blue);
            else c.setFillColor(sf::Color::Red);
            c.setPosition(px - 22.5, py - 22.5);
            w.draw(c);
        }
};

const struct thingstat Marine::stats[1] = {1000, 46, 3.15, 1, {0, 1}, {45, 55}, "Marine", {1, 0, 0, 1, 0, 0, 0, 0}, {1}, 18, 1, 0.375};
sf::CircleShape Marine::c = sf::CircleShape(22.5);