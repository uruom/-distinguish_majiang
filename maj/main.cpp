#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui_c.h>
using namespace std;
using namespace cv;

/// 全局变量
Mat img; Mat templ; Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// 函数声明
void MatchingMethod( int, void* );

/** @主函数 */
int main( )
{
  /// 载入原图像和模板块
//  img = imread( argv[1], 1 );
//  templ = imread( argv[2], 1 );
  img=imread("mjz1.jpeg");
  templ=imread("4t.png");
  

  /// 创建窗口
  namedWindow( image_window, CV_WINDOW_AUTOSIZE );
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );

  /// 创建滑动条
//  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
  createTrackbar( "trackbar_label", image_window, &match_method, max_Trackbar, MatchingMethod );//match_method jiushi bianliang 
  MatchingMethod(78,0 );

  waitKey(0);
  return 0;
}
/*CV_EXPORTS int createTrackbar(const string& trackbarname, const string& winname,
                              int* value, int count,
                              TrackbarCallback onChange = 0,
                              void* userdata = 0);*/
  //typedef void (CV_CDECL *TrackbarCallback)(int pos, void* userdata);
/**
 * @函数 MatchingMethod
 * @简单的滑动条回调函数
 */
void MatchingMethod( int, void* )//
{
  /// 将被显示的原图像
  Mat img_display,templ_display;
  img.copyTo( img_display );
  templ.copyTo(templ_display);
  /// 创建输出结果的矩阵
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  double min_ans=1,max_ans=0;
  Point matchLoc;
  Mat ans;
  int result_cols,result_rows;
  for(int i=1;i<=10;i++)
  {
   result_cols =  img.cols - templ_display.cols + 1;
   result_rows = img.rows - templ_display.rows + 1;

  result.create( result_cols, result_rows, CV_32FC1 );//zhegeyouwenti

  /// 进行匹配和标准化
  matchTemplate( img, templ_display, result, match_method );//img yuan templ pipei  result jieguotuxiang  fangfa zhiyouliuzhong
 
  //guiyi     
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() ); 

  /// 通过函数 minMaxLoc 定位最匹配的位置
  
  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );//zhaodao zuixiaozhi he zuidazhi

  /// 对于方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值代表更高的匹配结果. 而对于其他方法, 数值越大匹配越好
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {   if(min_ans>minVal)
       {
          min_ans=minVal;
          matchLoc = minLoc;
          ans=templ_display;
       }
    }
  else
    { if(max_ans<maxVal)
      {
        max_ans=maxVal;
        matchLoc = maxLoc;
        ans=templ_display;
      } 
    }
    resize(templ_display,templ_display,Size(),0.9,0.9);
  }
  rectangle( img_display, matchLoc, Point( matchLoc.x + ans.cols , matchLoc.y + ans.rows ), Scalar::all(0), 2, 8, 0 );//goujian juxin
  rectangle( result, matchLoc, Point( matchLoc.x + ans.cols , matchLoc.y + ans.rows ), Scalar::all(0), 2, 8, 0 );//goujian juxinz

  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}

//https://blog.csdn.net/sazass/article/details/89634427   daxiao