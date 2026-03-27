/******************************************************************************
 * Copyright (C) 2018-2023, idriverplus(BeiJing ZhiXingZhe, Inc.)
 *
 * NodeName: common
 * FileName: bspline.cpp
 *
 * Description: declare 3-order-B-Spline curve methods
 *
 * This is A COPY of
 *https://blog.csdn.net/jiangjjp2812/article/details/100176547
 *
 * History:
 * Steckon         2020/08/05    v1.0.0        Build this model.
 ******************************************************************************/
#pragma once

namespace geometry {

inline void BSpline::TwoOrderBSplineSmooth(CPosition *pt, int Num) {
  CPosition *temp = new CPosition[Num];
  for (int i = 0; i < Num; i++) temp[i] = pt[i];

  temp[0].x = 2 * temp[0].x - temp[1].x;  //  将折线两端点换成延长线上两点
  temp[0].y = 2 * temp[0].y - temp[1].y;

  temp[Num - 1].x = 2 * temp[Num - 1].x - temp[Num - 2].x;
  temp[Num - 1].y = 2 * temp[Num - 1].y - temp[Num - 2].y;

  CPosition NodePt1, NodePt2, NodePt3;
  double t;
  for (int i = 0; i < Num - 2; i++) {
    NodePt1 = temp[i];
    NodePt2 = temp[i + 1];
    NodePt3 = temp[i + 2];
    if (i == 0)  //  第一段取t=0和t=0.5点
    {
      t = 0;
      pt[i].x = F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
      pt[i].y = F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
      t = 0.5;
      pt[i + 1].x =
          F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
      pt[i + 1].y =
          F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
    } else if (i == Num - 3)  //  最后一段取t=0.5和t=1点
    {
      t = 0.5;
      pt[i + 1].x =
          F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
      pt[i + 1].y =
          F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
      t = 1;
      pt[i + 2].x =
          F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
      pt[i + 2].y =
          F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
    } else  //  中间段取t=0.5点
    {
      t = 0.5;
      pt[i + 1].x =
          F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
      pt[i + 1].y =
          F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
    }
  }
  delete[] temp;
}

//================================================================
// 函数功能： 二次B样条拟合,在节点之间均匀插入指定个数点
// 输入参数： *pt ：给定点序列，执行完成后，会被替换成新的数据点
//            Num：节点点个数
//            *InsertNum: 节点之间需要插入的点个数指针
// 返回值：   无返回值
//
// 编辑日期：   2014/12/07
//=================================================================
inline void BSpline::TwoOrderBSplineInterpolatePt(CPosition *&pt, int &Num,
                                           int *InsertNum) {
  if (pt == NULL || InsertNum == NULL) return;

  int InsertNumSum = 0;  //  计算需要插入的点总数
  for (int i = 0; i < Num - 1; i++) InsertNumSum += InsertNum[i];

  CPosition *temp =
      new CPosition[Num];  //  二次B样条不需要增加点数，需要将首尾点替换掉
  for (int i = 0; i < Num; i++) temp[i] = pt[i];

  temp[0].x = 2 * temp[0].x - temp[1].x;  //  将折线两端点换成延长线上两点
  temp[0].y = 2 * temp[0].y - temp[1].y;

  temp[Num - 1].x = 2 * temp[Num - 1].x - temp[Num - 2].x;
  temp[Num - 1].y = 2 * temp[Num - 1].y - temp[Num - 2].y;

  delete
      [] pt;  //  点数由原来的Num个增加到Num+InsertNumSum个，删除旧的存储空间，开辟新的存储空间

  pt = new CPosition[Num + InsertNumSum];

  CPosition NodePt1, NodePt2, NodePt3,
      NodePt4;  //  两节点间均匀插入点，需要相邻两段样条曲线,因此需要四个节点

  double t;
  int totalnum = 0;
  for (int i = 0; i < Num - 1; i++)  //  每条线段均匀插入点
  {
    if (i == 0)  //  第一段只需计算第一条样条曲线，无NodePt1
    {
      NodePt2 = temp[i];
      NodePt3 = temp[i + 1];
      NodePt4 = temp[i + 2];

      double dt = 0.5 / (InsertNum[i] + 1);
      for (int j = 0; j < InsertNum[i] + 1; j++) {
        t = 0 + dt * j;
        pt[totalnum].x =
            F02(t) * NodePt2.x + F12(t) * NodePt3.x + F22(t) * NodePt4.x;
        pt[totalnum].y =
            F02(t) * NodePt2.y + F12(t) * NodePt3.y + F22(t) * NodePt4.y;
        totalnum++;
      }
    } else if (i == Num - 2)  //  最后一段只需计算最后一条样条曲线，无NodePt4
    {
      NodePt1 = temp[i - 1];
      NodePt2 = temp[i];
      NodePt3 = temp[i + 1];

      double dt = 0.5 / (InsertNum[i] + 1);
      for (int j = 0; j < InsertNum[i] + 2; j++) {
        t = 0.5 + dt * j;
        pt[totalnum].x =
            F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
        pt[totalnum].y =
            F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
        totalnum++;
      }
    } else {
      NodePt1 = temp[i - 1], NodePt2 = temp[i];
      NodePt3 = temp[i + 1];
      NodePt4 =
          temp[i + 2];  // NodePt1,2,3计算第一条曲线，NodePt2,3,4计算第二条曲线

      int LeftInsertNum,
          RightInsertNum;  //  计算线段间左右曲线段上分别需要插入的点数
      double rightoffset =
          0;  //  左边曲线段从t=0.5开始，又边曲线段从t=rightoffset开始
      double Leftdt = 0, Rightdt = 0;  //  左右曲线取点t步长
      if (InsertNum[i] == 0) {
        LeftInsertNum = 0;
        RightInsertNum = 0;
      } else if (InsertNum[i] % 2 ==
                 1)  //  插入点数为奇数，左边曲线段插入点个数比右边多一点
      {
        RightInsertNum = InsertNum[i] / 2;
        LeftInsertNum = RightInsertNum + 1;
        Leftdt = 0.5 / (LeftInsertNum);
        Rightdt = 0.5 / (RightInsertNum + 1);
        rightoffset = Rightdt;
      } else  //  插入点数为偶数，左右边曲线段插入个数相同
      {
        RightInsertNum = InsertNum[i] / 2;
        LeftInsertNum = RightInsertNum;
        Leftdt = 0.5 / (LeftInsertNum + 0.5);
        Rightdt = 0.5 / (RightInsertNum + 0.5);
        rightoffset = Rightdt / 2;
      }

      for (int j = 0; j < LeftInsertNum + 1; j++) {
        t = 0.5 + Leftdt * j;
        pt[totalnum].x =
            F02(t) * NodePt1.x + F12(t) * NodePt2.x + F22(t) * NodePt3.x;
        pt[totalnum].y =
            F02(t) * NodePt1.y + F12(t) * NodePt2.y + F22(t) * NodePt3.y;
        totalnum++;
      }

      for (int j = 0; j < RightInsertNum; j++) {
        t = rightoffset + Rightdt * j;
        pt[totalnum].x =
            F02(t) * NodePt2.x + F12(t) * NodePt3.x + F22(t) * NodePt4.x;
        pt[totalnum].y =
            F02(t) * NodePt2.y + F12(t) * NodePt3.y + F22(t) * NodePt4.y;
        totalnum++;
      }
    }
  }
  delete[] temp;
  Num = Num + InsertNumSum;
}

//========================================================================
// 函数功能： 三次B样条平滑，把给定的点，平滑到B样条曲线上，不增加点的数目
// 输入参数： *pt ：给定点序列，执行完成后，会被替换成新的平滑点
//            Num：点个数
// 返回值：   无返回值
//
// 编辑日期：    2014/12/03
//========================================================================
inline void BSpline::ThreeOrderBSplineSmooth(CPosition *pt, int Num) {
  CPosition *temp = new CPosition[Num + 2];
  for (int i = 0; i < Num; i++) temp[i + 1] = pt[i];

  temp[0].x =
      2 * temp[1].x - temp[2].x;  //  将折线延长线上两点加入作为首点和尾点
  temp[0].y = 2 * temp[1].y - temp[2].y;

  temp[Num + 1].x = 2 * temp[Num].x - temp[Num - 1].x;
  temp[Num + 1].y = 2 * temp[Num].y - temp[Num - 1].y;

  CPosition NodePt1, NodePt2, NodePt3, NodePt4;
  double t;
  for (int i = 0; i < Num - 1; i++) {
    NodePt1 = temp[i];
    NodePt2 = temp[i + 1];
    NodePt3 = temp[i + 2];
    NodePt4 = temp[i + 3];

    if (i == Num - 4)  //  最后一段取t=0.5和t=1点
    {
      t = 0;
      pt[i].x = F03(t) * NodePt1.x + F13(t) * NodePt2.x + F23(t) * NodePt3.x +
                F33(t) * NodePt4.x;
      pt[i].y = F03(t) * NodePt1.y + F13(t) * NodePt2.y + F23(t) * NodePt3.y +
                F33(t) * NodePt4.y;
      t = 1;
      pt[i + 1].x = F03(t) * NodePt1.x + F13(t) * NodePt2.x +
                    F23(t) * NodePt3.x + F33(t) * NodePt4.x;
      pt[i + 1].y = F03(t) * NodePt1.y + F13(t) * NodePt2.y +
                    F23(t) * NodePt3.y + F33(t) * NodePt4.y;
    } else  //  中间段取t=0.5点
    {
      t = 0;
      pt[i].x = F03(t) * NodePt1.x + F13(t) * NodePt2.x + F23(t) * NodePt3.x +
                F33(t) * NodePt4.x;
      pt[i].y = F03(t) * NodePt1.y + F13(t) * NodePt2.y + F23(t) * NodePt3.y +
                F33(t) * NodePt4.y;
    }
  }
  delete[] temp;
}

//================================================================
// 函数功能： 三次B样条拟合,在节点之间均匀插入指定个数点
// 输入参数： *pt ：给定点序列，执行完成后，会被替换成新的数据点
//            Num：节点点个数
//            *InsertNum: 节点之间需要插入的点个数指针
// 返回值：   Num is returned size
//
// 编辑日期：   2014/12/07
//=================================================================
inline void BSpline::ThreeOrderBSplineInterpolatePt(CPosition *&pt, int &Num,
                                             int *InsertNum) {
  if (pt == NULL || InsertNum == NULL) return;
  // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<std::endl;

  int InsertNumSum = 0;  //  计算需要插入的点总数
  for (int i = 0; i < Num - 1; i++) InsertNumSum += InsertNum[i];
  // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<std::endl;

  CPosition *temp = new CPosition[Num + 2];
  for (int i = 0; i < Num; i++) temp[i + 1] = pt[i];
  // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<std::endl;

  temp[0].x =
      2 * temp[1].x - temp[2].x;  //  将折线延长线上两点加入作为首点和尾点
  temp[0].y = 2 * temp[1].y - temp[2].y;

  temp[Num + 1].x = 2 * temp[Num].x - temp[Num - 1].x;
  temp[Num + 1].y = 2 * temp[Num].y - temp[Num - 1].y;

  CPosition NodePt1, NodePt2, NodePt3, NodePt4;
  double t;

  delete
      [] pt;  //  点数由原来的Num个增加到Num+InsertNumSum个，删除旧的存储空间，开辟新的存储空间
  // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<std::endl;

  pt = new CPosition[Num + InsertNumSum];

  int totalnum = 0;
  for (int i = 0; i < Num - 1; i++)  //  每条线段均匀插入点
  {
    // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" i: "<<i<<std::endl;
    NodePt1 = temp[i];
    NodePt2 = temp[i + 1];
    NodePt3 = temp[i + 2];
    NodePt4 = temp[i + 3];
    double dt = 1.0 / (InsertNum[i] + 1);

    for (int j = 0; j < InsertNum[i] + 1; j++) {
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<" j: "<<j<<std::endl;
      t = dt * j;
      pt[totalnum].x = F03(t) * NodePt1.x + F13(t) * NodePt2.x +
                       F23(t) * NodePt3.x + F33(t) * NodePt4.x;
      pt[totalnum].y = F03(t) * NodePt1.y + F13(t) * NodePt2.y +
                       F23(t) * NodePt3.y + F33(t) * NodePt4.y;
      totalnum++;
    }

    if (i == Num - 2) {  //  最后一个尾点
      t = 1;
      pt[totalnum].x = F03(t) * NodePt1.x + F13(t) * NodePt2.x +
                       F23(t) * NodePt3.x + F33(t) * NodePt4.x;
      pt[totalnum].y = F03(t) * NodePt1.y + F13(t) * NodePt2.y +
                       F23(t) * NodePt3.y + F33(t) * NodePt4.y;
      totalnum++;
      // std::cout<<"["<<__FUNCTION__<<"] "<<__LINE__<<std::endl;
    }
  }
  std::cout << "[" << __FUNCTION__ << "] " << __LINE__
            << " totalnum: " << totalnum
            << ", Num+InsertNumSum: " << Num + InsertNumSum << std::endl;

  delete[] temp;
  Num = Num + InsertNumSum;
}

//================================================================
// 函数功能： 二次样条基函数
//
// 编辑日期：    2014/12/03
//================================================================
inline double BSpline::F02(double t) { return 0.5 * (t - 1) * (t - 1); }

inline double BSpline::F12(double t) { return 0.5 * (-2 * t * t + 2 * t + 1); }

inline double BSpline::F22(double t) { return 0.5 * t * t; }

//================================================================
// 函数功能： 三次样条基函数
//
// 编辑日期：    2014/12/03
//================================================================
inline double BSpline::F03(double t) {
  return 1.0 / 6 * (-t * t * t + 3 * t * t - 3 * t + 1);
}

inline double BSpline::F13(double t) {
  return 1.0 / 6 * (3 * t * t * t - 6 * t * t + 4);
}

inline double BSpline::F23(double t) {
  return 1.0 / 6 * (-3 * t * t * t + 3 * t * t + 3 * t + 1);
}

inline double BSpline::F33(double t) { return 1.0 / 6 * t * t * t; }

inline int BSpline::BSplineFilt(int order, std::list<Site> &result_path,
                         const std::list<Site> &un_filted_path) {
  int rtvalue = -1;
  int num = un_filted_path.size();

  if (order != 2 && order != 3) {
    std::cout << "[" << __FUNCTION__ << "] " << __LINE__
              << " order dismatch: " << order << std::endl;
    return rtvalue;
  }
  if (num < order + 1) {
    std::cout << "[" << __FUNCTION__ << "] " << __LINE__ << " num: " << num
              << std::endl;
    return rtvalue;
  }

  CPosition *testpt = new CPosition[num];
  int i = 0;
  for (const auto s : un_filted_path) {
    testpt[i] = CPosition(s.x, s.y);
    ++i;
  }

  int *Intnum = new int[num - 1];

  for (int i = 0; i < num - 1; i++) {
    Intnum[i] = 10;  //  每一个样条曲线内插入10个点
  }

  int num2 = num;
  BSpline bspline;
  if (order == 2) {
    bspline.TwoOrderBSplineInterpolatePt(testpt, num2,
                                         Intnum);  //  二次B样条曲线
  } else if (order == 3) {
    bspline.ThreeOrderBSplineInterpolatePt(testpt, num2,
                                           Intnum);  //  三次B样条曲线
  } else {
    return rtvalue;
  }
  // int length = sizeof(testpt) / sizeof(testpt[0]);
  std::cout << "[" << __FUNCTION__ << "] " << __LINE__
            << " filted length: " << num2 << std::endl;

  result_path.clear();
  for (int i = 0; i < num2; ++i) {
    Site s(testpt[i].x, testpt[i].y);
    result_path.emplace_back(s);
    rtvalue = 0;
  }
  delete[] Intnum;

  return rtvalue;
}

inline int BSpline::BSplineFilt(int order, std::vector<Site> &result_path,
                         const std::vector<Site> &un_filted_path) {
  int rtvalue = -1;
  int num = un_filted_path.size();

  if (order != 2 && order != 3) {
    std::cout << "[" << __FUNCTION__ << "] " << __LINE__
              << " order dismatch: " << order << std::endl;
    return rtvalue;
  }
  if (num < order + 1) {
    std::cout << "[" << __FUNCTION__ << "] " << __LINE__ << " num: " << num
              << std::endl;
    return rtvalue;
  }

  CPosition *testpt = new CPosition[num];
  int i = 0;
  for (const auto s : un_filted_path) {
    testpt[i] = CPosition(s.x, s.y);
    ++i;
  }

  int *Intnum = new int[num - 1];

  for (int i = 0; i < num - 1; i++) {
    Intnum[i] = 10;  //  每一个样条曲线内插入10个点
  }

  int num2 = num;
  BSpline bspline;
  if (order == 2) {
    bspline.TwoOrderBSplineInterpolatePt(testpt, num2,
                                         Intnum);  //  二次B样条曲线
  } else if (order == 3) {
    bspline.ThreeOrderBSplineInterpolatePt(testpt, num2,
                                           Intnum);  //  三次B样条曲线
  } else {
    return rtvalue;
  }
  // int length = sizeof(testpt) / sizeof(testpt[0]);
  std::cout << "[" << __FUNCTION__ << "] " << __LINE__
            << " filted length: " << num2 << std::endl;

  result_path.clear();
  for (int i = 0; i < num2; ++i) {
    Site s(testpt[i].x, testpt[i].y);
    result_path.emplace_back(s);
    rtvalue = 0;
  }
  delete[] Intnum;

  return rtvalue;
}

}  // namespace