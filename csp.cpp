#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <ctime>
#include <random>
#include <algorithm>
#include <list>
#include "csp.h"
#define MAX_DIM 500
#define pii pair<int, int>
#define pb push_back
#define mp make_pair
#define fi first
#define se second
using namespace std;
int current_time = 0;
int m, n, p;
int field[MAX_DIM][MAX_DIM];
int exp_field[MAX_DIM][MAX_DIM];
vector<Piece> pieces;
set<int> free_pieces;

bool collision_exp(int x, int y, int piece_index)
{
    if (x < 0 || x >= n || y < 0 || y >= m || exp_field[y][x] == -1)
    {
        return false;
    }
    if (exp_field[y][x] != piece_index && pieces[exp_field[y][x]].color == pieces[piece_index].color)
        return true;
    return false;
}

bool draw_one_tile_exp(int x, int y, int mark, bool place_it)
{
    if (x < 0 || x >= n || y < 0 || y >= m)
    {
        return false;
    }
    if (mark >= 0)
    {
        if (exp_field[y][x] != -1)
            return false;
        if (collision_exp(x - 1, y, mark) || collision_exp(x, y - 1, mark) || collision_exp(x + 1, y, mark) || collision_exp(x, y + 1, mark))
            return false;
    }
    if (place_it)
    {
        exp_field[y][x] = mark;
    }
    return true;
}

bool draw_exp(Piece &piece, Place place, bool place_it, int mark)
{
    for (int i = 0; i < piece.width; i++)
        for (int j = 0; j < piece.height; j++)
        {
            if (piece.pattern[j * piece.width + i] == '.')
                continue;
            if (place.dir == 0 && !draw_one_tile_exp(place.x + i, place.y + j, mark, place_it))
                return false;
            if (place.dir == 1 && !draw_one_tile_exp(place.x + j, place.y - i, mark, place_it))
                return false;
            if (place.dir == 2 && !draw_one_tile_exp(place.x - i, place.y - j, mark, place_it))
                return false;
            if (place.dir == 3 && !draw_one_tile_exp(place.x - j, place.y + i, mark, place_it))
                return false;
        }
    return true;
}

bool collision(int x, int y, int piece_index)
{
    if (x < 0 || x >= n || y < 0 || y >= m || field[y][x] == -1)
    {
        return false;
    }
    if (field[y][x] != piece_index && pieces[field[y][x]].color == pieces[piece_index].color)
        return true;
    return false;
}

int available_places(int piece_index)
{
    int n_places = 0;
    for (auto &place : pieces[piece_index].places)
        if (place.last_on >= current_time)
            n_places++;
    return n_places;
}

int choose_next_piece()
{
    int cpi = *free_pieces.begin(); // cpi stands for choosen_piece_index;
    int cpi_places = available_places(cpi);
    for (auto x : free_pieces)
    {
        int x_places = available_places(x);
        if (x_places < cpi_places)
        {
            cpi = x;
            cpi_places = x_places;
        }
    }
    return cpi;
}

bool draw_one_tile(int x, int y, int mark, bool place_it)
{
    if (x < 0 || x >= n || y < 0 || y >= m)
    {
        return false;
    }
    if (mark >= 0)
    {
        if (field[y][x] != -1)
            return false;
        if (collision(x - 1, y, mark) || collision(x, y - 1, mark) || collision(x + 1, y, mark) || collision(x, y + 1, mark))
            return false;
    }
    if (place_it)
    {
        field[y][x] = mark;
    }
    return true;
}

