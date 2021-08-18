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
  Mat img_display;
  img.copyTo( img_display );//等于是复制了一个mat，然后处理用的都是img_display，我觉得可能是避免在滑动条的时候影响到了img，导致后面的图像不对

  int result_cols =  img.cols - templ.cols + 1;//这是匹配的大小范围，简单来说能匹配的范围就是在这么大的区间以内，不然要出界报错
  int result_rows = img.rows - templ.rows + 1;//分别是行和列，其中row是行，col是宽

  result.create( result_cols, result_rows, CV_32FC1 );//创建矩阵
  /*～～～～*/
  matchTemplate( img, templ, result, match_method );//自带的一个匹配函数，
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );//归一化函数，目的可以理解为把把一段序列缩短至0～1中，用0～1之间的数来比较，这样方便比较最大个最小之
  /*这两个函数一起的目的我认为是对于所匹配原图上的每一个点进行判断，就是一个对比度的判断吧，判断相似性，然后对于每个值用0～1来表示*/
  /*～～～～*/
  
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  Point matchLoc;

  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );//寻找result这个矩阵中的最大和最小值的位置
  //因为前面其实已经有匹配和归一，所以说这个地方找出最大值或最小值就好了，这个地方同时找最大最小是因为后面根据不同方法，代表的最匹配是越大还是越小不同
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    { matchLoc = minLoc; }
  else
    { matchLoc = maxLoc; }
  //根据方法来看最大还是最小
  rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  rectangle( result, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );
  //以找出的那个点，然后构建出被匹配图大小的矩形就好了
  imshow( image_window, img_display );
  imshow( result_window, result );

  return;
}
