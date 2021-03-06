// Source file for image class



// Include files 

#include "R2/R2.h"
#include "R2Pixel.h"
#include "R2Image.h"
#include "svd.h"
#include <vector>
#include "math.h"
#include <cmath>
#include <cfloat>



////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
////////////////////////////////////////////////////////////////////////


R2Image::
R2Image(void)
  : pixels(NULL),
    npixels(0),
    width(0), 
    height(0)
{
}



R2Image::
R2Image(const char *filename)
  : pixels(NULL),
    npixels(0),
    width(0), 
    height(0)
{
  // Read image
  Read(filename);
}



R2Image::
R2Image(int width, int height)
  : pixels(NULL),
    npixels(width * height),
    width(width), 
    height(height)
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);
}



R2Image::
R2Image(int width, int height, const R2Pixel *p)
  : pixels(NULL),
    npixels(width * height),
    width(width), 
    height(height)
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = p[i];
}



R2Image::
R2Image(const R2Image& image)
  : pixels(NULL),
    npixels(image.npixels),
    width(image.width), 
    height(image.height)
    
{
  // Allocate pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = image.pixels[i];
}



R2Image::
~R2Image(void)
{
  // Free image pixels
  if (pixels) delete [] pixels;
}



R2Image& R2Image::
operator=(const R2Image& image)
{
  // Delete previous pixels
  if (pixels) { delete [] pixels; pixels = NULL; }

  // Reset width and height
  npixels = image.npixels;
  width = image.width;
  height = image.height;

  // Allocate new pixels
  pixels = new R2Pixel [ npixels ];
  assert(pixels);

  // Copy pixels 
  for (int i = 0; i < npixels; i++) 
    pixels[i] = image.pixels[i];

  // Return image
  return *this;
}


