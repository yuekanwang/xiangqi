#include"Stone.h"

struct Pos
{
    int _row;
    int _col;
    Stone::TY _ty;
    int _fight;
};
Stone::Stone()
{

}

Stone::~Stone()
{

}

int kong=25;//能走的空格所加的战斗力

Pos pos[16]={
    {0,0,Stone::CHE,kong*2},
    {0,1,Stone::MA,kong*2},
    {0,2,Stone::XIANG,kong*2},
    {0,3,Stone::SHI,kong*2},
    {0,4,Stone::JIANG,kong},
    {0,5,Stone::SHI,kong*2},
    {0,6,Stone::XIANG,kong*2},
    {0,7,Stone::MA,kong*2},
    {0,8,Stone::CHE,kong*2},

    {2,1,Stone::PAO,kong*11+300},
    {2,7,Stone::PAO,kong*11+300},

    {3,0,Stone::BING,kong},
    {3,2,Stone::BING,kong},
    {3,4,Stone::BING,kong},
    {3,6,Stone::BING,kong},
    {3,8,Stone::BING,kong},
};

void Stone::initialize(int id)//根据棋子的id，给每个棋子赋予他的初始状态
{
    if(id<16)
    {
        row=pos[id]._row;
        col=pos[id]._col;
        ty=pos[id]._ty;
        red=true;
        fight=pos[id]._fight;
    }
    else
    {
        row=9-pos[id-16]._row;
        col=8-pos[id-16]._col;
        ty=pos[id-16]._ty;
        fight=pos[id-16]._fight;
        red=false;
    }
    death=false;

}

QString Stone::getText(bool isRed)
{
    QString a;
    if(isRed){
        switch (ty) {
        case CHE:
            return "车";
        case MA:
            return "马";
        case PAO:
            return "炮";
        case BING:
            return "兵";
        case JIANG:
            return "帅";
        case SHI:
            return "士";
        case XIANG:
            return "象";
        default:
            return "null";
        }
    }
    else{

        switch (ty) {
        case CHE:
            return "車";
        case MA:
            return "馬";
        case PAO:
            return "砲";
        case BING:
            return "卒";
        case JIANG:
            return "將";
        case SHI:
            return "仕";
        case XIANG:
            return "相";
        default:
            return "null";
        }
    }
    return "ERROR";

    return a;
}

