
// Computer Vision for Digital Post-Production
// Lecturer: Gergely Vass - vassg@vassg.hu
//
// Skeleton Code for programming assigments
// 
// Code originally from Thomas Funkhouser
// main.c
// original by Wagner Correa, 1999
// modified by Robert Osada, 2000
// modified by Renato Werneck, 2003
// modified by Jason Lawrence, 2004
// modified by Jason Lawrence, 2005
// modified by Forrester Cole, 2006
// modified by Tom Funkhouser, 2007
// modified by Chris DeCoro, 2007
//



// Include files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "R2/R2.h"
#include "R2Pixel.h"
#include "R2Image.h"



// Program arguments

static char options[] =
"  -help\n"
"  -svdTest\n"
"  -sobelX\n"
"  -sobelY\n"
"  -log\n"
"  -harris <real:sigma>\n"
"  -saturation <real:factor>\n"
"  -brightness <real:factor>\n"
"  -blur <real:sigma>\n"
"  -sharpen \n"
"  -matchTranslation <file:other_image>\n"
"  -matchHomography <file:other_image>\n"
"  -processVid <int:num_frames>\n"
"  -multipleFreezes <int:num_frames>\n";


static void 
ShowUsage(void)
{
  // Print usage message and exit
  fprintf(stderr, "Usage: imgpro input_image output_image [  -option [arg ...] ...]\n");
  fprintf(stderr, options);
  exit(EXIT_FAILURE);
}



static void 
CheckOption(char *option, int argc, int minargc)
{
  // Check if there are enough remaining arguments for option
  if (argc < minargc)  {
    fprintf(stderr, "Too few arguments for %s\n", option);
    ShowUsage();
    exit(-1);
  }
}



static int 
ReadCorrespondences(char *filename, R2Segment *&source_segments, R2Segment *&target_segments, int& nsegments)
{
  // Open file
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "Unable to open correspondences file %s\n", filename);
    exit(-1);
  }

  // Read number of segments
  if (fscanf(fp, "%d", &nsegments) != 1) {
    fprintf(stderr, "Unable to read correspondences file %s\n", filename);
    exit(-1);
  }

  // Allocate arrays for segments
  source_segments = new R2Segment [ nsegments ];
  target_segments = new R2Segment [ nsegments ];
  if (!source_segments || !target_segments) {
    fprintf(stderr, "Unable to allocate correspondence segments for %s\n", filename);
    exit(-1);
  }

  // Read segments
  for (int i = 0; i <  nsegments; i++) {

    // Read source segment
    double sx1, sy1, sx2, sy2;
    if (fscanf(fp, "%lf%lf%lf%lf", &sx1, &sy1, &sx2, &sy2) != 4) { 
      fprintf(stderr, "Error reading correspondence %d out of %d\n", i, nsegments);
      exit(-1);
    }

    // Read target segment
    double tx1, ty1, tx2, ty2;
    if (fscanf(fp, "%lf%lf%lf%lf", &tx1, &ty1, &tx2, &ty2) != 4) { 
      fprintf(stderr, "Error reading correspondence %d out of %d\n", i, nsegments);
      exit(-1);
    }

    // Add segments to list
    source_segments[i] = R2Segment(sx1, sy1, sx2, sy2);
    target_segments[i] = R2Segment(tx1, ty1, tx2, ty2);
  }

  // Close file
  fclose(fp);

  // Return success
  return 1;
}



