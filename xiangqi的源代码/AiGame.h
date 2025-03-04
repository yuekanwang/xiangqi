#ifndef AIGAME_H
#define AIGAME_H

#include"Step.h"
#include"Gamescene.h"

class AiGame : public Gamescene
{
public:
    int lastrow;
    int lastcol;
    int lastfight;
    int _level;

    int kong;
    AiGame();
    ~AiGame();
    // void StoneMove(int row,int col);//移动函数
    void mousePressEvent(QMouseEvent *event);//重载点击函数
    Step* AiMove();//机器走棋

    void getAllcanMove(QVector<Step*>& stones);//尝试所有能走的棋子
    void fakeMove(Step *stone);//一步尝试
    void unfakeMove(Step *stone);//一步退回
    int getScore();//获得棋子分数
    void saveStone(int id,int killid,int row,int col,QVector<Step*> &stone);

    int getMinScore(int level,int curmax);//获取下一步走后的最小（黑减红）棋子分数
    int getMaxScore(int level,int curmin);//获取下一步步走后的最大（黑减红）棋子分数

    void Look_walkablelocation(int id,QVector<Step*> &stone);//寻找可走的位置
    void Look_Jianglocation(int id,QVector<Step*> &stone);//将
    void Look_Xianglocation(int id,QVector<Step*> &stone);//象
    void Look_Shilocation(int id,QVector<Step*> &stone);//士
    void Look_Binglocation(int id,QVector<Step*> &stone);//兵
    void Look_Chelocation(int id,QVector<Step*> &stone);//车
    void Look_Paolocation(int id,QVector<Step*> &stone);//炮
    void Look_Malocation(int id,QVector<Step*> &stone);//马

    void trying(int id,int killid,int row,int col,QVector<Step*> &stone);


};

#endif // AIGAME_H