void R2Image::
svdTest(void)
{
	// fit a 2D conic to five points
	R2Point p1(1.2,3.5);
	R2Point p2(2.1,2.2);
	R2Point p3(0.2,1.6);
	R2Point p4(0.0,0.5);
	R2Point p5(-0.2,4.2);

	// build the 5x6 matrix of equations
	double** linEquations = dmatrix(1,5,1,6);

	linEquations[1][1] = p1[0]*p1[0];
	linEquations[1][2] = p1[0]*p1[1];
	linEquations[1][3] = p1[1]*p1[1];
	linEquations[1][4] = p1[0];
	linEquations[1][5] = p1[1];
	linEquations[1][6] = 1.0;

	linEquations[2][1] = p2[0]*p2[0];
	linEquations[2][2] = p2[0]*p2[1];
	linEquations[2][3] = p2[1]*p2[1];
	linEquations[2][4] = p2[0];
	linEquations[2][5] = p2[1];
	linEquations[2][6] = 1.0;

	linEquations[3][1] = p3[0]*p3[0];
	linEquations[3][2] = p3[0]*p3[1];
	linEquations[3][3] = p3[1]*p3[1];
	linEquations[3][4] = p3[0];
	linEquations[3][5] = p3[1];
	linEquations[3][6] = 1.0;
	
	linEquations[4][1] = p4[0]*p4[0];
	linEquations[4][2] = p4[0]*p4[1];
	linEquations[4][3] = p4[1]*p4[1];
	linEquations[4][4] = p4[0];
	linEquations[4][5] = p4[1];
	linEquations[4][6] = 1.0;

	linEquations[5][1] = p5[0]*p5[0];
	linEquations[5][2] = p5[0]*p5[1];
	linEquations[5][3] = p5[1]*p5[1];
	linEquations[5][4] = p5[0];
	linEquations[5][5] = p5[1];
	linEquations[5][6] = 1.0;

	printf("\n Fitting a conic to five points:\n");
	printf("Point #1: %f,%f\n",p1[0],p1[1]);
	printf("Point #2: %f,%f\n",p2[0],p2[1]);
	printf("Point #3: %f,%f\n",p3[0],p3[1]);
	printf("Point #4: %f,%f\n",p4[0],p4[1]);
	printf("Point #5: %f,%f\n",p5[0],p5[1]);

	// compute the SVD
	double singularValues[7]; // 1..6
	double** nullspaceMatrix = dmatrix(1,6,1,6);
	svdcmp(linEquations, 5, 6, singularValues, nullspaceMatrix);

	// get the result
	printf("\n Singular values: %f, %f, %f, %f, %f, %f\n",singularValues[1],singularValues[2],singularValues[3],singularValues[4],singularValues[5],singularValues[6]);

	// find the smallest singular value:
	int smallestIndex = 1;
	for(int i=2;i<7;i++) if(singularValues[i]<singularValues[smallestIndex]) smallestIndex=i;

	// solution is the nullspace of the matrix, which is the column in V corresponding to the smallest singular value (which should be 0)
	printf("Conic coefficients: %f, %f, %f, %f, %f, %f\n",nullspaceMatrix[1][smallestIndex],nullspaceMatrix[2][smallestIndex],nullspaceMatrix[3][smallestIndex],nullspaceMatrix[4][smallestIndex],nullspaceMatrix[5][smallestIndex],nullspaceMatrix[6][smallestIndex]);

	// make sure the solution is correct:
	printf("Equation #1 result: %f\n",	p1[0]*p1[0]*nullspaceMatrix[1][smallestIndex] + 
										p1[0]*p1[1]*nullspaceMatrix[2][smallestIndex] + 
										p1[1]*p1[1]*nullspaceMatrix[3][smallestIndex] + 
										p1[0]*nullspaceMatrix[4][smallestIndex] + 
										p1[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #2 result: %f\n",	p2[0]*p2[0]*nullspaceMatrix[1][smallestIndex] + 
										p2[0]*p2[1]*nullspaceMatrix[2][smallestIndex] + 
										p2[1]*p2[1]*nullspaceMatrix[3][smallestIndex] + 
										p2[0]*nullspaceMatrix[4][smallestIndex] + 
										p2[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #3 result: %f\n",	p3[0]*p3[0]*nullspaceMatrix[1][smallestIndex] + 
										p3[0]*p3[1]*nullspaceMatrix[2][smallestIndex] + 
										p3[1]*p3[1]*nullspaceMatrix[3][smallestIndex] + 
										p3[0]*nullspaceMatrix[4][smallestIndex] + 
										p3[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #4 result: %f\n",	p4[0]*p4[0]*nullspaceMatrix[1][smallestIndex] + 
										p4[0]*p4[1]*nullspaceMatrix[2][smallestIndex] + 
										p4[1]*p4[1]*nullspaceMatrix[3][smallestIndex] + 
										p4[0]*nullspaceMatrix[4][smallestIndex] + 
										p4[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	printf("Equation #5 result: %f\n",	p5[0]*p5[0]*nullspaceMatrix[1][smallestIndex] + 
										p5[0]*p5[1]*nullspaceMatrix[2][smallestIndex] + 
										p5[1]*p5[1]*nullspaceMatrix[3][smallestIndex] + 
										p5[0]*nullspaceMatrix[4][smallestIndex] + 
										p5[1]*nullspaceMatrix[5][smallestIndex] + 
										nullspaceMatrix[6][smallestIndex]);

	R2Point test_point(0.34,-2.8);

	printf("A point off the conic: %f\n",	test_point[0]*test_point[0]*nullspaceMatrix[1][smallestIndex] + 
											test_point[0]*test_point[1]*nullspaceMatrix[2][smallestIndex] + 
											test_point[1]*test_point[1]*nullspaceMatrix[3][smallestIndex] + 
											test_point[0]*nullspaceMatrix[4][smallestIndex] + 
											test_point[1]*nullspaceMatrix[5][smallestIndex] + 
											nullspaceMatrix[6][smallestIndex]);

	return;	
}


/////////////////////////////////////////////////////////////////////////
////////////////////// FUNCTIONS FOR MAGIC FRAME ////////////////////////
/////////////////////////////////////////////////////////////////////////

void R2Image::mapFramePixels(R2Image * freezeFrame, Point origCorners[4], Point curCorners[4]) {
   
  //  1) detect 4 corners in "this" image translated from previous
  detectLocalCorners(curCorners);
  //  2) create H homography matrix from the point correspondences of the corners
  double ** model = DLT(curCorners, origCorners);
  //  3) map all points within the frozen image to their locations (Hx = x')
  //      in "this" image and overwrite the pixels with the frozen image pixels
  inverseWarp(freezeFrame, curCorners, model);
}

void R2Image::inverseWarp(R2Image * freezeFrame, Point curCorners[4], double ** model) {
	// figure out orientation of rectangle -- use fact that farthest corner from a given corner is the diagonal
	Point pt1 = curCorners[0];
	double dist1 = 9999999.9, dist2 = 9999999.9, dist3 = 9999999.9;
	int ind1, ind2, ind3;
	for (int j = 1; j < 4; j++) {
		double dist = sqrt(pow(pt1.x - curCorners[j].x,2) + pow(pt1.y - curCorners[j].y,2));
		if (dist < dist1) {
			dist3 = dist2;
			dist2 = dist1;
			dist1 = dist;
			ind3 = ind2;
			ind2 = ind1;
			ind1 = j;
		} else if (dist < dist2) {
			dist3 = dist2;
			dist2 = dist;
			ind3 = ind2;
			ind2 = j;
		} else {
			dist3 = dist;
			ind3 = j;
		}
	}
	// now create corners array that has the corners in order of going around the rectangle
	Point corners[4];
	corners[0] = pt1;
	corners[1] = curCorners[ind1];
	corners[2] = curCorners[ind3];
	corners[3] = curCorners[ind2];

	// create equations for borders of rectangle
	double m12 = 0.0,m23 = 0.0,m34 = 0.0,m41 = 0.0,b12 = 0.0,b23 = 0.0,b34 = 0.0,b41 = 0.0;
	if (corners[1].x == corners[0].x) {
		corners[1].x = corners[1].x + 1;
	}
	m12 = ((double)corners[1].y - corners[0].y) / ((double)corners[1].x - corners[0].x);
	b12 = corners[1].y - (m12 * corners[1].x);
	if (corners[2].x == corners[1].x) {
		corners[2].x = corners[2].x + 1;
	}
	m23 = ((double)corners[2].y - corners[1].y) / ((double)corners[2].x - corners[1].x);
	b23 = corners[2].y - (m23 * corners[2].x);
	if (corners[3].x == corners[2].x) {
		corners[3].x = corners[3].x + 1;
	}
	m34 = ((double)corners[3].y - corners[2].y) / ((double)corners[3].x - corners[2].x);
	b34 = corners[3].y - (m34 * corners[3].x);
	if (corners[4].x == corners[1].x) {
		corners[4].x = corners[4].x + 1;
	}
	m41 = ((double)corners[0].y - corners[3].y) / ((double)corners[0].x - corners[3].x);
	b41 = corners[0].y - (m41 * corners[0].x);

	// "above" variables are true if we want points above corresponding line
	bool above12 = false, above23 = false, above34 = false, above41 = false;
	if (corners[2].y >= (m12 * corners[2].x) + b12) {
		above12 = true;
	}
	if (corners[0].y >= (m23 * corners[0].x) + b23) {
		above23 = true;
	}
	if (corners[0].y >= (m34 * corners[0].x) + b34) {
		above34 = true;
	}
	if (corners[2].y >= (m41 * corners[2].x) + b41) {
		above41 = true;
	}

	for (int i = 0; i < width; i++) {
    	for (int j = 0; j < height; j++) {
    		if (((above12 && (j >= (m12 * i) + b12)) || (!above12 && (j <= (m12 * i) + b12))) &&
		        ((above23 && (j >= (m23 * i) + b23)) || (!above23 && (j <= (m23 * i) + b23))) &&
		        ((above34 && (j >= (m34 * i) + b34)) || (!above34 && (j <= (m34 * i) + b34))) &&
		        ((above41 && (j >= (m41 * i) + b41)) || (!above41 && (j <= (m41 * i) + b41)))) {

    			// these are the pixels to warp -- Hx = x'
	 			Point estimation;
		        float estx = (model[0][0] * i) + (model[0][1] * j) + model[0][2];
		        float esty = (model[1][0] * i) + (model[1][1] * j) + model[1][2];
		        float estz = (model[2][0] * i) + (model[2][1] * j) + model[2][2];
		        estimation.x = (int) (estx / estz);
		        estimation.y = (int) (esty / estz);

		        //// testing homography model ////
		        if (estimation.x < 0 || estimation.y < 0 || estimation.x > width || estimation.y > height) {
		        	fprintf(stderr,"Oops, (%d , %d) not on the image\n",estimation.x,estimation.y);
		        }

		        Pixel(i,j) = freezeFrame->Pixel(estimation.x, estimation.y);
		        // replace above to fill in frame with black:
		        //Pixel(i,j).SetRed(0.0); Pixel(i,j).SetGreen(0.0); Pixel(i,j).SetBlue(0.0); 
		        Pixel(i,j).Clamp();
    		}
    	}
    }
}


// Detects the locations of the 4 corners in "this" image, and fills
//    in the given "corners" array with the point coordinates, so they
//    can be accessed from the calling function
void R2Image::detectFrameCorners(Point corners[4]) {
  std::vector<Point> greenPts;

  // find the MAX total green + blue components of any single pixel in the image
  float max_GandB = 0.0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      float currGB = Pixel(i,j).Green() + Pixel(i,j).Blue();
      if (currGB > max_GandB) {
        max_GandB = currGB;
      }
    }
  }

  // find all points that fit into the given constraints (i.e. GREEN enough points)
  Point currPt;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      float currGB = Pixel(i,j).Green() + Pixel(i,j).Blue();
      if (currGB / max_GandB > .3 && Pixel(i,j).Green() > Pixel(i,j).Blue() && Pixel(i,j).Red() < .2) {
        currPt.x = i;
        currPt.y = j;
        greenPts.push_back(currPt);
      }
    }
  }

  Point centroids[4];
  // initialize centroids for K-means:
  for (int i = 0; i < 4; i++) {
    int randPtInd;
    while (true) {
      randPtInd = rand() % greenPts.size();
      bool valid = true;
      for (int j = 0; j < i; j++) {
        if (abs(greenPts[randPtInd].x - centroids[j].x) < 100 && abs(greenPts[randPtInd].y - centroids[j].y) < 100) {
          valid = false;
        }
      }
      if (valid == true) {
        centroids[i] = greenPts[randPtInd];
        break;
      }
    }
  }

  // modified k-means 5 iterations -- 
  // might actually be able to find correct centroids with only 1 iteration if no outlying green points
  int ptInds[greenPts.size()];
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < greenPts.size(); j++) {
      int closestCentroid = 0;
      for (int k = 1; k < 4; k++) {
        if (sqrt(pow(greenPts[j].x - centroids[k].x,2) + pow(greenPts[j].y - centroids[k].y,2)) < 
              sqrt(pow(greenPts[j].x - centroids[closestCentroid].x,2) + pow(greenPts[j].y - centroids[closestCentroid].y,2))) {
          closestCentroid = k;
        }
      }
      ptInds[j] = closestCentroid;
    }

    for (int k = 0; k < 4; k++) {
      float numPts = 0;
      int totX = 0;
      int totY = 0;
      for (int j = 0; j < greenPts.size(); j++) {
        if (ptInds[j] == k) {
          totX += greenPts[j].x;
          totY += greenPts[j].y;
          numPts++;
        }
      }
      centroids[k].x = totX / numPts;
      centroids[k].y = totY / numPts;
    }
  }

  corners[0] = centroids[0];
  corners[1] = centroids[1];
  corners[2] = centroids[2];
  corners[3] = centroids[3];
}


