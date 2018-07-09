#include <string>
#include <vector>
using namespace std;
struct Place
{
    int x;
    int y;
    int dir;
    int last_on;
};

struct Piece
{
    int width;
    int height;
    int color;
    string pattern;
    vector<Place> places;
};

struct Collision
{
    int p1;
    int p2;
    Place place1;
    Place place2;
    size_t hash;

    bool operator==(const Collision &b) const
    {
        return ((p1 == b.p1) && (p2 == b.p2) && (p1 == b.p1) && (place1.x == b.place1.x) &&
                (place2.x == b.place2.x) && (place1.y == b.place1.y) && (place2.y == b.place2.y) &&
                (place1.dir == b.place1.dir) && (place2.dir == b.place2.dir));
    };
};

struct Collision_Hasher
{
    size_t operator()(const Collision &b) const
    {
        return b.hash;
    };
};
