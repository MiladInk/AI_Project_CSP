#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<random>
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include "dpll.h"
#define miusi unordered_map<int, unordered_set<int>>
#define usi unordered_set<int>
#define umii unordered_map<int, int>
#define pii pair<int,int>
#define MAX_DIM 500
#define MAX_PIECE 50
#define print(x) cerr << x 
using namespace std;
//global variables//
//---performance check---//
clock_t unitc, cnfc, purec, choosec, crd;
int dpllcall =0, dplldepth = 0;


bool var_array[MAX_PIECE][MAX_DIM][MAX_DIM][4];
int m, n, p;
int field[MAX_DIM][MAX_DIM];
int return_depth = -1;
miusi var_rule;
miusi rule_var;
miusi gvar_rule;
miusi grule_var;
usi active_rules;
usi unit_rules;
usi pure_candidates;
bool exit_null_rule = false;
umii decl;
vector<int> deactivated_rules;
usi assigned_vars;
vector<pii> removed_variables;
vector<Piece> pieces;

bool get(int index) {
    bool negate = false;
    if (index < 0) {
        cout << "camel camel!" << endl;
        negate = true;
        index = -index;
    }
    int k = index / (m * n * 4);
    index %= m * n * 4;
    int m = index / (n * 4);
    index %= n * 4;
    int n = index / 4;
    int d = (index % 4);
    bool value = var_array[k][m][n][d];
    if (negate)
        value = !value;
    return value;
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
        cerr <<"not";
    cerr << "("<<k<<","<<m<<","<<n<<","<<d<<")";
    
}


void add_rule(vector<int> indices) {
    int rule_number = active_rules.size();
    active_rules.insert(rule_number);
    for (auto &index: indices) {
        var_rule[index].insert(rule_number);
        rule_var[rule_number].insert(index);
        gvar_rule[index].insert(rule_number);
        grule_var[rule_number].insert(index);
    }
}

void set(int index, bool value) {
    if (index < 0) {
        index = -index;
        value = !value;
    }
    int k = index / (m * n * 4);
    index %= m * n * 4;
    int m = index / (n * 4);
    index %= n * 4;
    int n = index / 4;
    int d = (index % 4);
    var_array[k][m][n][d] = value;
}


int get_index(int k, int i, int j, int d) {
    if (k == 0)
        cerr << "camel!" << endl;
    int result = k;
    result *= m;
    result += i;
    result *= n;
    result += j;
    result *= 4;
    result += d;
    return result;
}



bool collision(int x, int y, int piece_index) {
    if (x < 0 || x >= n || y < 0 || y >= m || field[y][x] == -1) {
        return false;
    }
    if (field[y][x] != piece_index && pieces[field[y][x]].color == pieces[piece_index].color)
        return true;
    return false;
}

int calc_return_depth(int index){
    int res = -1;
    // if(dpllcall%500<10)
    //     cout << "you spend"<< (double)crd/CLOCKS_PER_SEC << "in crd" << endl;
    clock_t begin = clock();
    for(auto& rule: gvar_rule[index]){
        for(auto var: grule_var[rule]){
            if(var_rule[var].empty() && var_rule[-var].empty()  && decl[var]>res)
                res = decl[var];
                if(res == dplldepth -1)
                    return res;
        }
    }
    for(auto& rule: gvar_rule[-index]){
        for(auto var: grule_var[rule]){
            if(var_rule[var].empty() && var_rule[-var].empty() && decl[var]>res)
                res = decl[var];
                if(res == dplldepth -1)
                    return res;
        }
    }
    crd += clock()-begin;
    //  if(res!= -1)
    //      cout <<"return depth is" << res << "but "<< dplldepth << endl;
    return res;
}

void cnf(int index, bool value) {
    clock_t begin = clock();
    // if((dpllcall%500)==0)
    //     cout << "time in cnf is " <<(double)cnfc/CLOCKS_PER_SEC << endl;
    decl[index] = dplldepth;
    decl[-index] = dplldepth; 
    if (value == false)
        index = -index;
    set(index, true);
    for (auto &rule: var_rule[index]) {
        for (auto &index2: rule_var[rule]) {
            if (index == index2)
                continue;
            var_rule[index2].erase(rule);
            if(var_rule[-index2].empty())
                pure_candidates.insert(index2);
            removed_variables.push_back(make_pair(index2, rule));
        }
        removed_variables.push_back(make_pair(index, rule));
        rule_var[rule].clear();
        active_rules.erase(rule);
        deactivated_rules.push_back(rule);
    }
    var_rule[index].clear();
    for (auto &rule:var_rule[-index]) {
        rule_var[rule].erase(-index);
        removed_variables.push_back(make_pair(-index, rule));
        if (rule_var[rule].size() == 1)
            unit_rules.insert(rule);
        else if (rule_var[rule].size() == 0) {
            exit_null_rule = true;
        }
    }
    var_rule[-index].clear();
    cnfc+= clock()-begin;
}

