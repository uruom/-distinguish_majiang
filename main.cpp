#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <opencv2/highgui/highgui_c.h>
using namespace std;
using namespace cv;
struct image_mat
{
  Mat image;
  string name;
  double val;
};
Mat img; Mat templ; Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";
int match_method=2;
int max_Trackbar = 5;
void Matching( int, void* );
image_mat data_tp[50];
int main( int argc, char** argv )
{
  string data_tp_name;
  ifstream inf ("name.txt");

  int name_i=0;
  while(getline(inf, data_tp_name))
  {
    name_i++;
   // cout<<data_tp_name<<name_i<<endl;
    data_tp[name_i].name=data_tp_name;
    data_tp[name_i].image=imread(data_tp_name);
  //  imshow(data_tp[name_i].name,data_tp[name_i].image);
  //  waitKey(0);
  }
//   waitKey(0);
  cout<<name_i<<endl;
  img=imread("2t.png");//要注意这里面的原图需要比待匹配模板更大，不然会莫名报错，试了好久，应该是这个函数的问题matchTemplate
//  templ=imread("tp/8t.png");//其中匹配只能匹配到图像相同的部分，图像大小不同的话就没办法
//  imshow("a",templ);
//  waitKey(0);
  
//  namedWindow( image_window, CV_WINDOW_AUTOSIZE );//创建一个窗口
//  namedWindow( result_window, CV_WINDOW_AUTOSIZE );//创建一个窗口
//  cout<<"aaa"<<endl;
  Matching( 0, 0 );
//  cout<<"aaa"<<endl;
/*～～～～～*/
//  waitKey(0);
  return 0;
}

void Matching( int, void* )
{
  Mat img_display,templ_display,templ_test;
//  img.copyTo( img_display );
  
  
  double minVal; double maxVal; Point minLoc; Point maxLoc;
  double min_ans=1,max_ans=0;//要注意对最小值先赋值最大1，对最大值先赋值最小0
  Point matchLoc;
  Mat ans;
  int result_cols,result_rows;
  //相比与之前，我把所有的定义提前，避免了在循环中出现问题
  for(int tp=1;tp<=34;tp++)
  {
    data_tp[tp].image.copyTo(templ);
    templ.copyTo(templ_display);
    max_ans=0;
  for(int i=1;i<=10;i++)//循环10次，每次改变0.9，也就是说最大的情况是变为原矩形的0.34倍，当然，后面这个可以根据精度什么的再调，估计会牵扯到调参的问题
  {
    img.copyTo( img_display );
    templ_display.copyTo(templ_test);//等于是复制了一个mat，然后处理用的都是img_display，我觉得可能是避免在滑动条的时候影响到了img，导致后面的图像不对
  //在这里我新增了一个templ_display，后面每次我会改变这个Mat的大小，这样可以进行大小的变化
   result_cols =  img.cols - templ_display.cols + 1;//这是匹配的大小范围，简单来说能匹配的范围就是在这么大的区间以内，不然要出界报错
   result_rows = img.rows - templ_display.rows + 1;//分别是行和列，其中row是行，col是宽
  
    //因为每次我的templ_display的值都会便，所以create是每次都会有一个  
  result.create( result_cols, result_rows, CV_32FC1 );//创建矩阵
  /*～～～～*/
  matchTemplate( img, templ_display, result, match_method );//自带的一个匹配函数，
   minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );//buyongguiyi buranzuida doushiyi
//  normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() ); //归一化函数，目的可以理解为把一段序列缩短至0～1中，用0～1之间的数来比较，这样方便比较最大个最小之
  /*这两个函数一起的目的我认为是对于所匹配原图上的每一个点进行判断，就是一个对比度的判断吧，判断相似性，然后对于每个值用0～1来表示*/
  /*～～～～*/
  
//  minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );//寻找result这个矩阵中的最大和最小值的位置
  /*～～～～～这次更新最大的改动在这里，对于每一次的计算来说，都会得到一个最大值和一个最小值，那么换句话说，在所有循环中的最小值所对应的点以及缩放
  就是这个图像的最佳匹配，也就是说对于循环中的每一次，我会记录下当前的最大/最小值，然后和已知的一个最大和最小值进行判断，大小，若是更大或者更小就
  更新，反之则不管，这样出来以后的值就是这所有循环中最匹配的值*/
 //     cout<<maxVal<<" ";
     if(max_ans<maxVal)//同理，是否大于最大值，不同方法找的最大或者最小值不同
      {
        max_ans=maxVal;////同理，maxVal是值，而其中的maxLoc是所对应点的坐标
      } 
    resize(templ_test,templ_test,Size(),0.9,0.9);
  }
 // cout<<endl;
    data_tp[tp].val=max_ans;
  }
  int ans_s;
  double ans_m=0;
  for(int i=1;i<=33;i++)
  {
    if(ans_m<data_tp[i].val)
    {
      ans_m=data_tp[i].val;
      ans_s=i;
      
    }
    cout<<i<<" "<<data_tp[i].name<<" "<<data_tp[i].val<<endl;
  }
  cout<<data_tp[ans_s].name;
  return;
}