// Detects the locations of the 4 corners in "this" image, and fills
//    in the given "corners" array with the point coordinates, so they
//    can be accessed from the calling function
void R2Image::detectLocalCorners(Point corners[4]) {
  std::vector<Point> greenPts;

  // find the MAX total green + blue components of any single pixel in the image
  float max_GandB = 0.0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      float currGB = Pixel(i,j).Green() + Pixel(i,j).Blue();
      if (currGB > max_GandB) {
        max_GandB = currGB;
      }
    }
  }

  // find all points that fit into the given constraints (i.e. GREEN enough points)
  Point currPt;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      float currGB = Pixel(i,j).Green() + Pixel(i,j).Blue();
      if (currGB / max_GandB > .3 && Pixel(i,j).Green() > Pixel(i,j).Blue() && Pixel(i,j).Red() < .2) {
        currPt.x = i;
        currPt.y = j;
        greenPts.push_back(currPt);
      }
    }
  }

  Point centroids[4];
  // initialize centroids for K-means:
  for (int i = 0; i < 4; i++) {
    int randPtInd;
    while (true) {
      randPtInd = rand() % greenPts.size();
      bool valid = true;
      for (int j = 0; j < i; j++) {
        if (abs(greenPts[randPtInd].x - centroids[j].x) < 100 && abs(greenPts[randPtInd].y - centroids[j].y) < 100) {
          valid = false;
        }
      }
      if (valid == true) {
        centroids[i] = greenPts[randPtInd];
        break;
      }
    }
  }

  // modified k-means 5 iterations -- 
  // might actually be able to find correct centroids with only 1 iteration if no outlying green points
  int ptInds[greenPts.size()];
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < greenPts.size(); j++) {
      int closestCentroid = 0;
      for (int k = 1; k < 4; k++) {
        if (sqrt(pow(greenPts[j].x - centroids[k].x,2) + pow(greenPts[j].y - centroids[k].y,2)) < 
              sqrt(pow(greenPts[j].x - centroids[closestCentroid].x,2) + pow(greenPts[j].y - centroids[closestCentroid].y,2))) {
          closestCentroid = k;
        }
      }
      ptInds[j] = closestCentroid;
    }

    for (int k = 0; k < 4; k++) {
      float numPts = 0;
      int totX = 0;
      int totY = 0;
      for (int j = 0; j < greenPts.size(); j++) {
        if (ptInds[j] == k) {
          totX += greenPts[j].x;
          totY += greenPts[j].y;
          numPts++;
        }
      }
      centroids[k].x = totX / numPts;
      centroids[k].y = totY / numPts;
    }
  }

  // map closest centroids to corners to each other
  for (int i = 0; i < 4; i++) { // loop through corners
    double lowest_dist = FLT_MAX;
    int lowest_centroid_index = 0;
    for (int j = 0; j < 4; j++) { // loop through centroids
      double dist = sqrt(pow(centroids[j].x-corners[i].x,2) + pow(centroids[j].y-corners[i].y,2));
      // update if necessary
      if (dist < lowest_dist) {
        lowest_dist = dist;
        lowest_centroid_index = j;
      } 
    }
    corners[i] = centroids[lowest_centroid_index];
  }
}