int 
main(int argc, char **argv)
{
  // Look for help
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "-help")) {
      ShowUsage();
    }
	if (!strcmp(argv[i], "-svdTest")) {
      R2Image *image = new R2Image();
	  image->svdTest();
	  return 0;
    }
  }

  // Read input and output image filenames
  if (argc < 3)  ShowUsage();
  argv++, argc--; // First argument is program name
  char *input_folder_name = *argv; argv++, argc--; 
  char *output_folder_name = *argv; argv++, argc--; 

  // Allocate image
  R2Image *image = new R2Image();
  if (!image) {
    fprintf(stderr, "Unable to allocate image\n");
    exit(-1);
  }

  // Read input image
  /*if (!image->Read(input_image_name)) {
    fprintf(stderr, "Unable to read image from %s\n", input_image_name);
    exit(-1);
  }*/

  // Initialize sampling method
  int sampling_method = R2_IMAGE_POINT_SAMPLING;

  // Parse arguments and perform operations 
  while (argc > 0) {
    if (!strcmp(*argv, "-brightness")) {
      CheckOption(*argv, argc, 2);
      double factor = atof(argv[1]);
      argv += 2, argc -=2;
      image->Brighten(factor);
    }
	else if (!strcmp(*argv, "-sobelX")) {
      argv++, argc--;
      image->SobelX();
    }
	else if (!strcmp(*argv, "-sobelY")) {
      argv++, argc--;
      image->SobelY();
    }
	else if (!strcmp(*argv, "-log")) {
      argv++, argc--;
      image->LoG();
    }
    else if (!strcmp(*argv, "-saturation")) {
      CheckOption(*argv, argc, 2);
      double factor = atof(argv[1]);
      argv += 2, argc -= 2;
      image->ChangeSaturation(factor);
    }
	else if (!strcmp(*argv, "-harris")) {
      CheckOption(*argv, argc, 2);
      double sigma = atof(argv[1]);
      argv += 2, argc -= 2;
      image->Harris(sigma);
    }
    else if (!strcmp(*argv, "-blur")) {
      CheckOption(*argv, argc, 2);
      double sigma = atof(argv[1]);
      argv += 2, argc -= 2;
      image->Blur(sigma);
    }
    else if (!strcmp(*argv, "-sharpen")) {
      argv++, argc--;
      image->Sharpen();
    }
    else if (!strcmp(*argv, "-matchTranslation")) {
      CheckOption(*argv, argc, 2);
      R2Image *other_image = new R2Image(argv[1]);
      argv += 2, argc -= 2;
      image->blendOtherImageTranslated(other_image);
      delete other_image;
    }
    else if (!strcmp(*argv, "-matchHomography")) {
      CheckOption(*argv, argc, 2);
      R2Image *other_image = new R2Image(argv[1]);
      argv += 2, argc -= 2;
      image->blendOtherImageHomography(other_image);
      delete other_image;
    }
    else if (!strcmp(*argv, "-processVid")) {
      CheckOption(*argv, argc, 2);
      int num_frames = atof(argv[1]);
      argv += 2, argc -= 2;
      int start_tracking = 0; // set the frame number when we begin tracking the frame
      R2Image *image_frame;
      Point origCorners[4];
      Point currCorners[4];
      for (int i = 0; i < num_frames; i++) {
        char inputname[100], outname[100];;
        sprintf(inputname, "%s/%07d.jpg", input_folder_name, i+1);
        sprintf(outname, "%s/%07d.jpg", output_folder_name, i+1);
        image_frame = new R2Image(inputname);

        if (i == start_tracking) {
          // capture the frame we need to freeze
          image->Read(inputname);
          image->detectFrameCorners(origCorners);

          for (int j = 0; j < 4; j++) {
            currCorners[j] = origCorners[j];
          }
        } else if (i > start_tracking) {
          // find frame and replace inside of frame with frozen image (must deal with different angle of frame)
          image_frame->mapFramePixels(image, origCorners, currCorners);
        }
        fprintf(stderr,"Made it through, %d",i);
        image_frame->Write(outname);
        delete image_frame;
      }
    }
    else if (!strcmp(*argv, "-multipleFreezes")) {
      CheckOption(*argv, argc, 2);
      int num_frames = atof(argv[1]);

      /*int start1 = 45;
      int end1 = 116;
      int start2 = 174;
      int end2 = 235;
      int start3 = 285;*/

      int start1 = 54;
      int end1 = 145;
      int start2 = 190;
      int end2 = 260;
      int start3 = 302;

      R2Image *image2 = new R2Image();
      R2Image *image3 = new R2Image();

      argv += 3, argc -= 3;
      R2Image *image_frame;
      Point origCorners[4];
      Point currCorners[4];

      for (int i = 0; i < num_frames; i++) {
        char inputname[100], outname[100];;
        sprintf(inputname, "%s/%07d.jpg", input_folder_name, i+1);
        sprintf(outname, "%s/%07d.jpg", output_folder_name, i+1);
        image_frame = new R2Image(inputname);

        if (i == start1) {
          // capture the frame we need to freeze
          image->Read(inputname);
          image->detectFrameCorners(origCorners);


          for (int j = 0; j < 4; j++) {
            currCorners[j] = origCorners[j];
          }
        } else if (i == start2 || i == start3) {
          if (i == start2) {
            image2->Read(inputname);
            image2->detectFrameCorners(origCorners);
          } else {
            image3->Read(inputname);
            image3->detectFrameCorners(origCorners);
          }
          for (int j = 0; j < 4; j++) {
            currCorners[j] = origCorners[j];
          }
        } else if ((i < start1) || (i >= end1 && i < start2)|| (i >= end2 && i < start3)) {
          // do nothing
        } else if (i > start1 && i <= end1) {
          fprintf(stderr,"replacing frame1 on image %d   ",i);
          // find frame and replace inside of frame with frozen image (must deal with different angle of frame)
          image_frame->mapFramePixels(image, origCorners, currCorners);
          //return 1;
        } else if (i > start2 && i <= end2) {
          fprintf(stderr,"replacing frame2 on image %d   ",i);
          image_frame->mapFramePixels(image2, origCorners, currCorners);
        } else if (i > start3) {
          fprintf(stderr,"replacing frame3 on image %d   ",i);
          image_frame->mapFramePixels(image3, origCorners, currCorners);
        }
        //if (i%10 == 0) {
          fprintf(stderr,"Made it through %d\n",i);
        //}
        image_frame->Write(outname);
        delete image_frame;
      }
    }
    else {
      // Unrecognized program argument
      fprintf(stderr, "image: invalid option: %s\n", *argv);
      ShowUsage();
    }
  }

  // Write output image
  /*if (!image->Write(output_image_name)) {
    fprintf(stderr, "Unable to read image from %s\n", output_image_name);
    exit(-1);
  }*/

  // Delete image
  delete image;

  // Return success
  return EXIT_SUCCESS;
}



