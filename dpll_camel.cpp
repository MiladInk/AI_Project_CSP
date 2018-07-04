#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<random>
#include<algorithm>
#include<vector>
#include<map>
#define MAX_DIM 500
#define MAX_PIECE 50
using namespace std;
bool var_array[MAX_PIECE][MAX_DIM][MAX_DIM][4];
bool var_declared[MAX_PIECE][MAX_DIM][MAX_DIM][4];
int m = 10, n = 10, p = 10, field[MAX_DIM][MAX_DIM];
vector<vector<int>> rules;

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
        negate = true;
        index = -index;
    }
    int k = index/(m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    if(!var_declared[k][m][n][d])
        return true;
    bool value =  var_array[k][m][n][d];
    if(negate)
        value = !value;
    return value;
}

void add_rule(vector<int> indices){
    rules.push_back(indices);
}

bool sat_rule(int rule_index){
    bool result = false;
    for(auto &atom : rules[rule_index])
        result = result || get(atom);
    return result;
}


void set(int index, bool value){
    int k = index/ (m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    var_array[k][m][n][d] = value;
    var_declared[k][m][n][d] = true;
}


void unset(int index, bool value){
    int k = index/(m*n*4);
    index %= m*n*4;
    int m = index/(n*4);
    index %= n*4;
    int n = index/4;
    int d = (index%4);
    var_declared[k][m][n][d] = false;
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
    vector<vector<int>> res;
    set(index, value);
    for (auto &rule: rules){
        bool add_flag = true;
        vector<int> to_add;
        for(auto& atom: rule){
            if((atom == index && value == true)||(atom == -index && value == false)){
                add_flag = false;
                break;
            }else if((atom == index && value == false)||(atom == -index && value == true)){
                continue;
            }
            to_add.push_back(atom);
        }
        if(add_flag)
            res.push_back(to_add);   
    }
    return res;
}

vector<vector<int>> pure(vector<vector<int>> rules){
    bool iter = true;
    while(iter){
        iter = false;
        map<int, bool> mp;
        for(auto &rule: rules){
            for(auto& atom :rule){
                mp[atom] = true;
            }
        }
        for(auto &kv: mp){
            if(kv.first>0 && mp.find(-kv.first) == mp.end()){
                rules = cnf(rules, kv.first, true);
                iter = true;
                //cout << kv.first << endl;
            }
            if(kv.first<0 && mp.find(-kv.first) == mp.end()){
                rules = cnf(rules, -kv.first, false);
                iter =true;
                //cout << kv.first << endl;
            }
        }
    }
    return rules;
}

vector<vector<int>> unit(vector<vector<int>> rules){
    bool iter = true;
    while(iter){
        iter = false;
        for(auto &rule: rules){
            if(rule.size()==1){
                iter = true;
                if(rule[0]>0)
                    rules = cnf(rules, rule[0], true);
                else
                    rules = cnf(rules, -rule[0], false);
            }
        }
    }
    return rules;
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

bool dpll(vector<vector<int>> rules){
    if(rules.empty())
        return true;
    if(find_empty_rule(rules))
        return false;
    rules = pure(rules);
    rules = unit(rules);
    if(dpll(cnf(rules, rules[0][0], true)))
        return true;
    return dpll(cnf(rules, rules[0][0], false));
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
    //---do not collide babies
    for(int k = 0; k<p; k++){
        Piece& piece1 = pieces[k];
        for(int x = 0; x<n; x++)
            for(int y = 0; y<m; y++)
                for(int d = 0; d<4; d++)
                    for(int kk = 0; kk<k; kk++){
                        Piece& piece2 = pieces[k];
                        for(int xx = 0; xx<n; xx++)
                            for(int yy = 0; yy<m; yy++)
                                for(int dd = 0; dd<4; dd++){
                                    int index1 = get_index(k+1, y, x, d);
                                    int index2 = get_index(kk+1, yy, xx, dd);
                                    Place place1 = {x, y, d};
                                    Place place2 = {xx, yy, dd};
                                    if(!draw(piece1, place1, false, piece1.color))
                                        continue;
                                    draw(piece1, place1, true, piece1.color);
                                    if(!draw(piece2, place2, false, piece2.color)){
                                        vector<int>atoms;
                                        atoms.push_back(-index1);
                                        atoms.push_back(-index2);
                                        add_rule(atoms);
                                    }    
                                    draw(piece1, place1, true, -1);//undraw first piece
                                }
                    }
    }

    
    //---solving the piece
    clock_t begin = clock();

    bool solved;

    clock_t end = clock();
    double passed_time = double(end-begin)/CLOCKS_PER_SEC;
    cerr << passed_time <<" seconds passed.\n";

    if(solved)
        print_field();
    else{
        cout<<"It is impossible."<<endl;
    }
    print_color_map();
}