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
  //在这里我新增了一个templ_display，后面每次我会改变这个Mat的大小，这样可以进行大小的变化
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  double min_ans=1,max_ans=0;//要注意对最小值先赋值最大1，对最大值先赋值最小0
  Point matchLoc;
  Mat ans;
  int result_cols,result_rows;
  //相比与之前，我把所有的定义提前，避免了在循环中出现问题
  for(int i=1;i<=10;i++)//循环10次，每次改变0.9，也就是说最大的情况是变为原矩形的0.34倍，当然，后面这个可以根据精度什么的再调，估计会牵扯到调参的问题
  {
   result_cols =  img.cols - templ_display.cols + 1;//这是匹配的大小范围，简单来说能匹配的范围就是在这么大的区间以内，不然要出界报错
   result_rows = img.rows - templ_display.rows + 1;//分别是行和列，其中row是行，col是宽
  
    //因为每次我的templ_display的值都会便，所以create是每次都会有一个  
  result.create( result_cols, result_rows, CV_32FC1 );//创建矩阵
  /*～～～～*/
  matchTemplate( img, templ_display, result, match_method );//自带的一个匹配函数，
  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() ); //归一化函数，目的可以理解为把一段序列缩短至0～1中，用0～1之间的数来比较，这样方便比较最大个最小之
  /*这两个函数一起的目的我认为是对于所匹配原图上的每一个点进行判断，就是一个对比度的判断吧，判断相似性，然后对于每个值用0～1来表示*/
  /*～～～～*/
  
  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );//寻找result这个矩阵中的最大和最小值的位置
  /*～～～～～这次更新最大的改动在这里，对于每一次的计算来说，都会得到一个最大值和一个最小值，那么换句话说，在所有循环中的最小值所对应的点以及缩放
  就是这个图像的最佳匹配，也就是说对于循环中的每一次，我会记录下当前的最大/最小值，然后和已知的一个最大和最小值进行判断，大小，若是更大或者更小就
  更新，反之则不管，这样出来以后的值就是这所有循环中最匹配的值*/
  if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    {   if(min_ans>minVal)//判断最小的值是否大于本次循环的最小值
       {
          min_ans=minVal;//其中minVal是值，而其中的minLoc是所对应点的坐标
          matchLoc = minLoc;
          ans=templ_display;
       }
    }
  else
    { if(max_ans<maxVal)//同理，是否大于最大值，不同方法找的最大或者最小值不同
      {
        max_ans=maxVal;////同理，maxVal是值，而其中的maxLoc是所对应点的坐标
        matchLoc = maxLoc;
        ans=templ_display;//储存当前的大小
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
