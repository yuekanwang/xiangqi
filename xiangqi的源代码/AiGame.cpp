#include "AiGame.h"
#include"Step.h"
#include  <QDebug>



AiGame::AiGame()
{
    this->setWindowTitle("中国象棋对战(人机对战)");

    _level=4;//设置推理步数
    kong =25; //设置能走的空格所加的战斗力
}
AiGame::~AiGame()
{

}

void AiGame::mousePressEvent(QMouseEvent *event)
{
    if(!this->redtrue)
    {
        return ;
    }
    if(this->redtrue)
    {
        Gamescene::mousePressEvent(event);
        repaint();

        if(!this->redtrue)
        {
            _sleep(1000);//机子停下0.5s
            Step* step=AiMove();
            StoneMove(step->lastrow,step->lastcol);
            StoneMove(step->torow,step->tocol);
            delete step;
            selectid=-1;
            //repaint();
        }
    }
}



Step* AiGame::AiMove()
{
    //获取所有的可行步骤
    QVector<Step*> steps;
    getAllcanMove(steps);
    //尝试走一下

    int maxScore=-100000;
    Step *ret=NULL;
    while(steps.count())
    {
        Step* step =steps.back();
        steps.removeLast();//删除一个数组的东西

        fakeMove(step);//试着走一下
        int Score=getMaxScore(_level-1,maxScore);//算局面分
        unfakeMove(step);//退回一步

        if(Score>maxScore)//选择最大值
        {
            maxScore=Score;
            ret=step;
        }
        else
        {
            delete step;//为什么要else才delete，因为它是个指针，如果都删掉，那ret指向会崩溃
        }
    }
    return ret;
}



int AiGame::getMinScore(int level,int curmax)//计算下一步的棋盘分数最小值（黑减红）
{
    if(level==0||(stone[4].death||stone[20].death))
        return getScore();

    QVector<Step*> steps;
    getAllcanMove(steps);//第二步是红旗的回合
    int minScore=9999;
    while(steps.count())
    {
        Step* step =steps.back();
        steps.removeLast();//删除一个数组的东西

        fakeMove(step);//试着走一下
        int Score=getMaxScore(level-1,minScore);//算局面分
        unfakeMove(step);//退回一步

        delete step;

        if(Score<=curmax)
        {
            while(steps.count())//清除指针内存
            {
                Step* step =steps.back();
                steps.removeLast();//删除一个数组的东西
                delete step;
            }
            return Score;
        //这里是站在寻找最大分数的角度，在其下一步找最小值。
        //发现下一步有个值，比上一次找到的最小值要小，便没有继续下去的需要了。
        }
        if(Score<minScore)//选择最小值
        {
            minScore=Score;
        }

    }
    return minScore;
}


int AiGame::getMaxScore(int level,int curmin)
{
    if(level==0||(stone[4].death||stone[20].death))//层数遍历够了，或者有将军死了，就停止
        return getScore();

    QVector<Step*> steps;
    getAllcanMove(steps);//第二步是红旗的回合
    int maxScore=-99999;

    while(steps.count())
    {
        Step* step =steps.back();
        steps.removeLast();//删除一个数组的东西

        fakeMove(step);//试着走一下
        int Score=getMinScore(level-1,maxScore);//算局面分
        unfakeMove(step);//退回一步

        delete step;//释放内存

        if(Score>=curmin)
        {
            while(steps.count())
            {
                Step* step =steps.back();
                steps.removeLast();//删除一个数组的东西
                delete step;
            }
            return Score;
        //这里是站在寻找最小值的角度，在其下一步找最大值。
        //发现下一步有个值，比上一次找到的最大值要大，便没有继续下去的需要了。
        }
        if(Score>maxScore)//选择最小值
        {
            maxScore=Score;
        }


    }
    return maxScore;
}



