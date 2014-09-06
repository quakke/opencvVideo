#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

using namespace std;

IplImage* src = 0;
IplImage* gray = 0;
IplImage* bin = 0;
IplImage* dst = 0;
IplImage* image = 0;

int radius = 1;
int iterations = 8;

int main( int argc, char** argv )
{
	cvNamedWindow( "AVI Video", CV_WINDOW_AUTOSIZE );

	char* filename = argc == 2 ? argv[1] : "D://2kadra.avi";
	
	CvCapture* capture = cvCreateFileCapture( filename );
	IplImage* frame;
	while(1) 
	{
		frame = cvQueryFrame( capture ); 

		if( !frame ) 
			break;

		image = cvCloneImage(frame);

		cvSetImageROI(image, cvRect(30,30,300,360));
		
		src = cvCloneImage(image);

		IplConvKernel* Kern = cvCreateStructuringElementEx(radius*2+1, radius*2+1, radius, radius, CV_SHAPE_ELLIPSE);
		IplImage* Temp = 0;
		Temp = cvCreateImage(cvSize(image->width, image->height) , IPL_DEPTH_8U, 1);
        
		cvMorphologyEx(image, src, Temp, Kern, CV_MOP_OPEN, iterations);
		cvMorphologyEx(image, src, Temp, Kern, CV_MOP_GRADIENT, iterations);
	
		// sozdaem odnokanal'nie kartinki
		gray = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
		bin = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
		// kloniryem
		dst = cvCloneImage(src);

		// preobrazyem v gradacii serogo
		cvCvtColor(src, gray, CV_RGB2GRAY);

		// preobrazyem v dvoi4noe
		cvInRangeS(gray, cvScalar(40), cvScalar(150), bin);

		CvMemStorage* storage = cvCreateMemStorage(0);
    		CvSeq* contours=0;

		// naxodim kontyri
		int contoursCont = cvFindContours( bin, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE ,cvPoint(0,0));

		contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );

		for(CvSeq* seq0 = contours; seq0 != 0; seq0 = seq0->h_next)
		{
 			cvDrawContours(dst, seq0, CV_RGB(255,216,0), CV_RGB(0,0,250), 0, 1, 8); // рисуем контур
		}	 

		CvSeq* seq = contours;
	
		CvPoint* M = (CvPoint*)cvGetSeqElem ( seq, 0 );
		CvPoint* M1 = (CvPoint*)cvGetSeqElem ( seq, 0 );
		CvPoint* VP = (CvPoint*)cvGetSeqElem ( seq, 3 );		
		CvPoint* M2 = (CvPoint*)cvGetSeqElem ( seq, 1 );
		CvPoint* NP = (CvPoint*)cvGetSeqElem ( seq, 2 );

		M1->x += (abs((M1->x - VP->x)/2));
		M2->x += (abs((M2->x - NP->x)/2));

		// koeff naklona priamoi
		float koeff = (M2->y - M1->y)/(M2->x - M1->x);

		M->x = M1->x + (abs(M1->x - M2->x)/2);
		M->y = M1->y + (abs(M1->y - M2->y)/2);
		int b = M->y;
		cvLine( frame, cvPoint(M->x=100, M->y = (1/koeff)*(M->x)+b), cvPoint(M->x = 400, M->y = (1/koeff)*(M->x)+b), CV_RGB(100,200,100), 3, 8);

		cvResetImageROI(image);

		cvShowImage( "AVI Video", frame );

		char c = cvWaitKey(33);
		if( c == 27 ) 
			break;
	}

	cvReleaseCapture( &capture );
	cvDestroyWindow( "AVI Video" );
	return 0;
}
