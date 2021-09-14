#include "Thing.hpp"
#include "math.h"
#include <set>
#include <iterator>

using namespace std;

typedef pair<int, int> Pair;
typedef pair<double, Pair> ppair;
typedef pair<int, pair<Pair, Pair>> pppair;

#define sqrt2 1.414;
struct cellNode{
    int parx, pary;
    double f, g, h;
};

class Game{
    public:    
        unordered_set<Thing*> stuff;
        //terrain 15-135 low-high, 0 blocked, -1 cliff, -2 nothing
        //cango means they can get from one exit to another, needs work
        int terrain[60][140];
        bool cango[15][4][4];
        pair<double, double> camera;
        bool hasExit[15][4];
        fakeAction exits[15][4];
        list<fakeAction> paths[15][4][4];

        set<Thing> allthings;
        vector<set<Thing>> groups, oppgroup, allgroup;

        void assignPath(Thing* thing, fakeAction a){
            
        };
        //find the submap of the point, from left to right, up to down
        int submap(double px, double py){
            int temp1 = (int) (px / 20), temp2 = (int) ((py - 20) / 20);
            if(temp2 < 0) temp2 = 0;
            if(temp2 > 4) temp2 = 4;
            return temp2 * 3 + temp1;
        }
        int submap(fakeAction ac){
            return submap(ac.px, ac.py);
        }

