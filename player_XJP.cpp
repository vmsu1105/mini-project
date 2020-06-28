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
int eval;
int mineval;
int maxeval;

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
    int discs[3];
    vector<Point>next_valid;
    int newboard[8][8];
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
                newboard[i][j]=board[i][j];
                discs[board[i][j]]++;
            }
        }
    }
    State()
    {
        reset();
    }
    void reset()
    {
        for(int i=0;i<3;i++)
        {
            discs[i]=0;
        }
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                newboard[i][j]=EMPTY;
            }
        }
    }
};

Point best;
State first;
std::vector<Point> next_valid_spots;
const int boardvalue[10][10]=
{
{99,-6,5,2,2,5,-6,99},
{-6,-6,5,1,1,5,-6,-6},
{ 5, 5,3,1,1,3, 5, 5},
{ 2, 1,1,0,0,1, 1, 2},
{ 2, 1,1,0,0,1, 1, 2},
{ 5, 5,3,1,1,3, 5, 5},
{-6,-6,5,1,1,5,-6,-6},
{99,-6,5,2,2,5,-6,99}
};

const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point( 0, -1), /*{0, 0}, */  Point( 0, 1),
        Point( 1, -1), Point( 1, 0), Point( 1, 1)
    }};

int get_next_player(int player)
{
    return 3 - player;
}
bool is_spot_on_board(Point p)
{
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}
int get_disc(Point p,State st) {
    return st.newboard[p.x][p.y];
}
State set_disc(Point p, int disc,State st)
{
    st.newboard[p.x][p.y] = disc;
    return st;
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
State flip_discs(Point center,int cur_player,State st)
{
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player),st))
            continue;
        std::vector<Point> discss({p});
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p,st) != EMPTY)
        {
            if (is_disc_at(p, cur_player,st))
            {
                for (Point pp: discss)
                {
                    st = set_disc(pp, cur_player,st);
                }
                st.discs[cur_player] += discss.size();
                st.discs[get_next_player(cur_player)] -= discss.size();
                break;
            }
            discss.push_back(p);
            p = p + dir;
        }
    }
    return st;
}
std::vector<Point> get_valid_spots(int cur_player,State st)
{
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
        if (board[i][j] != EMPTY)
                continue;
        if (is_spot_valid(p,cur_player,st))
            valid_spots.push_back(p);
        }
    }
    return valid_spots;
}
int getvalue(State st)
{
    int  value=0;
    value += st.discs[player];
    for(int i=0;i<SIZE;i++)
    {
        for(int j=0;j<SIZE;j++)
        {
            if(st.newboard[i][j]==player)
            {
                value += boardvalue[i][j];
            }
        }
    }
    return value;
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

int minimax(State cur, int depth, int alpha, int beta,int curplayer)
{
    if(curplayer == player)
    {
        maxeval = -INF;
        for(Point p : cur.next_valid)
        {
            State tmp = cur;
            tmp=set_disc(p,curplayer,tmp);
            tmp=flip_discs(p,curplayer,tmp);
            tmp.next_valid=get_valid_spots(get_next_player(curplayer),tmp);
            tmp.n_valid_spots=tmp.next_valid.size();
            if(depth==0 || tmp.n_valid_spots==0)
                return getvalue(tmp);
            else
            {
                eval=minimax(tmp,depth-1,alpha,beta,get_next_player(curplayer));
                if(alpha < eval && depth==5)
                    best = p;
                alpha = max(alpha,eval);
                maxeval = max(maxeval,alpha);
            }
            if(beta <= alpha)
                break;
        }
        return maxeval;
    }
    else
    {
        mineval = INF;
        for(Point p:cur.next_valid)
        {
            mineval = INF;
            State tmp=cur;
            tmp=set_disc(p,curplayer,tmp);
            tmp=flip_discs(p,curplayer,tmp);
            tmp.next_valid=get_valid_spots(get_next_player(curplayer),tmp);
            tmp.n_valid_spots=tmp.next_valid.size();
            if(depth==0||tmp.n_valid_spots==0)
                return getvalue(tmp);
            else
            {
                eval = minimax(tmp,depth-1,alpha,beta,get_next_player(curplayer));
                beta = min(beta,eval);
                mineval = min(beta,eval);
            }
            if(beta <= alpha)
                break;
        }
        return mineval;
    }
}

void write_valid_spot(std::ofstream& fout) {
    for(int i=0;i<SIZE;i++)
    {
        for(int j=0;j<SIZE;j++)
        {
            first.newboard[i][j]=board[i][j];
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