// Computes and returns the model homography matrix given 4 point correspondences
// If 'fromPoints' and 'toPoints' are from image A and B respectively, the returned
//    homography matrix 'H' should calculate x' = Hx where x is a point of A and x' is
//    the corresponding point of B (both points in homogeneous coordinates)
double** R2Image::DLT(Point fromPoints[4], Point toPoints[4]) {
  double** Ai = dmatrix(1,8,1,9);
  int r1,r2;
  int pt1[3], pt2[3];
  for (int i = 0; i < 4; i++) {
    pt1[0] = fromPoints[i].x;
    pt1[1] = fromPoints[i].y;
    pt1[2] = 1;
    pt2[0] = toPoints[i].x;
    pt2[1] = toPoints[i].y;
    pt2[2] = 1;
    r1 = (2*i)+1;
    r2 = (2*i)+2;
    for (int j = 1; j<=3; j++) {
      Ai[r1][j] = 0;
      Ai[r1][j+3] = (-1)*pt2[2]*pt1[j-1];
      Ai[r1][j+6] = pt2[1]*pt1[j-1];
      Ai[r2][j] = pt2[2]*pt1[j-1];
      Ai[r2][j+3] = 0;
      Ai[r2][j+6] = (-1)*pt2[0]*pt1[j-1];
    }
  }

  // now that the Ai matrix is populated, run svd on it:
  double singularValues[7]; // 1..6
  double** nullspaceMatrix = dmatrix(1,9,1,9);
  svdcmp(Ai, 8, 9, singularValues, nullspaceMatrix);
  // now find smallest SV and take corresponding eigenvector as H to return
  int smallestIndex = 1;
  for(int i = 2; i < 10; i++) {
    if(singularValues[i] < singularValues[smallestIndex]) {
      smallestIndex=i;
    }
  }

  double** H = new double*[3];
  H[0] = new double[3];
  H[1] = new double[3];
  H[2] = new double[3];
  for (int i = 0; i < 9; i++) {
    H[i/3][i%3] = nullspaceMatrix[i+1][smallestIndex];
  }
  return H;
}

////////////////////////////////////////////////////////////////////////
// Image processing functions
// YOU IMPLEMENT THE FUNCTIONS IN THIS SECTION
////////////////////////////////////////////////////////////////////////

// Per-pixel Operations ////////////////////////////////////////////////

void R2Image::
Brighten(double factor)
{
  // Brighten the image by multiplying each pixel component by the factor.
  // This is implemented for you as an example of how to access and set pixels
  for (int i = 0; i < width; i++) {
    for (int j = 0;  j < height; j++) {
      Pixel(i,j) *= factor;
      Pixel(i,j).Clamp();
    }
  }
}