bool pure() {
    clock_t begin = clock();
    bool changed = false;
    for (auto can: pure_candidates) {
        if (var_rule[-can].empty() && !var_rule[can].empty()) {
            cnf(can, true);
            changed = true;
        }
    }
    pure_candidates.clear();
    clock_t end = clock();
    purec = end - begin;
    return changed;
}

bool unit() {
    clock_t begin = clock();
    bool changed = false;
    while (!unit_rules.empty()) {
        int unit_rule = *unit_rules.begin();
        unit_rules.erase(unit_rule);
        if (rule_var[unit_rule].size() == 1) {
            cnf(*rule_var[unit_rule].begin(), true);
            changed = true;
        }
    }
    clock_t end = clock();
    unitc = end - begin;
    return changed;
}

bool draw_one_tile(int x, int y, int mark, bool place_it) {
    if (x < 0 || x >= n || y < 0 || y >= m) {
        return false;
    }
    if (mark >= 0) {
        if (field[y][x] != -1)
            return false;
        if (collision(x - 1, y, mark) || collision(x, y - 1, mark) || collision(x + 1, y, mark) ||
            collision(x, y + 1, mark))
            return false;
    }
    if (place_it) {
        field[y][x] = mark;
    }
    return true;
}

bool draw(Piece &piece, Place place, bool place_it, int mark) {
    for (int i = 0; i < piece.width; i++)
        for (int j = 0; j < piece.height; j++) {
            if (piece.pattern[j * piece.width + i] == '.')
                continue;
            if (place.dir == 0 && !draw_one_tile(place.x + i, place.y + j, mark, place_it))
                return false;
            if (place.dir == 1 && !draw_one_tile(place.x + j, place.y - i, mark, place_it))
                return false;
            if (place.dir == 2 && !draw_one_tile(place.x - i, place.y - j, mark, place_it))
                return false;
            if (place.dir == 3 && !draw_one_tile(place.x - j, place.y + i, mark, place_it))
                return false;
        }
    return true;
}

void print_field() {
    cout << "maparr = [";
    for (int j = 0; j < m; j++) {
        cout << "[";
        for (int i = 0; i < n; i++) {
            cout << field[j][i] + 1 << ",";
        }
        cout << "],";
        //cout << endl;
    }
    cout << "]" << endl;
}

void print_color_map() {
    cout << "color_map = ";
    cout << "{0:0,";
    for (int i = 0; i < p; i++) {
        cout << i + 1 << ":" << pieces[i].color;
        if (i != p - 1) {
            cout << ",";
        } else {
            cout << "}";
        }
    }
}

int choose_index() {
    int key = 0;
    int value = 0;
    for(auto& kv:var_rule){
        if(kv.first >0 && kv.second.size()+var_rule[-kv.first].size()>value){
            value = kv.second.size();
            key = kv.first;
        }
    }
    return key;
}

void revert(vector<int> deactivated_rules_local, vector<pii> removed_variables_local) {
    for (auto &rule:deactivated_rules_local) {
        active_rules.insert(rule);
    }
    for (auto &vr:removed_variables_local) {
        var_rule[vr.first].insert(vr.second);
        rule_var[vr.second].insert(vr.first);
    }
}

bool goahead(){
    if(return_depth == -1 || dplldepth == return_depth){
        return_depth = -1;
        return true;
    }
    return false;
}

void inc_depth(int index){
    dplldepth++;
    assigned_vars.insert(index);
}

void dec_depth(int index){
    dplldepth--;
    assigned_vars.erase(index);
}


bool dpll() {
    dpllcall++;
    //if(dpllcall%500==0) cout << "dpllcall is" << dpllcall <<"depth is"<<dplldepth<< endl;
    bool active = false, nullrule = false;
    while (true) {
        if (active_rules.empty()) {
            active = true;
            break;
        }
        if (exit_null_rule) {
            exit_null_rule = false;
            nullrule = true;
            break;
        }
        if (pure() || unit())
            continue;
        break;
    }
    vector<int> deactivated_rules_local1 = move(deactivated_rules);
    vector<pii> removed_variables_local1 = move(removed_variables);
    deactivated_rules.clear();
    removed_variables.clear();
    if (active || nullrule) {
        revert(deactivated_rules_local1, removed_variables_local1);
        if (active) 
            return true;
        if (nullrule){
            return false;
        }
    }
    int index = choose_index();
    inc_depth(index);
    if(dplldepth<5){
        cerr<<dplldepth<<" -> ";
        print_human(index); cerr << endl;
    }
    cnf(index, true);
    vector<int> deactivated_rules_local2 = move(deactivated_rules);
    vector<pair<int, int>> removed_variables_local2 = move(removed_variables);
    deactivated_rules.clear();
    removed_variables.clear();
    if (dpll())
        return true;
    revert(deactivated_rules_local2, removed_variables_local2);
    if(!goahead()){
        revert(deactivated_rules_local1, removed_variables_local1);
        dec_depth(index);
        return false;
    }
    assigned_vars.erase(index);
    assigned_vars.insert(-index);
    cnf(index, false);
    deactivated_rules_local2 = move(deactivated_rules);
    removed_variables_local2 = move(removed_variables);
    deactivated_rules.clear();
    removed_variables.clear();
    if(dplldepth<5){
        cerr<<dplldepth<<" -> not";
        print_human(index); cerr << endl;
    }
    if(dpll())
        return true;
    revert(deactivated_rules_local2, removed_variables_local2);
    return_depth = calc_return_depth(index);
    revert(deactivated_rules_local1, removed_variables_local1);
    dec_depth(-index);
    return false;
}

