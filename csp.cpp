#include<iostream>
#include<vector>
#include<string>
#include<set>
#define MAX_DIM 500
using namespace std;
int current_time = 0;
int m, n, p;
int field[MAX_DIM][MAX_DIM];


struct Place{
    int x;
    int y;
    int dir;
    int last_on;
};

struct Piece{
    int width;
    int height;
    int color;
    string pattern;
    vector<Place> places;
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
    for(int j = 0; j<m; j++){
        for(int i = 0; i<n; i++){
            cout << field[j][i]+1<<" ";
        }
        cout << endl;
    }
}

void prune(int piece_index){
   Piece* piece = &pieces[piece_index];
    for(auto &place : piece->places){
        if(draw(*piece, place, false, piece_index))
            place.last_on = current_time;
    } 
}

bool solve(int piece_index){
    Piece* piece = &pieces[piece_index];
    for(auto &place : piece->places){
        if(place.last_on<current_time)
            continue;
        if(!draw(*piece, place, true, piece_index)){
            cout <<"shit" <<piece_index << endl;
        }
        if(piece_index==p-1)
            return true;   
        current_time++;
        prune(piece_index+1);
        if(solve(piece_index+1))
            return true;
        draw(*piece, place, true, -1);
        place.last_on = current_time-1;
    }
    current_time--;
    return false;
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
        string next_pattern_line;
        for(int j = 0; j<piece.height;j++){
            cin >> next_pattern_line;
            piece.pattern += next_pattern_line;
            piece.width = next_pattern_line.length();
        }
        pieces.push_back(piece);
    }
    //---set places for the pieces
    for(auto &piece : pieces){
        for(int x = 0; x<n; x++)
            for(int y = 0; y<m; y++)
                for(int dir = 0; dir<4; dir++){
                    Place place;
                    place.x = x;
                    place.y = y;
                    place.dir = dir;
                    place.last_on = 0;
                    if(draw(piece, place, false, 0))
                        piece.places.push_back(place);
                }
    }
    if(solve(0))
        print_field();
    else{
        cout<<"It is impossible."<<endl;
    }
}