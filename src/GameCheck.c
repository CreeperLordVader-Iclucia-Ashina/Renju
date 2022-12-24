//
// Created by creeper on 22-12-22.
//

#include "GameCheck.h"
#include "GameCore.h"
#include "DataStructure.h"
#include <stdlib.h>
#define INSIDE(x,y) ((x) >= 0 && (y) >= 0 && (x) < SIZE && (y) < SIZE)
#define STEP(x, step, dir) (x+(step)*(d##x[dir]))
#define isEmpty(step) (!(local_l & (3 << ((idx + i + (step)) << 1))))
#define PLACEABLE(step) (INSIDE(STEP(x, i + (step), k), STEP(y, i + (step), k)) && isEmpty(step))
#define BLACK_PLACEABLE(x, y) (rand() > RUSSIAN_ROULLETE ? is_placeable(x, y) : black_point[(x)][(y)] >= 0)
#define BLACK_LONG_MASK 4095
#define EBBBE 252//three black stones in a row, one possible situation for a live three
#define BBBB 255//one possible situation for a four
#define EBBEBE 828
#define EBEBBE 972//another two possible situations
#define BBEBB 975
#define BEBBB 1011
#define BBBEB 831
#define BEB 51
#define BB 15
#define EWWWE 168
#define WWWWE 170
#define EWWWW 340
#define EWWEWE 552
#define EWEWWE 648
#define WEWWW 674
#define WWEWW 650
#define WWWEW 554
#define WEW 34
#define WW 10

static const int BLACK_DOUBLE_FOUR_SAME_LINE[3] = {13299, 62415, 258879};
//11001111110011, 1111001111001111, 111111001100111111
const int dx[] = {0, 1, 1, -1};
const int dy[] = {1, 0, 1, 1};
struct State* global_state;// this is shared by ForbidCheck.c and GameAI.c
extern const int INF;
extern int white_point[][ALIGNMENT], black_point[][ALIGNMENT];
const double PROBABILITY_RUSSIAN_ROULLETE = 0.05;
const int RUSSIAN_ROULLETE = (int)(32767 * PROBABILITY_RUSSIAN_ROULLETE);

bool forbid(int x, int y);
bool is_placeable(int x, int y)
{
    PLACE(global_state, x, y, BLACK);
    if(!forbid(x, y))
    {
        CLEAR(global_state, x, y);
        return true;
    }
    else
    {
        CLEAR(global_state, x, y);
        return false;
    }
}
int checkLiveThree(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-2, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == EBBBE)//find a BBBE at [idx + i, idx + i + 3]
            {
                if(PLACEABLE(-1))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i - 1, k), STEP(y, i - 1, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i - 1, k), STEP(y, i - 1, k)))//can form a four at [idx + i - 1, idx + i + 2]
                    {
                        if(PLACEABLE(-2) && PLACEABLE(3))
                        {
                            if(BLACK_PLACEABLE(STEP(x, i - 2, k), STEP(y, i - 2, k))
                               && is_placeable(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                            {
                                find = true;
                                CLEAR_CROSS(cross_to_be_placed);
                                break;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
                if(PLACEABLE(3))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 3, k), STEP(y, i + 3, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                    {
                        if(PLACEABLE(4))//can form a four at [idx + i, idx + i + 3]
                        {
                            if(is_placeable(STEP(x, i - 1, k), STEP(y, i - 1, k))
                               && is_placeable(STEP(x, i + 4, k), STEP(y, i + 4, k)))
                            {
                                find = true;
                                CLEAR_CROSS(cross_to_be_placed);
                                break;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
            }
        }
        if(find)
        {
            cnt++;
            if(cnt == 2) return 2;
            else find = false;
        }
        for(int i = max(-4, -idx); i <= min(-1, SIZE - 6 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 6) == EBBEBE)//find pattern EBBEBE at [i, i + 5]
            {
                if(PLACEABLE(3))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 3, k), STEP(y, i + 3, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 3, k), STEP(y, i + 3, k)))//can form a four at [idx + i + 1, idx + i + 4]
                    {
                        if(PLACEABLE(0) && PLACEABLE(5))
                        {
                            if(is_placeable(STEP(x, i, k), STEP(y, i, k))
                               && is_placeable(STEP(x, i + 5, k), STEP(y, i + 5, k)))
                            {
                                find = true;
                                CLEAR_CROSS(cross_to_be_placed);
                                break;
                            }
                        }
                        CLEAR_CROSS(cross_to_be_placed);
                    }
                }
            }
            else if(checkGrids(local_l, idx + i, 6) == EBEBBE)
            {
                if(PLACEABLE(2))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 2, k), STEP(y, i + 2, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 2, k), STEP(y, i + 2, k)))//can form a four at [idx + i, idx + i + 3]
                    {
                        if(PLACEABLE(0) && PLACEABLE(5))
                        {
                            if(is_placeable(STEP(x, i, k), STEP(y, i, k))
                               && is_placeable(STEP(x, i + 5, k), STEP(y, i + 5, k)))
                            {
                                find = true;
                                CLEAR_CROSS(cross_to_be_placed);
                                break;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
            }
        }
        if(find)
        {
            cnt++;
            if(cnt == 2) return 2;
        }
    }
    return cnt;
}