        /*The exit locations of each section, n location:
            0
        2       1
            3
        */
        pair<double, double> exitPos(int s, int n){
            double x[] = {(s % 3) * 10.0 + 5.5, (s / 3) * 10.0 + 0.5};
            if(n % 2 == 1){
                x[0] += 5;
                x[1] += 5;
            }
            if(n / 2 > 0){
                x[0] -= 5;
                x[1] += 5;
            }
            return make_pair(x[0], x[1]);
        }
        //called when building placed and the paths need to be recalculated
        void updatePaths(int s){
            memset(cango[s], false, sizeof cango[s]);
            for(int a = 0; a < 3; a ++){
                if(hasExit[s][a]){
                    for(int b = a + 1; b < 4; b ++){
                        if(hasExit[s][b]){
                            list<fakeAction> fa = {exits[s][a], exits[s][b]};
                            if(aStar(fa)){
                                cango[s][a][b] = true;
                                cango[s][b][a] = true;
                                paths[s][a][b] = fa;
                                fa.reverse();
                                paths[s][b][a] = fa;
                            }else{
                                cango[s][a][b] = false;
                                cango[s][b][a] = false;
                            }
                        }
                    }
                }
            }
        }
        //checks if it has things nearby
        bool checkChoke(int a, int b){
            for(int i = -1; i <= 1; i ++){
                for(int j = -1; j <= 1; j ++){
                    if(terrain[a + i][b + j] <= 0 && terrain[a + i][b + j] != -2) return false;
                }
            }
            return true;
        }
        //always in the same grid
        bool aStar(list<fakeAction> &l){
            fakeAction st = l.front(), en = l.back();
            int width = 20, height = 20, sm = submap(st), stpx = 20 * (sm % 3), stpy = 20 + 20 * (sm / 3);
            int stx = (int) st.px, sty = (int) st.py, enx = (int) en.px, eny = (int) en.py;
            
            if(sm == 1){
                stpy -= 20;
                height += 20;
            }
            
            if(sm == 13) height += 20;
            bool visit[width][height];
            memset(visit, false, sizeof(visit));
            cellNode celldetail[width][height];
            for(int a = 0; a < height; a ++){
                for(int b = 0; b < width; b ++){
                    celldetail[b][a].f = 100000;
                }
            }
            celldetail[stx - stpx][sty - stpy].g = 0;
            celldetail[stx - stpx][sty - stpy].f = 0;
            ppair q;
            set<ppair> openList;
            openList.insert(make_pair(0.0, make_pair(stx, sty)));
            int i, j, ip, jp;
            double hn, gn, fn;
            bool found = false;
            while(!openList.empty()){
                q = *openList.begin();
                openList.erase(openList.begin());
                i = q.second.first;
                j = q.second.second;
                ip = i - stpx;
                jp = j - stpy;
                found = (i == enx && j == eny);
                if(found) break;
                visit[ip][jp] = true;
                for(int a = -1; a <= 1; a ++){
                    for(int b = -1; b <= 1; b ++){
                        if(ip + a >= 0 && ip + a < width && jp + b >= 0 && jp + b < height){
                            if(abs(a) == abs(b)) continue;
                            if(terrain[i + a][j + b] > 0 && !visit[ip + a][jp + b]){
                                celldetail[ip + a][jp + b].g = celldetail[ip][jp].g + sqrt(pow(a, 2) + pow(b, 2));
                                celldetail[ip + a][jp + b].h = sqrt(pow(enx - i - a, 2) + pow(eny - j - b, 2));
                                fn = celldetail[ip + a][jp + b].h + celldetail[ip + a][jp + b].g;
                                if(celldetail[ip + a][jp + b].f > fn){
                                    celldetail[ip + a][jp + b].f = fn;
                                    openList.insert(make_pair(celldetail[ip + a][jp + b].f, make_pair(i + a, j + b)));
                                    celldetail[ip + a][jp + b].parx = ip;
                                    celldetail[ip + a][jp + b].pary = jp;
                                }
                            }
                        }
                    }
                }
            }

            if(found){
                auto it = prev(l.end());
                ip = enx - stpx;
                jp = eny - stpy;
                i = stx - stpx;
                j = sty - stpy;

                while(ip != i || jp != j){
                    int temp = celldetail[ip][jp].parx;
                    jp = celldetail[ip][jp].pary;
                    ip = temp;
                    if(!checkChoke(ip + stpx, jp + stpy)){
                        fakeAction tempac = fakeAction(ip + stpx + 0.5, jp + stpy + 0.5, 0);
                        l.insert(it, tempac);
                        --it;
                    }
                }
                return true;
            }
            return false;
        }
        //gives the other end of the ramp as the submap, exit pair
        Pair uphill(int s, int e){
            int a = e % 2 == 1? 1: -1;
            if(e == 0 || e == 3) a *= 3;
            //l.insert(it, exits[s + a, 3 - e]);
            return make_pair(s + a, 3 - e);
        }
        bool findPathWhole(list<fakeAction> &l){
            fakeAction st = l.front(), en = l.back();
            int stmap = submap(st), enmap = submap(en);
            list<fakeAction> fakelist;
            if(stmap == enmap){
                return aStar(l);
            }else{
                vector<int> to;
                for(int a = 0; a < 4; a ++){
                    if(hasExit[enmap][a]){
                        fakeAction temp = exits[enmap][a];
                        fakelist = {temp, en};
                        if(aStar(fakelist)){
                            to.push_back(a);
                        }
                    }
                }
                if(to.size() == 0) return false;
                int visited[15][4], len[to.size()];
                memset(visited, -1, sizeof visited);
                memset(len, -1, sizeof len);
                set<ppair> pathsto;
                int tracer[15][4];
                for(int a = 0; a < 4; a ++){
                    if(hasExit[stmap][a]){
                        fakeAction temp = (exits[stmap][a]);
                        fakelist = {st, temp};
                        if(aStar(fakelist)){
                            visited[stmap][a] = 0;
                            pathsto.insert(make_pair(0, make_pair(stmap, a)));
                            int found = -1;
                            while(!pathsto.empty()){
                                ppair temppair = *pathsto.begin();
                                pathsto.erase(pathsto.begin());
                                int tempint = temppair.second.second, t1 = temppair.second.first, t2 = tempint % 2 == 0? -1: 1;
                                if(tempint == 0 || tempint == 3) t2 *= 3;
                                t1 += t2;
                                tempint = 3 - tempint;
                                visited[t1][tempint] = temppair.first;
                                if(t1 == enmap){
                                    int tempi = 0, updated = 0;
                                    for(auto it = to.begin(); it != to.end(); ++ it, tempi ++){
                                        if(*it == tempint){
                                            if(len[tempi] > temppair.first || len[tempi] == -1){
                                                updated = true;
                                                len[tempi] = temppair.first;
                                            }
                                            found = tempi;
                                            break;
                                        }
                                    }
                                    if(found != -1){
                                        if(!updated) found = -1;
                                        break;
                                    }
                                }
                                for(int b = 0; b < 4; b ++){
                                    if(b != tempint){
                                        if(cango[t1][b][tempint]){
                                            if(visited[t1][b] == -1 || visited[t1][b] > temppair.first + 1){
                                                visited[t1][b] = temppair.first + 1;
                                                tracer[t1][b] = tempint;
                                                pathsto.insert(make_pair(temppair.first + 1, make_pair(t1, b)));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                int found = -1, lengtha = 1000;
                for(int a = 0; a < to.size(); a ++){
                    if(len[a] != -1 && lengtha > len[a]){
                        found = a;
                        lengtha = len[a];
                    }
                }
                if(found == -1) return false;
                fakeAction temp = (exits[enmap][to[found]]);
                l = {temp, en};
                aStar(l);
                Pair p = uphill(enmap, to[found]);
                enmap = p.first;
                found = p.second;
                while(enmap != stmap){
                    int ti1 = tracer[enmap][found];
                    l.splice(l.begin(), paths[enmap][ti1][found]);
                    tie(enmap, found) = uphill(enmap, ti1);
                }

                list<fakeAction>templist = {st, exits[stmap][found]};
                aStar(templist);
                l.splice(l.begin(), templist);
            }
            return true;
        }
        Game(){
            //the main blocks terrain
            for(int a = 0; a < 20; a ++){
                for(int b = 0; b < 20; b ++){
                    terrain[a + 20][b] = 135;
                    terrain[a + 20][b + 20] = 135;
                    terrain[a + 20][b + 100] = 135;
                    terrain[a + 20][b + 120] = 135;
                    terrain[a][b + 20] = 75;
                    terrain[a + 40][b + 20] = 75;
                    terrain[a][b + 100] = 75;
                    terrain[a + 40][b + 100] = 75;
                    terrain[a][b + 40] = 75;
                    terrain[a + 40][b + 40] = 75;
                    terrain[a + 20][b + 40] = 75;
                    terrain[a][b + 80] = 75;
                    terrain[a + 40][b + 80] = 75;
                    terrain[a + 20][b + 80] = 75;
                    terrain[a][b + 60] = 75;
                    terrain[a + 40][b + 60] = 75;
                    terrain[a + 20][b + 60] = 15;                    
                }
            }
            //the cliffs
            for(int a = 20; a < 120; a ++){
                terrain[19][a] = -1;
                terrain[40][a] = -1;
            }
            for(int a = 0; a < 60; a ++){
                terrain[a][40] = -1;
                terrain[a][59] = -1;
                terrain[a][80] = -1;
                terrain[a][99] = -1;
            }
            //the sides
            for(int a = 0; a < 20; a ++){
                for(int b = 0; b < 20; b ++){
                    terrain[a][b] = -2;
                    terrain[40 + a][b] = -2;
                    terrain[a][120 + b] = -2;
                    terrain[40 + a][120 + b] = -2;
                }
            }
            //the ramps
            for(int a = 0; a < 3; a ++){
                terrain[18 + a][27] = -1;
                terrain[18 + a][31] = -1;
                terrain[39 + a][27] = -1;
                terrain[39 + a][31] = -1;
                terrain[18 + a][68] = -1;
                terrain[18 + a][72] = -1;
                terrain[39 + a][67] = -1;
                terrain[39 + a][71] = -1;
                terrain[18 + a][108] = -1;
                terrain[18 + a][112] = -1;
                terrain[39 + a][108] = -1;
                terrain[39 + a][112] = -1;
                terrain[28][58 + a] = -1;
                terrain[32][58 + a] = -1;
                terrain[27][79 + a] = -1;
                terrain[31][79 + a] = -1;
                terrain[48 + a][40] = 75;
                terrain[48 + a][59] = 75;
                terrain[48 + a][80] = 75;
                terrain[9 + a][99] = 75;
                terrain[9 + a][80] = 75;
                terrain[9 + a][59] = 75;
                terrain[19][48 + a] = 75;
                terrain[40][48 + a] = 75;
                terrain[19][89 + a] = 75;
                terrain[40][89 + a] = 75;
                for(int b = 0; b < 3; b ++){
                    terrain[18 + a][28 + b] = 90 + 15 * a;
                    terrain[39 + a][28 + b] = 120 - 15 * a;
                    terrain[18 + a][69 + b] = 60 - 15 * a;
                    terrain[39 + a][68 + b] = 30 + 15 * a;
                    terrain[18 + a][109 + b] = 90 + 15 * a;
                    terrain[39 + a][109 + b] = 120 - 15 * a;
                    terrain[29 + a][58 + b] = 60 - 15 * b;
                    terrain[28 + a][79 + b] = 30 + 15 * b;
                }
            }
            
            memset(hasExit, false, sizeof hasExit);
            hasExit[0][1] = true;
            exits[0][1] = fakeAction(18.5, 29.5, 0);
            hasExit[1][2] = true;
            exits[1][2] = fakeAction(20.5, 29.5, 0);
            hasExit[1][1] = true;
            exits[1][1] = fakeAction(39.5, 29.5, 0);
            hasExit[2][2] = true;
            exits[2][2] = fakeAction(41.5, 29.5, 0);
            hasExit[2][3] = true;
            exits[2][3] = fakeAction(49.5, 39.5, 0);
            hasExit[5][0] = true;
            exits[5][0] = fakeAction(49.5, 41.5, 0);
            hasExit[5][2] = true;
            exits[5][2] = fakeAction(41.5, 49.5, 0);
            hasExit[5][3] = true;
            exits[5][3] = fakeAction(49.5, 58.5, 0);
            hasExit[8][0] = true;
            exits[8][0] = fakeAction(49.5, 60.5, 0);
            hasExit[4][1] = true;
            exits[4][1] = fakeAction(39.5, 49.5, 0);
            hasExit[4][2] = true;
            exits[4][2] = fakeAction(20.5, 49.5, 0);
            hasExit[4][3] = true;
            exits[4][3] = fakeAction(30.5, 58.5, 0);
            hasExit[7][0] = true;
            exits[7][0] = fakeAction(30.5, 60.5, 0);
            hasExit[3][1] = true;
            exits[3][1] = fakeAction(18.5, 49.5, 0);
            hasExit[3][3] = true;
            exits[3][3] = fakeAction(10.5, 58.5, 0);
            hasExit[6][0] = true;
            exits[6][0] = fakeAction(10.5, 60.5, 0);
            hasExit[6][1] = true;
            exits[6][1] = fakeAction(18.5, 70.5, 0);
            hasExit[7][2] = true;
            exits[7][2] = fakeAction(20.5, 70.5, 0);
            hasExit[7][3] = true;
            exits[7][3] = fakeAction(29.5, 79.5, 0);
            hasExit[10][0] = true;
            exits[10][0] = fakeAction(29.5, 81.5, 0);
            hasExit[7][1] = true;
            exits[7][1] = fakeAction(39.5, 69.5, 0);
            hasExit[8][2] = true;
            exits[8][2] = fakeAction(41.5, 69.5, 0);
            hasExit[8][3] = true;
            exits[8][3] = fakeAction(49.5, 79.5, 0);
            hasExit[9][0] = true;
            exits[9][0] = fakeAction(10.5, 81.5, 0);
            hasExit[6][3] = true;
            exits[6][3] = fakeAction(10.5, 79.5, 0);
            hasExit[11][0] = true;
            exits[11][0] = fakeAction(49.5, 81.5, 0);
            hasExit[11][2] = true;
            exits[11][2] = fakeAction(41.5, 90.5, 0);
            hasExit[10][1] = true;
            exits[10][1] = fakeAction(39.5, 90.5, 0);
            hasExit[10][2] = true;
            exits[10][2] = fakeAction(20.5, 90.5, 0);
            hasExit[9][1] = true;
            exits[9][1] = fakeAction(18.5, 90.5, 0);
            hasExit[9][3] = true;
            exits[9][3] = fakeAction(10.5, 98.5, 0);
            hasExit[12][0] = true;
            exits[12][0] = fakeAction(10.5, 100.5, 0);
            hasExit[12][1] = true;
            exits[12][1] = fakeAction(18.5, 110.5, 0);
            hasExit[13][2] = true;
            exits[13][2] = fakeAction(20.5, 110.5, 0);
            hasExit[13][1] = true;
            exits[13][1] = fakeAction(39.5, 110.5, 0);
            hasExit[14][2] = true;
            exits[14][2] = fakeAction(41.5, 110.5, 0);
            camera = make_pair(0.0, 0.0);
            for(int a = 0; a < 15; a ++) updatePaths(a);
        }
        void update(){
            for(Thing *t: stuff){
                updateU(*t);
            }
        }

    private:
        void updateU(Thing u){
            if(u.isUnit){
                Unit up = (Unit) u;
                if(!up.alis.empty()){
                    
                }
            }
        }
};