bool draw(Piece &piece, Place place, bool place_it, int mark)
{
    for (int i = 0; i < piece.width; i++)
        for (int j = 0; j < piece.height; j++)
        {
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

void print_field()
{
    cout << "maparr = [";
    for (int j = 0; j < m; j++)
    {
        cout << "[";
        for (int i = 0; i < n; i++)
        {
            cout << field[j][i] + 1 << ",";
        }
        cout << "],";
        //cout << endl;
    }
    cout << "]" << endl;
}

void print_color_map()
{
    cout << "color_map = ";
    cout << "{0:0,";
    for (int i = 0; i < p; i++)
    {
        cout << i + 1 << ":" << pieces[i].color;
        if (i != p - 1)
        {
            cout << ",";
        }
        else
        {
            cout << "}";
        }
    }
}

bool is_2c(int piece1_index, Place &place1, int piece2_index, Place &place2)
{
    draw_exp(pieces[piece1_index], place1, true, piece1_index);
    bool res = draw_exp(pieces[piece2_index], place2, false, piece2_index);
    draw_exp(pieces[piece1_index], place1, true, -1);
    return res;
}

void reduce_arc(pii arc, list<pii> &arcs)
{
    //cerr << "reduce arc is: " << arc.fi << "," << arc.se << endl;
    Piece *phot = &pieces[arc.fi];  //this piece is hot, it is going to get rid of unconsistent places
    Piece *pcold = &pieces[arc.se]; //this piece is cold, places are static during this func
    bool changed = false;
    for (auto &hotplace : phot->places)
    {
        bool found = false;
        if (hotplace.last_on != current_time)
            continue;
        for (auto &coldplace : pcold->places)
        {
            if (coldplace.last_on != current_time)
                continue;
            if (is_2c(arc.fi, hotplace, arc.se, coldplace))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            changed = true;
            hotplace.last_on = current_time - 1;
        }
    }
    if (changed)
        for (auto &piece_index : free_pieces)
        {
            if (piece_index == arc.se || piece_index == arc.fi)
                continue;
            pii tmp_arc = mp(piece_index, arc.fi);
            arcs.pb(tmp_arc);
        }
}
/*set_piece is the piece which is recently set, so 
this is the initial set of inconsistencies in the graph
this means that it will clear some pieces from the domain
of the other pieces(in this code it is equivalent of their time being 
less than current_time) which will cause them inconsistency*/
bool ac3(int set_piece)
{
    //get places of free pieces the chance to exist.
    for (auto &free_piece : free_pieces)
    {
        for (auto &place : pieces[free_piece].places)
        {
            if (place.last_on >= current_time - 1)
                place.last_on = current_time;
        }
    }
    //set initial arcs
    list<pii> arcs;
    for (auto &piece_index : free_pieces)
    {
        pii arc = mp(piece_index, set_piece);
        arcs.pb(arc);
    }
    //make arcs consistent
    while (!arcs.empty())
    {
        pii arc = arcs.front();
        arcs.pop_front();
        reduce_arc(arc, arcs);
    }
}

bool solve(int piece_index)
{
    //cerr << "solve: " << piece_index << "time is" << current_time << endl;
    free_pieces.erase(piece_index);
    Piece *piece = &pieces[piece_index];
    for (auto &place : piece->places)
    {
        if (place.last_on < current_time) //this place is not availabe in crrent state
            continue;
        draw(*piece, place, true, piece_index);
        if (free_pieces.empty())
            return true;
        current_time++;
        place.last_on = current_time;
        ac3(piece_index);
        if (solve(choose_next_piece()))
            return true;
        draw(*piece, place, true, -1);
        place.last_on = current_time;
    }
    current_time--;
    free_pieces.insert(piece_index);
    //cerr << "return from" << piece_index << endl;
    return false;
}

int main()
{
    cin >> m >> n >> p;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            field[j][i] = exp_field[j][i] = -1;
    //--get the pieces
    for (int i = 0; i < p; i++)
    {
        Piece piece;
        cin >> piece.height >> piece.color;
        if (piece.height == -1)
            break;
        string next_pattern_line;
        for (int j = 0; j < piece.height; j++)
        {
            cin >> next_pattern_line;
            piece.pattern += next_pattern_line;
            piece.width = next_pattern_line.length();
        }
        pieces.push_back(piece);
        free_pieces.insert(i);
    }
    p = pieces.size();
    //---set places for the pieces
    for (auto &piece : pieces)
    {
        for (int x = 0; x < n; x++)
            for (int y = 0; y < m; y++)
                for (int dir = 0; dir < 4; dir++)
                {
                    Place place;
                    place.x = x;
                    place.y = y;
                    place.dir = dir;
                    place.last_on = 0;
                    if (draw(piece, place, false, 0))
                        piece.places.push_back(place);
                }
    }

    //---solving the piece
    clock_t begin = clock();

    bool solved = solve(choose_next_piece());

    clock_t end = clock();
    double passed_time = double(end - begin) / CLOCKS_PER_SEC;
    cerr << passed_time << " seconds passed.\n";

    if (solved)
        print_field();
    else
    {
        cout << "It is impossible." << endl;
    }
    print_color_map();
}