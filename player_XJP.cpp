#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iterator>
using namespace std;

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;

int minmax_cnt;
const int EMPTY = 0;
const int INF = 10000000;

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(int x, int y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}

};
class State{
    public:
    int n_valid_spots;
    vector<Point>next_valid;
    int bd[8][8];
    array<int,3>discs;
    State(std::array<std::array<int, SIZE>, SIZE> board)
    {
        for(int i=0;i<3;i++)
        {
            discs[i]=0;
        }
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                bd[i][j]=board[i][j];
                discs[board[i][j]]++;
            }
        }
    }
    State(){
        for(int i=0;i<3;i++)
        {
            discs[i]=0;
        }
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                bd[i][j]=0;
            }
        }
    }
};

Point best;
State first;
std::vector<Point> next_valid_spots;
const int boardvalue[10][10]=
{
{20,-4,3,2,2,3,-4,20},
{-4,-4,3,1,1,3,-4,-4},
{ 3, 3,2,1,1,2, 3, 3},
{ 2, 1,1,0,0,1, 1, 2},
{ 2, 1,1,0,0,1, 1, 2},
{ 3, 3,2,1,1,2, 3, 3},
{-4,-4,3,1,1,3,-4,-4},
{20,-4,3,2,2,3,-4,20}
};

const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};

int get_next_player(int player)
{
    return 3 - player;
}
bool is_spot_on_board(Point p)
{
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
int get_disc(Point p,State s) {
    return s.bd[p.x][p.y];
}
State set_disc(Point p, int disc,State s)
{
    s.bd[p.x][p.y] = disc;
    return s;
}
bool is_disc_at(Point p, int disc,State s)
{
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(p,s) != disc)
        return false;
    return true;
}
bool is_spot_valid(Point center,int cur_player,State s)
{
    if (get_disc(center,s) != EMPTY)
        return false;
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player),s))
            continue;
    p = p + dir;
        while (is_spot_on_board(p) && get_disc(p,s) != EMPTY) {
            if (is_disc_at(p, cur_player,s))
                return true;
            p = p + dir;
        }
    }
    return false;
}
State flip_discs(Point center,int cur_player,State s)
{
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player),s))
            continue;
        std::vector<Point> discss({p});
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p,s) != EMPTY)
        {
            if (is_disc_at(p, cur_player,s))
            {
                for (Point pp: discss)
                {
                    s = set_disc(pp, cur_player,s);
                }
                s.discs[cur_player] += discss.size();
                s.discs[get_next_player(cur_player)] -= discss.size();
                break;
            }
            discss.push_back(p);
            p = p + dir;
        }
    }
    return s;
}
std::vector<Point> get_valid_spots(int cur_player,State s)
{
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
        if (board[i][j] != EMPTY)
                continue;
        if (is_spot_valid(p,cur_player,s))
            valid_spots.push_back(p);
        }
    }
    return valid_spots;
}
int getvalue(State s,int curplayer)
{
    int  value=0;
    value += s.discs[player];
    for(int i=0;i<SIZE;i++)
    {
        for(int j=0;j<SIZE;j++)
        {
            if(s.bd[i][j]==player)
            {
                value += boardvalue[i][j];
            }
        }
    }
    return value;
}

int minimax(State cur, int depth, int alpha, int beta,int curplayer)
{
    if(curplayer == player)
    {
        for(Point p : cur.next_valid)
        {
            State tmp = cur;
            tmp = set_disc(p,curplayer,tmp);
            tmp = flip_discs(p,curplayer,tmp);
            tmp.next_valid = get_valid_spots(get_next_player(curplayer),tmp);
            tmp.n_valid_spots = tmp.next_valid.size();
            if(depth==0 || tmp.n_valid_spots==0)
                return getvalue(tmp,player);
            else
            {
                int value = minimax(tmp,depth-1,alpha,beta,get_next_player(curplayer));
                if(alpha < value && depth==5)
                    best = p;
                alpha=max(alpha,value);
            }
            if(beta <= alpha)
                break;
        }
        return alpha;
    }
    else
    {
        for(Point p:cur.next_valid)
        {
            State tmp = cur;
            tmp = set_disc(p,curplayer,tmp);
            tmp = flip_discs(p,curplayer,tmp);
            tmp.next_valid = get_valid_spots(get_next_player(curplayer),tmp);
            tmp.n_valid_spots = tmp.next_valid.size();
            if(depth==0||tmp.n_valid_spots==0)
                return getvalue(tmp,player);
            else beta = min(beta,minimax(tmp,depth-1,alpha,beta,get_next_player(curplayer)));
            if(beta <= alpha)
                break;
        }
        return beta;
    }
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}
int nn_valid_spot;
void read_valid_spots(std::ifstream& fin) {
    fin >> nn_valid_spot;
    int x, y;
    for (int i = 0; i < nn_valid_spot; i++) {
        fin >> x >> y;
        first.next_valid.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    for(int i=0;i<SIZE;i++)
    {
        for(int j=0;j<SIZE;j++)
        {
            first.bd[i][j]=board[i][j];
        }
    }
    srand(time(NULL));
    int ran = (rand() % first.next_valid.size());
    best = first.next_valid[ran];
    fout << best.x << " " << best.y << std::endl;
    fout.flush();
    minimax(first, 5,-INF,INF,player);
    fout << best.x << " " << best.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}


//main player_XJP.exe player_random.exe