void R2Image::
SobelX(void)
{
	// Apply the Sobel oprator to the image in X direction
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "SobelX() not implemented\n");
}

void R2Image::
SobelY(void)
{
	// Apply the Sobel oprator to the image in Y direction
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "SobelY() not implemented\n");
}

void R2Image::
LoG(void)
{
  // Apply the LoG oprator to the image
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "LoG() not implemented\n");
}



void R2Image::
ChangeSaturation(double factor)
{
  // Changes the saturation of an image
  // Find a formula that changes the saturation without affecting the image brightness

  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "ChangeSaturation(%g) not implemented\n", factor);
}


// Linear filtering ////////////////////////////////////////////////
void R2Image::
Blur(double sigma)
{
  // Gaussian blur of the image. Separable solution is preferred
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "Blur(%g) not implemented\n", sigma);
}


void R2Image::
Harris(double sigma)
{
    // Harris corner detector. Make use of the previously developed filters, such as the Gaussian blur filter
	// Output should be 50% grey at flat regions, white at corners and black/dark near edges
  
  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "Harris(%g) not implemented\n", sigma);
}


void R2Image::
Sharpen()
{
  // Sharpen an image using a linear filter. Use a kernel of your choosing.

  // FILL IN IMPLEMENTATION HERE (REMOVE PRINT STATEMENT WHEN DONE)
  fprintf(stderr, "Sharpen() not implemented\n");
}


void R2Image::
blendOtherImageTranslated(R2Image * otherImage)
{
	// find at least 100 features on this image, and another 100 on the "otherImage". Based on these,
	// compute the matching translation (pixel precision is OK), and blend the translated "otherImage" 
	// into this image with a 50% opacity.
	fprintf(stderr, "fit other image using translation and blend imageB over imageA\n");
	return;
}

void R2Image::
blendOtherImageHomography(R2Image * otherImage)
{
	// find at least 100 features on this image, and another 100 on the "otherImage". Based on these,
	// compute the matching homography, and blend the transformed "otherImage" into this image with a 50% opacity.
	fprintf(stderr, "fit other image using a homography and blend imageB over imageA\n");
	return;
}


////////////////////////////////////////////////////////////////////////
// I/O Functions
////////////////////////////////////////////////////////////////////////

int R2Image::
Read(const char *filename)
{
  // Initialize everything
  if (pixels) { delete [] pixels; pixels = NULL; }
  npixels = width = height = 0;

  // Parse input filename extension
  char *input_extension;
  if (!(input_extension = (char*)strrchr(filename, '.'))) {
    fprintf(stderr, "Input file has no extension (e.g., .jpg).\n");
    return 0;
  }
  
  // Read file of appropriate type
  if (!strncmp(input_extension, ".bmp", 4)) return ReadBMP(filename);
  else if (!strncmp(input_extension, ".ppm", 4)) return ReadPPM(filename);
  else if (!strncmp(input_extension, ".jpg", 4)) return ReadJPEG(filename);
  else if (!strncmp(input_extension, ".jpeg", 5)) return ReadJPEG(filename);
  
  // Should never get here
  fprintf(stderr, "Unrecognized image file extension");
  return 0;
}



int R2Image::
Write(const char *filename) const
{
  // Parse input filename extension
  char *input_extension;
  if (!(input_extension = (char*)strrchr(filename, '.'))) {
    fprintf(stderr, "Input file has no extension (e.g., .jpg).\n");
    return 0;
  }
  
  // Write file of appropriate type
  if (!strncmp(input_extension, ".bmp", 4)) return WriteBMP(filename);
  else if (!strncmp(input_extension, ".ppm", 4)) return WritePPM(filename, 1);
  else if (!strncmp(input_extension, ".jpg", 5)) return WriteJPEG(filename);
  else if (!strncmp(input_extension, ".jpeg", 5)) return WriteJPEG(filename);

  // Should never get here
  fprintf(stderr, "Unrecognized image file extension");
  return 0;
}



////////////////////////////////////////////////////////////////////////
// BMP I/O
////////////////////////////////////////////////////////////////////////

#if (RN_OS == RN_LINUX) && !WIN32