void AiGame::getAllcanMove(QVector<Step*> &steps)
{
    int min=16,max=32;//如果是算黑棋的步数
    if(this->redtrue)
    {
        min=0,max=16;//如果是算红棋的步数
    }


    //获取所有的可行步骤
    for(int i=min;i<max;i++)
    {
        if(this->stone[i].death) continue;

        lastrow = this->stone[i].row;
        lastcol = this->stone[i].col;
    //    lastfight=this->stone[i].fight;

        Look_walkablelocation(i,steps);

    }
}


void AiGame::saveStone(int id,int killid,int row,int col,QVector<Step*> &steps)
{
    Step* _stone=new Step;
    _stone->_id=id;
    _stone->_killid=killid;
    _stone->torow=row;
    _stone->tocol=col;
    _stone->lastrow=lastrow;
    _stone->lastcol=lastcol;
    steps.append(_stone);

}


void AiGame::fakeMove(Step* _stone)
{
    if(_stone->_killid!=-1)
    {
        this->stone[_stone->_killid].death = true;
        //this->stone[_stone->_id].fight
    }
    this->stone[_stone->_id].row = _stone->torow;
    this->stone[_stone->_id].col = _stone->tocol;

}

void AiGame::unfakeMove(Step* _stone)
{
    if(_stone->_killid!=-1)
    {
        this->stone[_stone->_killid].death = false;
    }
    this->stone[_stone->_id].row = _stone->lastrow;
    this->stone[_stone->_id].col = _stone->lastcol;
}



int AiGame::getScore()
{
    //               enum TY{JIANG, SHI, XIANG, MA,   CHE, PAO, BING};
    static int stoneScore[]={99999,  200,  200, 300 , 500, 300,  100};

    int redScore=0,blackScore=0;

    for(int i=0;i<16;i++)
    {

        if(!stone[i].death)
            redScore+=stoneScore[stone[i].ty]+stone[i].fight;
    }
    for(int i=16;i<32;i++)
    {

        if(!stone[i].death)
            blackScore+=stoneScore[stone[i].ty]+stone[i].fight;
    }

    return blackScore-redScore;//黑棋分数-红旗分数
}

void AiGame::Look_walkablelocation(int id,QVector<Step*> &steps)//寻找可走的位置
{
    switch(stone[id].ty)//根据选中的棋子，判断遍历哪些位置。比如，车只看横竖，将只看上下左右。
    {
    case Stone::JIANG:
        Look_Jianglocation(id,steps);
    case Stone::SHI:
        Look_Shilocation(id,steps);
    case Stone::XIANG:
        Look_Xianglocation(id,steps);
    case Stone::MA:
        Look_Malocation(id,steps);
    case Stone::CHE:
        Look_Chelocation(id,steps);
    case Stone::PAO:
        Look_Paolocation(id,steps);
    case Stone::BING:
        Look_Binglocation(id,steps);
    }
    return;
}


void AiGame::trying(int id,int killid,int row,int col,QVector<Step*> &steps)
{
    int row_=stone[id].row;//记下原来的位置
    int col_=stone[id].col;

    if(killid!=-1)
    {
        this->stone[killid].death = true;
    }
    this->stone[id].row = row;
    this->stone[id].col = col;

    if(isDefeated(1)&&!redtrue)
    {
        if(killid!=-1)
        {
            this->stone[killid].death = false;
        }
        this->stone[id].row = row_;
        this->stone[id].col = col_;
        return ;
    }
    else if(isDefeated(0)&&redtrue)
    {
        if(killid!=-1)
        {
            this->stone[killid].death = false;
        }
        this->stone[id].row = row_;
        this->stone[id].col = col_;
        return ;
    }

    if(killid!=-1)
    {
        this->stone[killid].death = false;
    }
    this->stone[id].row = row_;
    this->stone[id].col = col_;
    saveStone(id,killid,row,col,steps);

    return ;
}