void generate_rules(){
    for (int k = 0; k < p; k++) {
        vector<int> exist_baby;
        Piece &piece = pieces[k];
        for (int x = 0; x < n; x++)
            for (int y = 0; y < m; y++)
                for (int d = 0; d < 4; d++) {
                    Place place = {x, y, d};
                    int index1 = get_index(k + 1, y, x, d);
                    exist_baby.push_back(index1);

                    if (!draw(piece, place, false, 0)) {
                        vector<int> atoms;
                        atoms.push_back(-index1);
                        add_rule(atoms);
                    }
                    for (int xx = 0; xx < n; xx++)
                        for (int yy = 0; yy < m; yy++)
                            for (int dd = 0; dd < 4; dd++) {
                                if (x == xx && y == yy && d == dd)
                                    continue;
                                int index2 = get_index(k + 1, yy, xx, dd);
                                if (index1 < index2)
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
    for (int k = 0; k < p; k++) {
        Piece &piece1 = pieces[k];
        for (int x = 0; x < n; x++)
            for (int y = 0; y < m; y++)
                for (int d = 0; d < 4; d++)
                    for (int kk = 0; kk < k; kk++) {
                        Piece &piece2 = pieces[kk];
                        for (int xx = 0; xx < n; xx++)
                            for (int yy = 0; yy < m; yy++)
                                for (int dd = 0; dd < 4; dd++) {
                                    int index1 = get_index(k + 1, y, x, d);
                                    int index2 = get_index(kk + 1, yy, xx, dd);
                                    Place place1 = {x, y, d};
                                    Place place2 = {xx, yy, dd};
                                    if (!draw(piece1, place1, false, k))
                                        continue;
                                    draw(piece1, place1, true, k);
                                    if (!draw(piece2, place2, false, kk)) {
                                        vector<int> atoms;
                                        atoms.push_back(-index1);
                                        atoms.push_back(-index2);
                                        add_rule(atoms);
                                    }
                                    draw(piece1, place1, true, -1);//undraw first piece
                                }
                    }
    }
    
    
}

void fast_hash_setting(){
    var_rule.rehash(var_rule.size()*100);
    rule_var.rehash(rule_var.size()*100);
    active_rules.rehash(active_rules.size()*100);
    pure_candidates.rehash(var_rule.size()*100);
    unit_rules.rehash(rule_var.size()*100);
    pure_candidates.rehash(var_rule.size()*100);
    decl.rehash(var_rule.size()*100);
}

int main() {
    cin >> m >> n >> p;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            field[j][i] = -1;
    //--get the pieces
    for (int i = 0; i < p; i++) {
        Piece piece;
        cin >> piece.height >> piece.color;
        if (piece.height == -1)
            break;
        string next_pattern_line;
        for (int j = 0; j < piece.height; j++) {
            cin >> next_pattern_line;
            piece.pattern += next_pattern_line;
            piece.width = next_pattern_line.length();
        }
        pieces.push_back(piece);
    }
    p = pieces.size();
    generate_rules();
    fast_hash_setting();
    //---solving the piece
    clock_t begin = clock();
    bool solved = dpll();
    clock_t end = clock();
    double passed_time = double(end - begin) / CLOCKS_PER_SEC;
    cerr << passed_time << " seconds passed.\n";
    if (solved) {
        for (int k = 0; k < p; k++) {
            Piece &piece1 = pieces[k];
            for (int x = 0; x < n; x++)
                for (int y = 0; y < m; y++)
                    for (int d = 0; d < 4; d++) {
                        Place place = {x, y, d};
                        if (var_array[k + 1][y][x][d])
                            draw(piece1, place, true, k);
                    }
        }
        print_field();
    } else {
        cout << "It is impossible." << endl;
    }
    print_color_map();
}