int checkFour(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-3, -idx); i <= min(0, SIZE - 3 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 4) == BBBB)//find a BBBB at [idx + i, idx + i + 3]
            {
                if(PLACEABLE(-1) && is_placeable(STEP(x, i - 1, k), STEP(y, i - 1, k)))
                {
                    find = true;
                    break;
                }
                else if(PLACEABLE(4) && is_placeable(STEP(x, i + 4, k), STEP(y, i + 4, k)))
                {
                    find = true; //can form a five at [idx + i, idx + i + 4]
                    break;
                }
            }
        }
        if(find)
        {
            cnt++;
            if(cnt == 2) return 2;
            else find = false;
        }
        for(int i = max(-4, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == BEBBB && PLACEABLE(1))//find pattern BEBBB at [i, i + 4]
            {
                if(is_placeable(STEP(x, i + 1, k), STEP(y, i + 1, k)))
                {
                    find = true;
                    break;
                }
            }
            if(checkGrids(local_l, idx + i, 5) == BBEBB && PLACEABLE(2))
            {
                if(is_placeable(STEP(x, i + 2, k), STEP(y, i + 2, k)))
                {
                    find = true;
                    break;
                }
            }
            if(checkGrids(local_l, idx + i, 5) == BBBEB && PLACEABLE(3))
            {
                if(is_placeable(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                {
                    find = true;
                    break;
                }
            }
        }
        if(find)
        {
            cnt++;
            if(cnt == 2)return 2;
        }
    }
    return cnt;
}

/**
 * Checks whether the black stone placed at (x, y) forms a forbidden move.
 * To guarantee the accuracy of this check, I sacrifice much efficiency to make it robust.
 * However, this function can't be used in AI. To guarantee the efficiency of AI, the forbid checks should be as fast as
 * possible.
 */
bool forbid(int x, int y)
{
    //first consider the forbidden move formed in one direction, including the long forbid
    if(searchPatternAroundPointGreedy(global_state, x, y, 6, BLACK_LONG_MASK)) return true;
    if(searchPatternAroundPoint(global_state, x, y, RENJU_LENGTH, BLACK_RENJU_MASK)) return false;
    if(searchPatternAroundPoint(global_state, x, y, 7, BLACK_DOUBLE_FOUR_SAME_LINE[0])) return true;
    if(searchPatternAroundPoint(global_state, x, y, 8, BLACK_DOUBLE_FOUR_SAME_LINE[1])) return true;
    if(searchPatternAroundPoint(global_state, x, y, 9, BLACK_DOUBLE_FOUR_SAME_LINE[2])) return true;
    //now we can focus on the forbidden move formed in 2 directions
    //this can only be double-3 and double-4, first check double-3
    if(checkLiveThree(x, y) == 2) return true;
    //finally check double-4
    if(checkFour(x, y) == 2) return true;
    return false;
}

