#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/highgui/highgui_c.h>
using namespace std;
using namespace cv;
Mat img; Mat templ; Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";
int match_method;
int max_Trackbar = 5;
void MatchingMethod( int, void* );
int main( int argc, char** argv )
{
  img=imread("mjz1.jpeg");//要注意这里面的原图需要比待匹配模板更大，不然会莫名报错，试了好久，应该是这个函数的问题matchTemplate
  templ=imread("4t.png");//其中匹配只能匹配到图像相同的部分，图像大小不同的话就没办法
  
  namedWindow( image_window, CV_WINDOW_AUTOSIZE );//创建一个窗口
  namedWindow( result_window, CV_WINDOW_AUTOSIZE );//创建一个窗口

  /*这个仅仅只是给输出的滚动条取名*/
  char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
 /*～～～～～这两个算是一套createTrackbar的一个标准写法，具体来说，中间那个&match_method在函数中是一个可变量，根据滑动的条来确定*/
 /*其中createtracker中的最后一个以及这个后面的那个都是函数名，函数是int，void,后面那个函数的变量是0，0，*/
 /*不太懂，但是试了一下，前面那个0可以变为任意数，无影响，后面那个数不能够改变，不然报错*/
  /*其中match_method这个变量其实不太懂是怎么在函数中等于那些方法的，尝试了一下，我倾向于可能是函数中封装了就是enum，即
  enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 }所以就可以直接用滑动条来变了*/
  createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );
  MatchingMethod( 0, 0 );
/*～～～～～*/
  waitKey(0);
  return 0;
}

void MatchingMethod( int, void* )
{
  Mat img_display,templ_display;
  img.copyTo( img_display );
  templ.copyTo(templ_display);//等于是复制了一个mat，然后处理用的都是img_display，我觉得可能是避免在滑动条的时候影响到了img，导致后面的图像不对

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
  rectangle( img_display, matchLoc, Point( matchLoc.x + ans.cols , matchLoc.y + ans.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + ans.cols , matchLoc.y + ans.rows ), Scalar::all(0), 2, 8, 0 );;
  //以找出的那个点，然后构建出被匹配图大小的矩形就好了
  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}