typedef struct tagBITMAPFILEHEADER {
  unsigned short int bfType;
  unsigned int bfSize;
  unsigned short int bfReserved1;
  unsigned short int bfReserved2;
  unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  unsigned int biSize;
  int biWidth;
  int biHeight;
  unsigned short int biPlanes;
  unsigned short int biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  int biXPelsPerMeter;
  int biYPelsPerMeter;
  unsigned int biClrUsed;
  unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBTRIPLE {
  unsigned char rgbtBlue;
  unsigned char rgbtGreen;
  unsigned char rgbtRed;
} RGBTRIPLE;

typedef struct tagRGBQUAD {
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char rgbReserved;
} RGBQUAD;

#endif

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#define BMP_BF_TYPE 0x4D42 /* word BM */
#define BMP_BF_OFF_BITS 54 /* 14 for file header + 40 for info header (not sizeof(), but packed size) */
#define BMP_BI_SIZE 40 /* packed size of info header */


static unsigned short int WordReadLE(FILE *fp)
{
  // Read a unsigned short int from a file in little endian format 
  unsigned short int lsb, msb;
  lsb = getc(fp);
  msb = getc(fp);
  return (msb << 8) | lsb;
}



static void WordWriteLE(unsigned short int x, FILE *fp)
{
  // Write a unsigned short int to a file in little endian format
  unsigned char lsb = (unsigned char) (x & 0x00FF); putc(lsb, fp); 
  unsigned char msb = (unsigned char) (x >> 8); putc(msb, fp);
}



static unsigned int DWordReadLE(FILE *fp)
{
  // Read a unsigned int word from a file in little endian format 
  unsigned int b1 = getc(fp);
  unsigned int b2 = getc(fp);
  unsigned int b3 = getc(fp);
  unsigned int b4 = getc(fp);
  return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



static void DWordWriteLE(unsigned int x, FILE *fp)
{
  // Write a unsigned int to a file in little endian format 
  unsigned char b1 = (x & 0x000000FF); putc(b1, fp);
  unsigned char b2 = ((x >> 8) & 0x000000FF); putc(b2, fp);
  unsigned char b3 = ((x >> 16) & 0x000000FF); putc(b3, fp);
  unsigned char b4 = ((x >> 24) & 0x000000FF); putc(b4, fp);
}



static int LongReadLE(FILE *fp)
{
  // Read a int word from a file in little endian format 
  int b1 = getc(fp);
  int b2 = getc(fp);
  int b3 = getc(fp);
  int b4 = getc(fp);
  return (b4 << 24) | (b3 << 16) | (b2 << 8) | b1;
}



static void LongWriteLE(int x, FILE *fp)
{
  // Write a int to a file in little endian format 
  char b1 = (x & 0x000000FF); putc(b1, fp);
  char b2 = ((x >> 8) & 0x000000FF); putc(b2, fp);
  char b3 = ((x >> 16) & 0x000000FF); putc(b3, fp);
  char b4 = ((x >> 24) & 0x000000FF); putc(b4, fp);
}



int R2Image::
ReadBMP(const char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  /* Read file header */
  BITMAPFILEHEADER bmfh;
  bmfh.bfType = WordReadLE(fp);
  bmfh.bfSize = DWordReadLE(fp);
  bmfh.bfReserved1 = WordReadLE(fp);
  bmfh.bfReserved2 = WordReadLE(fp);
  bmfh.bfOffBits = DWordReadLE(fp);
  
  /* Check file header */
  assert(bmfh.bfType == BMP_BF_TYPE);
  /* ignore bmfh.bfSize */
  /* ignore bmfh.bfReserved1 */
  /* ignore bmfh.bfReserved2 */
  assert(bmfh.bfOffBits == BMP_BF_OFF_BITS);
  
  /* Read info header */
  BITMAPINFOHEADER bmih;
  bmih.biSize = DWordReadLE(fp);
  bmih.biWidth = LongReadLE(fp);
  bmih.biHeight = LongReadLE(fp);
  bmih.biPlanes = WordReadLE(fp);
  bmih.biBitCount = WordReadLE(fp);
  bmih.biCompression = DWordReadLE(fp);
  bmih.biSizeImage = DWordReadLE(fp);
  bmih.biXPelsPerMeter = LongReadLE(fp);
  bmih.biYPelsPerMeter = LongReadLE(fp);
  bmih.biClrUsed = DWordReadLE(fp);
  bmih.biClrImportant = DWordReadLE(fp);
  
  // Check info header 
  assert(bmih.biSize == BMP_BI_SIZE);
  assert(bmih.biWidth > 0);
  assert(bmih.biHeight > 0);
  assert(bmih.biPlanes == 1);
  assert(bmih.biBitCount == 24);  /* RGB */
  assert(bmih.biCompression == BI_RGB);   /* RGB */
  int lineLength = bmih.biWidth * 3;  /* RGB */
  if ((lineLength % 4) != 0) lineLength = (lineLength / 4 + 1) * 4;
  assert(bmih.biSizeImage == (unsigned int) lineLength * (unsigned int) bmih.biHeight);

  // Assign width, height, and number of pixels
  width = bmih.biWidth;
  height = bmih.biHeight;
  npixels = width * height;

  // Allocate unsigned char buffer for reading pixels
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = bmih.biSizeImage;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Read buffer 
  fseek(fp, (long) bmfh.bfOffBits, SEEK_SET);
  if (fread(buffer, 1, bmih.biSizeImage, fp) != bmih.biSizeImage) {
    fprintf(stderr, "Error while reading BMP file %s", filename);
    return 0;
  }

  // Close file
  fclose(fp);

  // Allocate pixels for image
  pixels = new R2Pixel [ width * height ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Assign pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      double b = (double) *(p++) / 255;
      double g = (double) *(p++) / 255;
      double r = (double) *(p++) / 255;
      R2Pixel pixel(r, g, b, 1);
      SetPixel(i, j, pixel);
    }
  }

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return success
  return 1;
}



int R2Image::
WriteBMP(const char *filename) const
{
  // Open file
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Compute number of bytes in row
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;

  // Write file header 
  BITMAPFILEHEADER bmfh;
  bmfh.bfType = BMP_BF_TYPE;
  bmfh.bfSize = BMP_BF_OFF_BITS + rowsize * height;
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits = BMP_BF_OFF_BITS;
  WordWriteLE(bmfh.bfType, fp);
  DWordWriteLE(bmfh.bfSize, fp);
  WordWriteLE(bmfh.bfReserved1, fp);
  WordWriteLE(bmfh.bfReserved2, fp);
  DWordWriteLE(bmfh.bfOffBits, fp);

  // Write info header 
  BITMAPINFOHEADER bmih;
  bmih.biSize = BMP_BI_SIZE;
  bmih.biWidth = width;
  bmih.biHeight = height;
  bmih.biPlanes = 1;
  bmih.biBitCount = 24;       /* RGB */
  bmih.biCompression = BI_RGB;    /* RGB */
  bmih.biSizeImage = rowsize * (unsigned int) bmih.biHeight;  /* RGB */
  bmih.biXPelsPerMeter = 2925;
  bmih.biYPelsPerMeter = 2925;
  bmih.biClrUsed = 0;
  bmih.biClrImportant = 0;
  DWordWriteLE(bmih.biSize, fp);
  LongWriteLE(bmih.biWidth, fp);
  LongWriteLE(bmih.biHeight, fp);
  WordWriteLE(bmih.biPlanes, fp);
  WordWriteLE(bmih.biBitCount, fp);
  DWordWriteLE(bmih.biCompression, fp);
  DWordWriteLE(bmih.biSizeImage, fp);
  LongWriteLE(bmih.biXPelsPerMeter, fp);
  LongWriteLE(bmih.biYPelsPerMeter, fp);
  DWordWriteLE(bmih.biClrUsed, fp);
  DWordWriteLE(bmih.biClrImportant, fp);

  // Write image, swapping blue and red in each pixel
  int pad = rowsize - width * 3;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      const R2Pixel& pixel = (*this)[i][j];
      double r = 255.0 * pixel.Red();
      double g = 255.0 * pixel.Green();
      double b = 255.0 * pixel.Blue();
      if (r >= 255) r = 255;
      if (g >= 255) g = 255;
      if (b >= 255) b = 255;
      fputc((unsigned char) b, fp);
      fputc((unsigned char) g, fp);
      fputc((unsigned char) r, fp);
    }

    // Pad row
    for (int i = 0; i < pad; i++) fputc(0, fp);
  }
  
  // Close file
  fclose(fp);

  // Return success
  return 1;  
}