void estimateBlackPoints(int x, int y)
{
    PLACE(global_state, x, y, BLACK);
    if(searchPatternAroundPointGreedy(global_state, x, y, 6, BLACK_LONG_MASK)) black_point[x][y] = -INF;
    else if(searchPatternAroundPoint(global_state, x, y, RENJU_LENGTH, BLACK_RENJU_MASK)) black_point[x][y] = FORM_RENJU;
    else if(searchPatternAroundPoint(global_state, x, y, 7, BLACK_DOUBLE_FOUR_SAME_LINE[0])) black_point[x][y] = -INF;
    else if(searchPatternAroundPoint(global_state, x, y, 8, BLACK_DOUBLE_FOUR_SAME_LINE[1])) black_point[x][y] = -INF;
    else if(searchPatternAroundPoint(global_state, x, y, 9, BLACK_DOUBLE_FOUR_SAME_LINE[2])) black_point[x][y] = -INF;
    else
    {
        int cnt = checkLiveThree(x, y);
        if(cnt == 2) black_point[x][y] = -INF;
        else
        {
            black_point[x][y] = cnt * FORM_LIVE_THREE;
            cnt = checkFour(x, y);
            black_point[x][y] += cnt * FORM_FOUR;
        }
    }
    CLEAR(global_state, x, y);
}

bool checkForbid(struct State* const state, int x, int y)
{
    global_state = state;
    return forbid(x, y);
}

bool destroyBlackLiveThree(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        for(int i = max(-2, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == EBBBE)//find a BBBE at [idx + i, idx + i + 3]
            {
                if(PLACEABLE(-1))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i - 1, k), STEP(y, i - 1, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i - 1, k), STEP(y, i - 1, k)))//can form a four at [idx + i - 1, idx + i + 2]
                    {
                        if(PLACEABLE(-2) && PLACEABLE(3))
                        {
                            if(BLACK_PLACEABLE(STEP(x, i - 2, k), STEP(y, i - 2, k))
                               && BLACK_PLACEABLE(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                            {
                                white_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] += DESTROY_LIVE_THREE;
                                CLEAR_CROSS(cross_to_be_placed);
                                return true;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
                if(PLACEABLE(3))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 3, k), STEP(y, i + 3, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                    {
                        if(PLACEABLE(-1) && PLACEABLE(4))//can form a four at [idx + i, idx + i + 3]
                        {
                            if(BLACK_PLACEABLE(STEP(x, i - 1, k), STEP(y, i - 1, k))
                               && BLACK_PLACEABLE(STEP(x, i + 4, k), STEP(y, i + 4, k)))
                            {
                                white_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_LIVE_THREE;
                                CLEAR_CROSS(cross_to_be_placed);
                                return true;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
            }
        }
        for(int i = max(-4, -idx); i <= min(-1, SIZE - 6 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 6) == EBBEBE)//find pattern EBBEBE at [i, i + 5]
            {
                if(PLACEABLE(3))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 3, k), STEP(y, i + 3, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 3, k), STEP(y, i + 3, k)))//can form a four at [idx + i + 1, idx + i + 4]
                    {
                        if(PLACEABLE(0) && PLACEABLE(5))
                        {
                            if(BLACK_PLACEABLE(STEP(x, i, k), STEP(y, i, k))
                               && BLACK_PLACEABLE(STEP(x, i + 5, k), STEP(y, i + 5, k)))
                            {
                                white_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_LIVE_THREE;
                                CLEAR_CROSS(cross_to_be_placed);
                                return true;
                            }
                        }
                        CLEAR_CROSS(cross_to_be_placed);
                    }
                }
            }
            else if(checkGrids(local_l, idx + i, 6) == EBEBBE)
            {
                if(PLACEABLE(2))
                {
                    struct Cross cross_to_be_placed = takeCross(global_state, STEP(x, i + 2, k), STEP(y, i + 2, k));
                    PLACE_CROSS(cross_to_be_placed, BLACK);
                    if(!forbid(STEP(x, i + 2, k), STEP(y, i + 2, k)))//can form a four at [idx + i, idx + i + 3]
                    {
                        if(PLACEABLE(0) && PLACEABLE(5))
                        {
                            if(BLACK_PLACEABLE(STEP(x, i, k), STEP(y, i, k))
                               && BLACK_PLACEABLE(STEP(x, i + 5, k), STEP(y, i + 5, k)))
                            {
                                white_point[STEP(x, i + 2, k)][STEP(y, i + 2, k)] += DESTROY_LIVE_THREE;
                                CLEAR_CROSS(cross_to_be_placed);
                                return true;
                            }
                        }
                    }
                    CLEAR_CROSS(cross_to_be_placed);
                }
            }
        }
    }
    return false;
}

