#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iterator>
using namespace std;

struct Point {
    int x, y;
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
int x_dir[10] = {0, 0,-1,-1,-1, 1, 1, 1};
int y_dir[10] = {1,-1, 0, 1,-1, 0, 1,-1};

int center(int x,int y)
{
    int re;
    re = min(abs(x-3),abs(x-4));
    re += min(abs(y-3),abs(y-4));
    return re;
}

int howgood(int x,int y)
{
    int value = 0;
    if(x==0 || x==7 || y==0 || y==7)
    {
        value += 5;
    }
    if((x==0 || x==7) && (y==0 || y==7))
    {
        value += 15;
    }
    if(x==6 || y==6 || x==1 || y==1)
    {
        value -= 5;
    }
    value += center(x,y);
    int cnt[10][10];
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            cnt[i][j]=0;
        }
    }
    for(int i=0;i<8;i++)
    {
        int px = x + x_dir[i];
        int py = y + y_dir[i];
        if(board[px][py]!=3-player)
        {
            continue;
        }
        while((0 <= px && px < 8 && 0 <= py && py < 8) && board[px][py]!=0)
        {
            if(board[px][py]==player)
            {
                break;
            }
            if(cnt[px][py]==0)
            {
                value += 2;
            }
            cnt[px][py]=1;
            px += x_dir[i];
            py += y_dir[i];
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

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Keep updating the output until getting killed.
    while (true) {
        // Choose random spot. (Not random uniform here)
        int best = 0;
        Point bestpoint = next_valid_spots[0];
        for(auto it : next_valid_spots)
        {
            int newvalue = howgood(it.x,it.y);
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

        // Remember to flush the output to ensure the last action is written to file.
        fout << bestpoint.x << " " << bestpoint.y << std::endl;
        fout.flush();
    }
}

/*
random

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

*/

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


//main player_nmsl.exe player_infinite.exe