////////////////////////////////////////////////////////////////////////
// PPM I/O
////////////////////////////////////////////////////////////////////////

int R2Image::
ReadPPM(const char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Read PPM file magic identifier
  char buffer[128];
  if (!fgets(buffer, 128, fp)) {
    fprintf(stderr, "Unable to read magic id in PPM file");
    fclose(fp);
    return 0;
  }

  // skip comments
  int c = getc(fp);
  while (c == '#') {
    while (c != '\n') c = getc(fp);
    c = getc(fp);
  }
  ungetc(c, fp);

  // Read width and height
  if (fscanf(fp, "%d%d", &width, &height) != 2) {
    fprintf(stderr, "Unable to read width and height in PPM file");
    fclose(fp);
    return 0;
  }
	
  // Read max value
  double max_value;
  if (fscanf(fp, "%lf", &max_value) != 1) {
    fprintf(stderr, "Unable to read max_value in PPM file");
    fclose(fp);
    return 0;
  }
	
  // Allocate image pixels
  pixels = new R2Pixel [ width * height ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for PPM file");
    fclose(fp);
    return 0;
  }

  // Check if raw or ascii file
  if (!strcmp(buffer, "P6\n")) {
    // Read up to one character of whitespace (\n) after max_value
    int c = getc(fp);
    if (!isspace(c)) putc(c, fp);

    // Read raw image data 
    // First ppm pixel is top-left, so read in opposite scan-line order
    for (int j = height-1; j >= 0; j--) {
      for (int i = 0; i < width; i++) {
        double r = (double) getc(fp) / max_value;
        double g = (double) getc(fp) / max_value;
        double b = (double) getc(fp) / max_value;
        R2Pixel pixel(r, g, b, 1);
        SetPixel(i, j, pixel);
      }
    }
  }
  else {
    // Read asci image data 
    // First ppm pixel is top-left, so read in opposite scan-line order
    for (int j = height-1; j >= 0; j--) {
      for (int i = 0; i < width; i++) {
	// Read pixel values
	int red, green, blue;
	if (fscanf(fp, "%d%d%d", &red, &green, &blue) != 3) {
	  fprintf(stderr, "Unable to read data at (%d,%d) in PPM file", i, j);
	  fclose(fp);
	  return 0;
	}

	// Assign pixel values
	double r = (double) red / max_value;
	double g = (double) green / max_value;
	double b = (double) blue / max_value;
        R2Pixel pixel(r, g, b, 1);
        SetPixel(i, j, pixel);
      }
    }
  }

  // Close file
  fclose(fp);

  // Return success
  return 1;
}



