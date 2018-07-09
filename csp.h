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
