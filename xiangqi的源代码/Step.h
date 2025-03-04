#ifndef STEP_H
#define STEP_H

#include"Gamescene.h"
class Step
{
public:
    Step();

    int _id;
    int tocol;//要去的行和列
    int torow;
    int _killid;
    int lastcol;//原来的行和列
    int lastrow;

    int lastfight;//原来的战斗力
    int tofight;//要去的战斗力

    //void getLastrowcol(int id);
};

#endif // STEP_H