int R2Image::
WritePPM(const char *filename, int ascii) const
{
  // Check type
  if (ascii) {
    // Open file
    FILE *fp = fopen(filename, "w");
    if (!fp) {
      fprintf(stderr, "Unable to open image file: %s", filename);
      return 0;
    }

    // Print PPM image file 
    // First ppm pixel is top-left, so write in opposite scan-line order
    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int j = height-1; j >= 0 ; j--) {
      for (int i = 0; i < width; i++) {
        const R2Pixel& p = (*this)[i][j];
        int r = (int) (255 * p.Red());
        int g = (int) (255 * p.Green());
        int b = (int) (255 * p.Blue());
        fprintf(fp, "%-3d %-3d %-3d  ", r, g, b);
        if (((i+1) % 4) == 0) fprintf(fp, "\n");
      }
      if ((width % 4) != 0) fprintf(fp, "\n");
    }
    fprintf(fp, "\n");

    // Close file
    fclose(fp);
  }
  else {
    // Open file
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
      fprintf(stderr, "Unable to open image file: %s", filename);
      return 0;
    }
    
    // Print PPM image file 
    // First ppm pixel is top-left, so write in opposite scan-line order
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "255\n");
    for (int j = height-1; j >= 0 ; j--) {
      for (int i = 0; i < width; i++) {
        const R2Pixel& p = (*this)[i][j];
        int r = (int) (255 * p.Red());
        int g = (int) (255 * p.Green());
        int b = (int) (255 * p.Blue());
        fprintf(fp, "%c%c%c", r, g, b);
      }
    }
    
    // Close file
    fclose(fp);
  }

  // Return success
  return 1;  
}



////////////////////////////////////////////////////////////////////////
// JPEG I/O
////////////////////////////////////////////////////////////////////////


// #define USE_JPEG
#ifdef USE_JPEG
  extern "C" { 
#   define XMD_H // Otherwise, a conflict with INT32
#   undef FAR // Otherwise, a conflict with windows.h
#   include "jpeg/jpeglib.h"
  };
#endif



int R2Image::
ReadJPEG(const char *filename)
{
#ifdef USE_JPEG
  // Open file
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Initialize decompression info
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  // Remember image attributes
  width = cinfo.output_width;
  height = cinfo.output_height;
  npixels = width * height;
  int ncomponents = cinfo.output_components;

  // Allocate pixels for image
  pixels = new R2Pixel [ npixels ];
  if (!pixels) {
    fprintf(stderr, "Unable to allocate memory for BMP file");
    fclose(fp);
    return 0;
  }

  // Allocate unsigned char buffer for reading image
  int rowsize = ncomponents * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = rowsize * height;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for JPEG file");
    fclose(fp);
    return 0;
  }

  // Read scan lines 
  // First jpeg pixel is top-left, so read pixels in opposite scan-line order
  while (cinfo.output_scanline < cinfo.output_height) {
    int scanline = cinfo.output_height - cinfo.output_scanline - 1;
    unsigned char *row_pointer = &buffer[scanline * rowsize];
    jpeg_read_scanlines(&cinfo, &row_pointer, 1);
  }

  // Free everything
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  // Close file
  fclose(fp);

  // Assign pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      double r, g, b, a;
      if (ncomponents == 1) {
        r = g = b = (double) *(p++) / 255;
        a = 1;
      }
      else if (ncomponents == 1) {
        r = g = b = (double) *(p++) / 255;
        a = 1;
        p++;
      }
      else if (ncomponents == 3) {
        r = (double) *(p++) / 255;
        g = (double) *(p++) / 255;
        b = (double) *(p++) / 255;
        a = 1;
      }
      else if (ncomponents == 4) {
        r = (double) *(p++) / 255;
        g = (double) *(p++) / 255;
        b = (double) *(p++) / 255;
        a = (double) *(p++) / 255;
      }
      else {
        fprintf(stderr, "Unrecognized number of components in jpeg image: %d\n", ncomponents);
        return 0;
      }
      R2Pixel pixel(r, g, b, a);
      SetPixel(i, j, pixel);
    }
  }

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return success
  return 1;
#else
  fprintf(stderr, "JPEG not supported");
  return 0;
#endif
}


	

int R2Image::
WriteJPEG(const char *filename) const
{
#ifdef USE_JPEG
  // Open file
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open image file: %s", filename);
    return 0;
  }

  // Initialize compression info
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, fp);
  cinfo.image_width = width; 	/* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
  cinfo.dct_method = JDCT_ISLOW;
  jpeg_set_defaults(&cinfo);
  cinfo.optimize_coding = TRUE;
  jpeg_set_quality(&cinfo, 95, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
	
  // Allocate unsigned char buffer for reading image
  int rowsize = 3 * width;
  if ((rowsize % 4) != 0) rowsize = (rowsize / 4 + 1) * 4;
  int nbytes = rowsize * height;
  unsigned char *buffer = new unsigned char [nbytes];
  if (!buffer) {
    fprintf(stderr, "Unable to allocate temporary memory for JPEG file");
    fclose(fp);
    return 0;
  }

  // Fill buffer with pixels
  for (int j = 0; j < height; j++) {
    unsigned char *p = &buffer[j * rowsize];
    for (int i = 0; i < width; i++) {
      const R2Pixel& pixel = (*this)[i][j];
      int r = (int) (255 * pixel.Red());
      int g = (int) (255 * pixel.Green());
      int b = (int) (255 * pixel.Blue());
      if (r > 255) r = 255;
      if (g > 255) g = 255;
      if (b > 255) b = 255;
      *(p++) = r;
      *(p++) = g;
      *(p++) = b;
    }
  }



  // Output scan lines
  // First jpeg pixel is top-left, so write in opposite scan-line order
  while (cinfo.next_scanline < cinfo.image_height) {
    int scanline = cinfo.image_height - cinfo.next_scanline - 1;
    unsigned char *row_pointer = &buffer[scanline * rowsize];
    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
  }

  // Free everything
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);

  // Close file
  fclose(fp);

  // Free unsigned char buffer for reading pixels
  delete [] buffer;

  // Return number of bytes written
  return 1;
#else
  fprintf(stderr, "JPEG not supported");
  return 0;
#endif
}