void AiGame::Look_Jianglocation(int id,QVector<Step*> &steps)//将
{
    int dr[4]={-1,1,0,0};
    int dc[4]={0,0,1,-1};

    for(int i=0;i<4;i++)//上下左右
    {
        int row=stone[id].row+dr[i],col=stone[id].col+dc[i];

        if((row>=0&&row<=9)&&(col>=0&&col<=8))
            {
                int killid = this->getStoneId(row,col);
                if(canMove(id,killid,row,col))
                {
                    trying(id, killid, row, col,steps);
                }
            }
    }
    return ;
}

void AiGame::Look_Xianglocation(int id,QVector<Step*> &steps)//象
{
    int dr[4]={-2,-2,2,2};
    int dc[4]={-2,2,2,-2};

    for(int i=0;i<4;i++)//上下左右
    {
        int row=stone[id].row+dr[i],col=stone[id].col+dc[i];
        if((row>=0&&row<=9)&&(col>=0&&col<=8))
        {
            int killid = this->getStoneId(row,col);
            if(canMove(id,killid,row,col))
            {
                trying(id, killid, row, col,steps);
            }
        }
    }
    return ;
}

void AiGame::Look_Shilocation(int id,QVector<Step*> &steps)//士
{
    int dr[4]={-1,-1,1,1};
    int dc[4]={-1,1,1,-1};

    for(int i=0;i<4;i++)//上下左右
    {
        int row=stone[id].row+dr[i],col=stone[id].col+dc[i];
        if((row>=0&&row<=9)&&(col>=0&&col<=8))
        {
            int killid = this->getStoneId(row,col);
            if(canMove(id,killid,row,col))
            {
                trying(id, killid, row, col,steps);
            }
        }
    }
    return ;
}

void AiGame::Look_Binglocation(int id,QVector<Step*> &steps)//兵
{
    int dr[4]={-1,1,0,0};
    int dc[4]={0,0,1,-1};

    for(int i=0;i<4;i++)//上下左右
    {
        int row=stone[id].row+dr[i],col=stone[id].col+dc[i];

        if((row>=0&&row<=9)&&(col>=0&&col<=8))
        {
            int killid = this->getStoneId(row,col);
            if(canMove(id,killid,row,col))
            {
                trying(id, killid, row, col,steps);
            }
        }
    }
    return ;
}


void AiGame::Look_Chelocation(int id,QVector<Step*> &steps)//车
{
    int row_=stone[id].row,col_=stone[id].col;

    for(int row=0;row<=9;row++)
    {
        if(row_==row)continue;

        int col=col_;
        int killid = this->getStoneId(row,col);
        if(canMove(id,killid,row,col))
        {
            trying(id, killid, row, col,steps);
        }
    }

    for(int col=0;col<=8;col++)
    {
        if(col_==col)continue;

        int row=row_;
        int killid = this->getStoneId(row,col);
        if(canMove(id,killid,row,col))
        {
            trying(id, killid, row, col,steps);
        }
    }
}


void AiGame::Look_Paolocation(int id,QVector<Step*> &steps)//炮
{
    int row_=stone[id].row,col_=stone[id].col;

    for(int row=0;row<=9;row++)
    {
        if(row_==row)continue;

        int col=col_;
        int killid = this->getStoneId(row,col);
        if(canMove(id,killid,row,col))
        {
            trying(id, killid, row, col,steps);
        }
    }

    for(int col=0;col<=8;col++)
    {
        if(col_==col)continue;

        int row=row_;
        int killid = this->getStoneId(row,col);
        if(canMove(id,killid,row,col))
        {
            trying(id, killid, row, col,steps);
        }
    }
}
void AiGame::Look_Malocation(int id,QVector<Step*> &steps)//马
{
    int dr[8]={-1,-2,-2,-1,1,2,2,1};
    int dc[8]={2,1,-1,-2,-2,-1,1,2};

    for(int i=0;i<8;i++)//上下左右
    {
        int row=stone[id].row+dr[i],col=stone[id].col+dc[i];
        if((row>=0&&row<=9)&&(col>=0&&col<=8))
        {
            int killid = this->getStoneId(row,col);
            if(canMove(id,killid,row,col))
            {
                trying(id, killid, row, col,steps);
            }
        }
    }
    return ;
}



