#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;
using namespace cv;

int frameNumber, beginningFrameNumber;
int outputframenumberPos = 0;
int outputframenumberNeg = 0;
bool stepThorugh = true;
bool matchFound = false;

std::vector<string> &split(const string &s, char delim, std::vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<string> split(const string &s, char delim) {
    std::vector<string> elems;
    split(s, delim, elems);
    return elems;
}

int findLargestAnnoNumberInFile(std::vector<std::string> annotationVector){
  int largestNumber=0;
  for (int i = 0; i < annotationVector.size(); ++i)
  {
  int frameNumberInVector = atoi(annotationVector.at( i ).c_str());
    if (frameNumberInVector>largestNumber)
    {
      largestNumber=frameNumberInVector;
    }
  }
  return largestNumber;
}

int findSmallestAnnoNumberInFile(std::vector<std::string> annotationVector){
  return atoi(annotationVector.at( 0 ).c_str());
}

void readAllTxt( int argc, char** argv){
  string str;
  string delimframeNumber = "\t";

  std::vector<std::string> firstCompressedAnnotationVector, firstExpandedAnnotationVector; 

  char annotationString[100];
  char annotationTag[50];
  char inputTextPath[100];

  std::ofstream myfile;
  myfile.open("output.txt");
  myfile << "Filename\tAnnotation tag\tUpper left corner X\tUpper left corner Y\tLower right corner X\tLower right corner Y\tOrigin file\tOrigin frame number\tOrigin track\tOrigin track frame number" << std::endl;

  for (int i = 3; i < argc; ++i)
  {
    firstExpandedAnnotationVector.clear();
    firstCompressedAnnotationVector.clear();

    if (i % 2) // every second to fit input argument syntax
    { 
      memset(inputTextPath, 0, sizeof(inputTextPath)); 
      sprintf(inputTextPath,"%s/%s",argv[1],argv[i]);

      ifstream inputTextFile(inputTextPath);

      int auha=1;
      while (std::getline(inputTextFile, str)) 
      {
        firstExpandedAnnotationVector.push_back(str);
        size_t pos = 0;
        string token;
          while (( (pos = str.find(delimframeNumber)) != std::string::npos) && (auha<=1)) {
            token = str.substr(0, pos);
            firstCompressedAnnotationVector.push_back(token);
            str.erase(0, pos + delimframeNumber.length());
            auha++;
          }
    
        auha=1;
      }
      int returnedLargestNumber = findLargestAnnoNumberInFile(firstExpandedAnnotationVector);
      int returnedSmallestNumber = findSmallestAnnoNumberInFile(firstExpandedAnnotationVector);
    
      //std::cout << "argv[" << i << "]: " << argv[i] << " largest frame number is: " << returnedLargestNumber << " smallest frame number is: " << returnedSmallestNumber << std::endl;

      for (int frameNumber = returnedSmallestNumber; frameNumber <= returnedLargestNumber; ++frameNumber)
      {
        bool foundFirstFrame=false;
        bool noFound=true;
        int firstFrameNo=0;

        for (int k = 0; k < firstExpandedAnnotationVector.size(); ++k)
        {
          int frameNumberInVector = atoi(firstExpandedAnnotationVector.at( k ).c_str());

          if (frameNumberInVector==frameNumber)
          {
            foundFirstFrame=true;
            noFound=false;
            firstFrameNo=k;
          }
        }
        
        if(noFound==false)
        {
          if (foundFirstFrame==true )
          {
            int objectNo = 1;
            int localX1 = 2;
            int localX2 = 3;
            int localX3 = 4;
            int localX4 = 5;
            int totalNumOfObjects=0;
            
            std::vector<std::string> xFirst= split(firstExpandedAnnotationVector.at( firstFrameNo ), '\t');
            
            int objectsFirst = atoi(xFirst.at(objectNo).c_str());

            totalNumOfObjects = objectsFirst; 

            for (int m = 0; m < objectsFirst; ++m)
            {
              int x = atoi(xFirst.at( localX1+(4*m) ).c_str());
              int y = atoi(xFirst.at( localX2+(4*m) ).c_str());
              int width = atoi(xFirst.at( localX3+(4*m) ).c_str());
              int height = atoi(xFirst.at( localX4+(4*m)  ).c_str());

              memset(annotationString, 0, sizeof(annotationString)); 
              sprintf(annotationString+strlen(annotationString),"%i\t%i\t%i\t%i",x,y,x+width,y+height);
                
              memset(annotationTag, 0, sizeof(annotationTag)); 
              if (atoi(argv[i+1])==0) // Go
              {
                sprintf(annotationTag,"go");
              }
              if (atoi(argv[i+1])==1) // Warning
              {
                sprintf(annotationTag,"warning");
              }
              if (atoi(argv[i+1])==2) // Stop
              {
                sprintf(annotationTag,"stop");
              }
              if (atoi(argv[i+1])==3) // Go left 
              {
                sprintf(annotationTag,"goLeft");
              }
              if (atoi(argv[i+1])==4) // Warning left
              {
                sprintf(annotationTag,"warningLeft");
              }
              if (atoi(argv[i+1])==5) // Stop left
              {
                sprintf(annotationTag,"stopLeft");
              }
              if (atoi(argv[i+1])==6) // Go forward
              {
                sprintf(annotationTag,"goForward");
              }

              char originFileString[100];
              sprintf(originFileString,"%s/%s.avi\t%i\t%s/%s.avi\t%i",argv[1],argv[2],frameNumber,argv[1],argv[2],frameNumber);
              char decimalFramenumber[15];
              sprintf(decimalFramenumber,"%.5i",frameNumber);
              //std::cout<< "Decimal Number: " << decimalFramenumber << std::endl;

              myfile << argv[2] << "-" << decimalFramenumber << ".png" << "\t"<< annotationTag << "\t"<< annotationString << "\t"<< originFileString << std::endl;     
            }
          }
          memset(annotationString, 0, sizeof(annotationString)); 
        }

      }
      std::cout << "Reading input file: '" << inputTextPath <<"' with defined class: '" << annotationTag<< "'." << std::endl;
    }

  }
  myfile.close();
  waitKey();
}
static void help()
{
    std::cout << "\nThis is a tool developed for .\n"
            "\n"
            "\n"
            "\n"

            "Usage: \n"
            "   ./convertAnno [Direction to video to annotate] [Start annotation number]\n"
            "Example: \n"
            "   ./convertAnno training/dayclip1/ dayclip1 go.txt 0 warning.txt 1 stop.txt 2 goLeft.txt 3 warningLeft.txt 4 stopLeft.txt 5 goForward.txt 6\n\n";
}

int main(int argc, char** argv)
{
  help();
  clock_t start, finish;
  start = clock();
  std::cout << "------------------------------" << std::endl;
  std::cout << "---Creating new annotations---" << std::endl;
  std::cout << "------------------------------" << std::endl;
  readAllTxt(argc, argv);
  finish = clock();
  std::cout << "------------------------------" << std::endl;
  std::cout << "Execution time (seconds): " << ((double)(finish - start))/CLOCKS_PER_SEC << std::endl;

  start = clock();
  std::cout << "\n------------------------------" << std::endl;
  std::cout << "--Creating frames from video--" << std::endl;
  std::cout << "------------------------------" << std::endl;
  
  // Creating frames folder in directory 
  char outputPath[100];
  sprintf(outputPath,"mkdir %s/frames",argv[1]);
  string command(outputPath);
  system(command.c_str());

  char moveTextFilePath[100];
  sprintf(moveTextFilePath,"mv output.txt %s/frames/frameAnnotations.txt",argv[1]);
  string moveCommand(moveTextFilePath);
  system(moveCommand.c_str());

  // Read in video file
  char inputVideoPath[100];
  sprintf(inputVideoPath,"%s/%s.mov",argv[1],argv[2]);
  std::cout << "Converting following videofile to frames: '" << inputVideoPath <<"'."<< std::endl;

  cv::VideoCapture capture(inputVideoPath);
  int frames = capture.get(CV_CAP_PROP_FRAME_COUNT);

  cv::Mat image;
  cv::Mat imgLeft;

  int IMAGEWIDTH = 1280;
  int IMAGEHEIGHT = 960;
  int inputVideoFrameCounter = 0;

  string pngPath;
  while(1)
  {
    capture >> image;
    
    //if fail to read the image
    if (image.empty()) 
    { 
      //std::cout << "Error loading the image" << std::endl;
      break;
    }
    imgLeft = image(cv::Rect(cv::Point(0,0), cv::Size(IMAGEWIDTH, IMAGEHEIGHT)));

    // Write to frames to folder
    stringstream frameWriteToPath;

    char decimalFramenumberOut[15];
    sprintf(decimalFramenumberOut,"%.5i",inputVideoFrameCounter);
    //std::cout<< "Decimal Number: " << decimalFramenumberOut << std::endl;

    frameWriteToPath << argv[1] << "/frames/"<< argv[2]<<"--"<<decimalFramenumberOut << ".png";
    
    pngPath = frameWriteToPath.str();
    //std::cout<< "Write to path: " << pngPath<< std::endl;
    imwrite(pngPath,imgLeft);

    if (inputVideoFrameCounter == floor(frames*0) )
    {
      std::cout<< "|----------|  0 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.1) )
    {
      std::cout<< "|+---------| 10 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.2) )
    {
      std::cout<< "|++--------| 20 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.3) )
    {
      std::cout<< "|+++-------| 30 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.4) )
    {
      std::cout<< "|++++------| 40 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.5) )
    {
      std::cout<< "|+++++-----| 50 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.6) )
    {
      std::cout<< "|++++++----| 60 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.7) )
    {
      std::cout<< "|+++++++---| 70 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.8) )
    {
      std::cout<< "|++++++++--| 80 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*0.9) )
    {
      std::cout<< "|+++++++++-| 90 %" << std::endl;
    }
    if (inputVideoFrameCounter == floor(frames*1.0)-1 )
    {
      std::cout<< "|++++++++++| 100 %" << std::endl;
    } 

    inputVideoFrameCounter = inputVideoFrameCounter + 1;

    /*imshow( "Main Window", imgLeft );

    char k = cv::waitKey(1);
    if ( (k == 'q') || (k == 27) ) // push q or escape to quit.
    {
      break;
    } */ 
  }

  finish = clock();
  std::cout << "------------------------------" << std::endl;
  std::cout << "Execution time (seconds): " << ((double)(finish - start))/CLOCKS_PER_SEC << std::endl;


  return 0;
}


