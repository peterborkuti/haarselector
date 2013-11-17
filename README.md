haarselector
============

UI program based on OpenCV to select and label rectangles on images for Haar object detection

If you want to use OpenCV's Haar detection, you have to train Haar detector with appropriate images. Namely 
positive and negative ("background") images. Positive samples are images on which there are rectangular areas which are
labeled according to your needs.

For example, you want to train Haar detector for car's front view, you need to create two lists of samples. One consists of
pictures of car's front views, the others consists of negative samples, aka backgrounds.

The input list of haar-trainer is like this (from the OpenCV documentation)

img/img1.jpg  1  140 100 45 45
img/img2.jpg  2  100 200 50 50   50 30 25 25

The first number is the count of objects, the remaining numbers are for rectangles (x,y,width,height).

We also need a list of background images:

img/img1.jpg
img/img2.jpg

My program helps you with creating these two files. What you only need is 
1. to create many same-sized images which contain objects and backgrounds
2. use my program to select rectangles of objects on images and label them, or label the background images.

HaarSelector creates an output list, like this:

CAR_FRONT ./20131027-121836/00000134.png 1 434 213 167 123
CAR_FRONT ./20131027-121836/00000135.png 1 450 236 176 138
background ./20131027-121836/00000142.png
background ./20131027-121836/00000143.png
CAR_FRONT ./20131027-121836/00000144.png 1 425 249 65 52
CAR_FRONT ./20131027-121836/00000145.png 1 408 235 63 51
CAR_FRONT ./20131027-121836/00000146.png 1 440 249 53 42

You can filter this list later to ouput to different files the different types of lines
(here "background" and "CAR_FRONT")
