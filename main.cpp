#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std; 
Mat image; 
Mat image_org;
vector<Rect> blurredRegions;//declares a space to store the blurred areas while new ones are being drawn
Point pt = Point(-1, -1); //sets the first point to an invalid value
bool drawing = false; 
unsigned char** arr2D;//declares the 2D array
int blur_degree = 5;//sets the blur degree to 5

void blurBlock(Mat& img, Rect area, int blockSize) {
	for (int y = area.y; y < area.y + area.height; y += blockSize) {

		for (int x = area.x; x < area.x + area.width; x += blockSize) {

			Rect rect(x, y, blockSize, blockSize);//makes a rectangle to be blured based on the two points and the size of the blurred blocks
			rect &= Rect(0, 0, img.cols, img.rows);//applies an intersection between rect and the rect object here
			Scalar avgColor = mean(img(rect));//finds the average color of each pixel
			rectangle(img, rect, avgColor, FILLED);//takes the average color computed in the previous line and creats a rectangle with that color
		}
	}
}

void redrawBlurredRegions() {
	image_org.copyTo(image);//copies the image 

	for (const auto& area : blurredRegions) {//iterates over all elements without modifiying them

		blurBlock(image, area, blur_degree);
	}
}

void onMyMouse(int event, int x, int y, int flag, void* data)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		//Draw the point that the mouse clicks
		pt = Point(x, y);
		drawing = true;
	}
	else if (event == EVENT_LBUTTONUP)
	{
		drawing = false;//stops drawing a rectangle when the user lifts the left button
		image.copyTo(image_org);//creates a copy of the image
		Rect selectedRegion(pt, Point(x, y));
		selectedRegion &= Rect(0, 0, image.cols, image.rows);//applies the region between the two points and makes another rectangle between them

		if (selectedRegion.area() > 0) {

			blurredRegions.push_back(selectedRegion);//stores the previously blurred regions
			redrawBlurredRegions();//redraws the selected region with updated blur degree

		}
	}
	if (drawing)
	{	
		redrawBlurredRegions();
		image_org.copyTo(image);//removes the previous rectangle as the user draws. 
		rectangle(image, pt, Point(x, y), Scalar(255, 255, 255), 3);//draws a rectangle 
	}

}
	int main(int argc, char** argv)
	{
		image = imread("mountains.png");//gets the image and sets it to image
		image.copyTo(image_org);//makes a copy of the image

		arr2D = new unsigned char* [image.rows];
		for (int i = 0; i < image.rows; i++) {

			arr2D[i] = new unsigned char[image.cols * 3];
			memcpy(arr2D[i], image.data + i * image.cols * 3, image.cols * 3);//copys the array data to the image

		}

		namedWindow("My Window");//opens the window for the image
		setMouseCallback("My Window", onMyMouse); //register the callback function

		while (1)
		{
			imshow("My Window", image);
			char c = waitKey(100);

			if (c == 27) //ASCII code for ESC key
			{
				break;
			}

			if (c == 'r' || c == 'R') {
				

				image = imread("mountains.png");//copys the original image
				image.copyTo(image_org);
				blurredRegions.clear();//removes the blur
			}

			if (c == 's' || c == 'S') {
				imwrite("moddedmountains.png", image);//saves image to specified file
				break;
			}
			if (c == 'i' || c == 'I') {
				if (blur_degree > 5) {
					blur_degree = blur_degree + 5;//increases blur by 5
				}
				else {
					blur_degree = blur_degree + 1;//increases blur by 1
				}
				redrawBlurredRegions();//redos blured regions to changed blur degree
			}

			if (c == 'd' || c == 'D') {

				if (blur_degree > 5) {
					blur_degree = blur_degree - 5;//decreases blur by 5
				}
				else {
					blur_degree = blur_degree - 1;//decreases blur by 1
				}
				redrawBlurredRegions();
			}
		}

	return 1;
}
