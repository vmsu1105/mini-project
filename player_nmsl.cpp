#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iterator>
using namespace std;

struct Point
{
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
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

std::array<int, 3> disc_count;

const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
std::array<std::array<int, SIZE>, SIZE> second_board;
std::vector<Point> second_valid_spots;
std::array<std::array<int, SIZE>, SIZE> third_board;
std::vector<Point> third_valid_spots;
int x_dir[10] = {0, 0,-1,-1,-1, 1, 1, 1};
int y_dir[10] = {1,-1, 0, 1,-1, 0, 1,-1};
int first_value;
int second_value;
int third_value;
int flag = 1;

int center(int x,int y)
{
    int re;
    re = min(abs(x-3),abs(x-4));
    re += min(abs(y-3),abs(y-4));
    return re;
}

int howmany()
{
    int me = 0;
    int newme = 0;
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            if(third_board[i][j]==player)
            {
                me++;
            }
            if(board[i][j]==player)
            {
                newme++;
            }
        }
    }
    int re;
    re = newme-me;
    return re;
}

int howgood(int x,int y)
{
    int value = 0;
    if(x==0 || x==7 || y==0 || y==7)
    {
        value += 2;
    }
    if((x==0 || x==7) && (y==0 || y==7))
    {
        value += 10;
    }
    if(x==6 || y==6 || x==1 || y==1)
    {
        value -= 3;
    }
    value += center(x,y);
    return value;
}

bool is_spot_valid(Point center)
{
    if(flag==1)
    {
        if (board[center.x][center.y] != 0)
            return false;
        for (Point dir: directions)
        {
            // Move along the direction while testing.
            Point p = center + dir;
            if (board[p.x][p.y]!=player)
                continue;
            p = p + dir;
            while ((0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE) && board[p.x][p.y]!=0)
            {
                if (board[p.x][p.y]==3-player)
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    else if(flag==2)
    {
        if (second_board[center.x][center.y] != 0)
            return false;
        for (Point dir: directions)
        {
            // Move along the direction while testing.
            Point p = center + dir;
            if (second_board[p.x][p.y]!=player)
                continue;
            p = p + dir;
            while ((0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE) && second_board[p.x][p.y]!=0)
            {
                if (second_board[p.x][p.y]==3-player)
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    else if(flag==3)
    {
        if (third_board[center.x][center.y] != 0)
            return false;
        for (Point dir: directions)
        {
            // Move along the direction while testing.
            Point p = center + dir;
            if (third_board[p.x][p.y]!=player)
                continue;
            p = p + dir;
            while ((0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE) && third_board[p.x][p.y]!=0)
            {
                if (third_board[p.x][p.y]==3-player)
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
}

std::vector<Point> get_valid_spots()
{
    if(flag==1)
    {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                Point p = Point(i, j);
                if (board[i][j] != 0)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    else if(flag==2)
    {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                Point p = Point(i, j);
                if (second_board[i][j] != 0)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    else if(flag==3)
    {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                Point p = Point(i, j);
                if (third_board[i][j] != 0)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
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

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    if(flag==1)
    {
        for (int i = 0; i < n_valid_spots; i++)
        {
            fin >> x >> y;
            next_valid_spots.push_back({x, y});
        }
    }
    else if(flag==2)
    {
        for (int i = 0; i < n_valid_spots; i++)
        {
            fin >> x >> y;
            second_valid_spots.push_back({x, y});
        }
    }
    else if(flag==3)
    {
        for (int i = 0; i < n_valid_spots; i++)
        {
            fin >> x >> y;
            third_valid_spots.push_back({x, y});
        }
    }
}

Point cul()
{
    int newvalue = 0;
    srand(time(NULL));
    while (true)
    {
        flag=1;
        int best = 0;
        Point bestpoint(next_valid_spots[0]);
        for(auto it : next_valid_spots)
        {
            if(it.x==0 || it.x==7 || it.y==0 || it.y==7)
            {
                newvalue += 8;
            }
            if((it.x==0 || it.x==7) && (it.y==0 || it.y==7))
            {
                return it;
            }
            if(it.x==6 || it.y==6 || it.x==1 || it.y==1)
            {
                newvalue -= 5;
            }
            for(int i=0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                    second_board[i][j] = board[i][j];
                }
            }
            flag = 2;
            second_board[it.x][it.y]=player;
            player = 3 - player;
            second_valid_spots = get_valid_spots();
            for(auto sec_it : second_valid_spots)
            {
                for(int i=0;i<8;i++)
                {
                    for(int j=0;j<8;j++)
                    {
                        third_board[i][j]=second_board[i][j];
                    }
                }
                flag = 3;
                third_board[sec_it.x][sec_it.y]=player;
                player = 3 - player;
                third_valid_spots=get_valid_spots();
                for(auto thr_it : third_valid_spots)
                {
                    newvalue = howmany() + howgood(thr_it.x,thr_it.y);
                    if(newvalue == best)
                    {
                        if(it.x == 0 || it.x == 7 || it.y == 0 || it.y ==7)
                        {
                            best = newvalue;
                            bestpoint = it;
                        }
                    }
                    else if(newvalue > best)
                    {
                        best = newvalue;
                        bestpoint = it;
                    }
                }
            }
        }
        return bestpoint;
    }
}

void write_valid_spot(std::ofstream& fout) {
    int value;
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Keep updating the output until getting killed.
    // Choose random spot. (Not random uniform here)
    Point bestpoint = cul();
    // Remember to flush the output to ensure the last action is written to file.
    fout << bestpoint.x << " " << bestpoint.y << std::endl;
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


//main player_nmsl.exe player_random.exe
