#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<random>
#include<algorithm>
#include<vector>
#include<map>
#include<unordered_set>
#define MAX_DIM 500
#define MAX_PIECE 50
using namespace std;
bool var_array[MAX_PIECE][MAX_DIM][MAX_DIM][4];
int m , n, p, field[MAX_DIM][MAX_DIM];
vector<vector<int>> rules;
map<int,unordered_set<int>> var_rule_mp;
int unitcall, purecall, choosecall, cnfcall;

void print_rule(vector<vector<int>> rules){
    cout <<"rules:";
    for(auto &rule: rules){
        for(auto& atom: rule){
            cout << atom <<",";
        }
        cout << endl;
    }
    cout <<"rules ended" << endl;
}

bool get(int index){
    bool negate = false;
    if(index < 0){
        cout << "camel camel!" << endl;
        negate = true;
        index = -index;
    }
    int k = index/(m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    bool value =  var_array[k][m][n][d];
    if(negate)
        value = !value;
    return value;
}

void print_hamany(int index){
    int k = index/(m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    cout << "("<<k<<","<<m<<","<<n<<","<<d<<")";
}

void print_human(int index){
    bool negate = false;
    if(index < 0){
        negate = true;
        index = -index;
    }
    int k = index/(m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    if(negate)
        cout <<"not";
    cout << "("<<k<<","<<m<<","<<n<<","<<d<<")";
    
}

void add_rule(vector<int> indices){
    for(auto& index: indices)
        mp[index].insert()
    rules.push_back(indices);
}






void set(int index, bool value){
    int k = index/ (m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    var_array[k][m][n][d] = value;
}


int get_index(int k, int i, int j, int d){
    if(k == 0)
        cerr <<"camel!" << endl;
    int result = k;
    result*= m;
    result+= i;
    result*=n;
    result+=j;
    result*=4;
    result+=d;
    return result;    
}

struct Place{
    int x;
    int y;
    int dir;
};

struct Piece{
    int width;
    int height;
    int color;
    string pattern;
};

vector<Piece>pieces;


bool collision(int x, int y, int piece_index){
    if(x<0 || x>=n || y<0 || y>=m || field[y][x]==-1){
        return false;
    } 
    if(field[y][x]!= piece_index && pieces[field[y][x]].color == pieces[piece_index].color)
        return true;
    return false;
}

vector<vector<int>> cnf(vector<vector<int>> rules, int index, bool value){
    cnfcall++;
    if(index<0){
        index = -index;
        value = !value;
    }
    vector<vector<int>> res;
    set(index, value);
    for (auto &rule: rules){
        bool add_flag = true;
        vector<int> to_add;
        for(auto& atom: rule){
            if((atom == index && value == true)||(atom == -index && value == false)){
                add_flag = false;
                break;
            }
            if((atom == index && value == false)||(atom == -index && value == true)){
                continue;
            }
            to_add.push_back(atom);
        }
        if(add_flag)
            res.push_back(to_add);   
    }
    return res;
}

bool pure(vector<vector<int>>& rules){
    purecall++;
    bool changed = false;
    map<int, bool> mp;
    for(auto &rule: rules){
        for(auto& atom :rule){
            mp[atom] = true;
        }
    }
    for(auto &kv: mp){
        if(mp.find(-kv.first) == mp.end()){
            rules = cnf(rules, kv.first, true);
            changed = true;
        }
    }
    return changed;
}

bool unit(vector<vector<int>>& rules){
    unitcall++;
    bool changed = false;
    map<int, bool> mp;
    for(auto &rule: rules){
        if(rule.size()==1){
            mp[rule[0]] = true;
        }
    }
    for(auto &kv: mp){
        rules = cnf(rules, kv.first, true);
        changed = true;
    }
    return changed;
}

bool draw_one_tile(int x, int y, int mark, bool place_it){
    if(x<0 || x>=n || y<0 || y>=m){
        return false;
    }
    if(mark>=0){
        if(field[y][x]!= -1)
            return false;
        if(collision(x-1,y,mark)||collision(x,y-1,mark)||collision(x+1,y,mark)||collision(x,y+1,mark))
            return false;
    }
    if(place_it){
        field[y][x] = mark;
    }
    return true;
}

bool draw(Piece& piece, Place place, bool place_it, int mark){
    for(int i = 0; i<piece.width; i++)
        for(int j = 0; j<piece.height; j++){
            if(piece.pattern[j*piece.width+i] == '.')
                continue;
            if(place.dir == 0 && !draw_one_tile(place.x+i, place.y+j, mark, place_it))
                return false;
            if(place.dir == 1 && !draw_one_tile(place.x+j, place.y-i, mark, place_it))
                return false;
            if(place.dir == 2 &&!draw_one_tile(place.x-i, place.y-j, mark, place_it))
                return false;
            if(place.dir == 3 &&!draw_one_tile(place.x-j, place.y+i, mark, place_it))
                return false;
        }
    return true;
}

void print_field(){
    cout << "maparr = [";
    for(int j = 0; j<m; j++){
        cout<<"[";
        for(int i = 0; i<n; i++){
            cout << field[j][i]+1<<",";
        }
        cout<<"],";
        //cout << endl;
    }
    cout << "]" << endl;
}

void print_color_map(){
    cout << "color_map = ";
    cout <<"{0:0,";
    for(int i = 0; i<p; i++){
        cout << i+1 <<":" << pieces[i].color;
        if(i!=p-1){
            cout << ",";
        }
        else{
            cout << "}";
        }
    }
}

bool find_empty_rule(vector<vector<int>> rules){
    for(auto& rule: rules){
        if(rule.empty())
            return true;
    }
    return false;
}    

int choose_index(vector<vector<int>>& rules){
    choosecall++;
    map<int,int>mp;
    for(auto& rule:rules)
        for(auto &atom:rule){
            mp[atom]++;
        }
    int key = 0;
    int value = 0;
    for(auto& kv: mp){
        if(kv.second>value){
            key = kv.first;
            value = kv.second;
        }
    }
    return key;
}

bool dpll(vector<vector<int>> rules){
    while(true){
        if(rules.empty())
            return true;
        if(find_empty_rule(rules))
            return false;
        if(pure(rules) || unit(rules))
            continue;
        break;
    }
    int index = choose_index(rules);
    if(dpll(cnf(rules, index, true)))
        return true;
    return dpll(cnf(rules, index, false));
}



int main(){
    cin >> m >> n >> p;
    for(int i = 0; i<n; i++)
        for(int j = 0; j<m; j++)
            field[j][i] = -1;
    //--get the pieces
    for(int i = 0; i<p; i++){
        Piece piece;
        cin >> piece.height >> piece.color;
        if(piece.height == -1)
            break;
        string next_pattern_line;
        for(int j = 0; j<piece.height;j++){
            cin >> next_pattern_line;
            piece.pattern += next_pattern_line;
            piece.width = next_pattern_line.length();
        }
        pieces.push_back(piece);
    }
    p = pieces.size();
    for(int k = 0; k<p; k++){
        vector<int> exist_baby;
        Piece& piece = pieces[k];
        for(int x = 0; x<n; x++)
            for(int y = 0; y<m; y++)
                for(int d = 0; d<4; d++){
                    Place place = {x, y, d};
                    int index1 = get_index(k+1, y, x, d);
                    exist_baby.push_back(index1);

                    if(!draw(piece, place, false, 0)){
                        vector<int> atoms;
                        atoms.push_back(-index1);
                        add_rule(atoms);
                    }
                    for(int xx = 0; xx<n; xx++)
                        for(int yy = 0; yy<m; yy++)
                            for(int dd = 0; dd<4; dd++){
                                if(x==xx && y==yy && d==dd)
                                    continue;
                                int index2 = get_index(k+1, yy, xx, dd);
                                if(index1<index2)
                                    continue;
                                vector<int> atoms;
                                atoms.push_back(-index1);
                                atoms.push_back(-index2);
                                add_rule(atoms);
                            }
                }
        //---existential rules
        add_rule(exist_baby);
    }
    //cout << rules.size() << endl;
    //---do not collide babies
    for(int k = 0; k<p; k++){
        Piece& piece1 = pieces[k];
        for(int x = 0; x<n; x++)
            for(int y = 0; y<m; y++)
                for(int d = 0; d<4; d++)
                    for(int kk = 0; kk<k; kk++){
                        Piece& piece2 = pieces[kk];
                        for(int xx = 0; xx<n; xx++)
                            for(int yy = 0; yy<m; yy++)
                                for(int dd = 0; dd<4; dd++){
                                    int index1 = get_index(k+1, y, x, d);
                                    int index2 = get_index(kk+1, yy, xx, dd);
                                    Place place1 = {x, y, d};
                                    Place place2 = {xx, yy, dd};
                                    if(!draw(piece1, place1, false, k))
                                        continue;
                                    draw(piece1, place1, true, k);
                                    if(!draw(piece2, place2, false, kk)){
                                        vector<int>atoms;
                                        atoms.push_back(-index1);
                                        atoms.push_back(-index2);
                                        add_rule(atoms);
                                    }    
                                    draw(piece1, place1, true, -1);//undraw first piece
                                }
                    }
    }
    //cout << rules.size() << endl;
    //---solving the piece
    clock_t begin = clock();

    bool solved = dpll(rules);
    clock_t end = clock();
    double passed_time = double(end-begin)/CLOCKS_PER_SEC;
    cerr << passed_time <<" seconds passed.\n";
    if(solved){
        for(int k = 0; k<p; k++){
            Piece& piece1 = pieces[k];
            for(int x = 0; x<n; x++)
                for(int y = 0; y<m; y++)
                    for(int d = 0; d<4; d++){
                        Place place = {x, y, d};
                        if(var_array[k+1][y][x][d])
                            draw(piece1, place, true, k);
                    }
        }
        print_field();
    }
    else{
        cout<<"It is impossible."<<endl;
    }
    print_color_map();
    cout << unitcall << " "<<purecall <<" "<< choosecall <<" "<<cnfcall << endl;
}