bool destroyBlackFour(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        for(int i = max(-3, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 4) == BBBB)//find a BBBB at [idx + i, idx + i + 3]
            {
                if(PLACEABLE(-1) && black_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] >= 0)
                {
                    white_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] += DESTROY_FOUR;
                    return true;
                }
                else if(PLACEABLE(4) && black_point[STEP(x, i + 4, k)][STEP(y, i + 4, k)] >= 0)
                {
                    white_point[STEP(x, i + 4, k)][STEP(y, i + 4, k)] += DESTROY_FOUR;
                    return true;
                }
            }
        }
        for(int i = max(-4, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == BEBBB)//find pattern BEBBB at [i, i + 4]
            {
                if(black_point[STEP(x, i + 1, k)][STEP(y, i + 1, k)] >= 0)
                {
                    white_point[STEP(x, i + 1, k)][STEP(y, i + 1, k)] += DESTROY_FOUR;
                    return true;
                }
            }
            if(checkGrids(local_l, idx + i, 5) == BBEBB)
            {
                if(black_point[STEP(x, i + 2, k)][STEP(y, i + 2, k)] >= 0)
                {
                    white_point[STEP(x, i + 2, k)][STEP(y, i + 2, k)] += DESTROY_FOUR;
                    return true;
                }
            }
            if(checkGrids(local_l, idx + i, 5) == BBBEB)
            {
                if(black_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] >= 0)
                {
                    white_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_FOUR;
                    return true;
                }
            }
        }
    }
    return false;
}

int countWhiteLiveThree(struct Cross cross)
{
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-3, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == EWWWE)//find a EWWWE at [idx + i, idx + i + 3]
            {
                find = true;
                break;
            }
        }
        if(find)
        {
            cnt++;
            break;
        }
        for(int i = max(-4, -idx); i <= min(-1, SIZE - 6 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 6) == EWWEWE)
            {
                find = true;
                break;
            }
            else if(checkGrids(local_l, idx + i, 6) == EWEWWE)
            {
                find = true;
                break;
            }
        }
        if(find) cnt++;
    }
    return cnt;
}

int countWhiteFour(struct Cross cross)
{
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-3, -idx); i <= min(0, SIZE - 3 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 4) == EWWWW)//find a BBBB at [idx + i, idx + i + 3]
            {
                find = true;
                break;
            }
        }
        if(find)
        {
            cnt++;
            break;
        }
        for(int i = max(-3, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 4) == WWWWE)
            {
                find = true; //can form a five at [idx + i, idx + i + 4]
                break;
            }
        }
        for(int i = max(-4, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == WEWWW)//find pattern BEBBB at [i, i + 4]
            {
                find = true;
                break;
            }
            if(checkGrids(local_l, idx + i, 5) == WWEWW)
            {
                find = true;
                break;
            }
            if(checkGrids(local_l, idx + i, 5) == WWWEW)
            {
                find = true;
                break;
            }
        }
        if(find)cnt++;
    }
    return cnt;
}

bool checkWhiteRenju(struct Cross cross)
{
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        for(int i = max(-4, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, RENJU_LENGTH) == WHITE_RENJU_MASK)
                return true;
        }
    }
    return false;
}

int countWhiteTwo(struct Cross cross)
{
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-2, -idx); i <= SIZE - 3 - idx; i++)
        {
            if(checkGrids(local_l, idx + i, 3) == WEW)
            {
                find = true;
                break;
            }
        }
        if(find)
        {
            cnt++;
            break;
        }
        for(int i = max(-1, -idx); i <= SIZE - 2 - idx; i++)
        {
            if(checkGrids(local_l, idx + i, 2) == WW)
            {
                find = true;
                break;
            }
        }
        if(find) cnt++;
    }
    return cnt;
}

bool destroyWhiteLiveThree(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        for(int i = max(-3, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == EWWWE)//find a BBBE at [idx + i, idx + i + 3]
            {
                if(PLACEABLE(-1))
                {
                    if(black_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] >= 0)//can form a four at [idx + i - 1, idx + i + 2]
                    {
                        if(PLACEABLE(-2))
                        {
                            if(black_point[STEP(x, i - 2, k)][STEP(y, i - 2, k)] >= 0
                               && black_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] >= 0)
                            {
                                black_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] += DESTROY_LIVE_THREE;
                                return true;
                            }
                        }
                    }
                }
                if(PLACEABLE(3))
                {
                    if(!forbid(STEP(x, i + 3, k), STEP(y, i + 3, k)))
                    {
                        if(PLACEABLE(4))//can form a four at [idx + i, idx + i + 3]
                        {
                            if(black_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] >= 0
                               && black_point[STEP(x, i + 4, k)][STEP(y, i + 4, k)] >= 0)
                            {
                                black_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_LIVE_THREE;
                                return true;
                            }
                        }
                    }
                }
            }
        }
        for(int i = max(-4, -idx); i <= min(-1, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 6) == EWWEWE)//find pattern EBBEBE at [i, i + 5]
            {
                if(PLACEABLE(3))
                {
                    black_point[STEP(x, i, k)][STEP(y, i, k)] += DESTROY_LIVE_THREE;
                    black_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_LIVE_THREE;
                    black_point[STEP(x, i + 5, k)][STEP(y, i + 5, k)] += DESTROY_LIVE_THREE;
                    return true;
                }
            }
            else if(checkGrids(local_l, idx + i, 6) == EWEWWE)
            {
                if(PLACEABLE(2))
                {
                    black_point[STEP(x, i, k)][STEP(y, i, k)] += DESTROY_LIVE_THREE;
                    black_point[STEP(x, i + 2, k)][STEP(y, i + 2, k)] += DESTROY_LIVE_THREE;
                    black_point[STEP(x, i + 5, k)][STEP(y, i + 5, k)] += DESTROY_LIVE_THREE;
                    return true;
                }
            }
        }
    }
    return false;
}

bool destroyWhiteFour(int x, int y)
{
    struct Cross cross = takeCross(global_state, x, y);
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        for(int i = max(-4, -idx); i <= min(0, SIZE - 4 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == EWWWW)
            {
                black_point[STEP(x, i - 1, k)][STEP(y, i - 1, k)] += DESTROY_FOUR;
                return true;
            }
        }
        for(int i = max(-3, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == WWWWE)
            {
                black_point[STEP(x, i + 4, k)][STEP(y, i + 4, k)] += DESTROY_FOUR;
                return true;
            }
        }
        for(int i = max(-4, -idx); i <= min(0, SIZE - 5 - idx); i++)
        {
            if(checkGrids(local_l, idx + i, 5) == WEWWW)//find pattern BEBBB at [i, i + 4]
            {
                black_point[STEP(x, i + 1, k)][STEP(y, i + 1, k)] += DESTROY_FOUR;
                return true;
            }
            if(checkGrids(local_l, idx + i, 5) == WWEWW)
            {
                black_point[STEP(x, i + 2, k)][STEP(y, i + 2, k)] += DESTROY_FOUR;
                return true;
            }
            if(checkGrids(local_l, idx + i, 5) == WWWEW)
            {
                black_point[STEP(x, i + 3, k)][STEP(y, i + 3, k)] += DESTROY_FOUR;
                return true;
            }
        }
    }
    return false;
}

int countBlackTwo()
{
    int cnt = 0;
    for(int k = 0; k < DIR_NUM; k++)
    {
        int local_l = *(cross.line[k]);
        int idx = cross.idx[k];
        bool find = false;
        for(int i = max(-2, -idx); i <= SIZE - 3 - idx; i++)
        {
            if(checkGrids(local_l, idx + i, 3) == BEB && black_point[STEP(x, i + 1, k)][STEP(y, i + 1, k)] >= 0)
            {
                find = true;
                break;
            }
        }
        if(find)
        {
            cnt++;
            break;
        }
        for(int i = max(-1, -idx); i <= SIZE - 2 - idx; i++)
        {
            if(checkGrids(local_l, idx + i, 2) == BB)
            {
                find = true;
                break;
            }
        }
        if(find) cnt++;
    }
    return cnt;
}
#undef isEmpty
#undef STEP
#undef PLACEABLE