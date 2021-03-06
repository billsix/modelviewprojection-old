////Book licensed under Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)
////https://creativecommons.org/licenses/by-nc-sa/4.0/
////
////Generated graphics C++ code licensed under Apache 2.0 license

//= Model View Projection
//:author: Bill Six <billsix@gmail.com>
//:doctype: book
//:toc:
//
//[dedication]
//= Dedication
//
//To Teresa, Liam, Adam, and Kate.
//
//
//[preface]
//= Preface
//
//"I had no idea how much math was involved in computer graphics."
//
//Unfortunately many students of computer graphics have the impression
//that the writing of computer graphics programs requires knowledge of
//advanced math; which is patently untrue.
//Only the understanding of high-school level geometry is required.
//Using math you already know, this book builds both 2D and 3D
//applications from the ground up using OpenGL, a standard for graphics
//programming.
//
//Thoughout the book, I show how to place objects in space,
//how to draw objects relative to other objects, how to add a
//camera which moves over time based on user input, and how to transform all
//the objects into the 2D pixel coordinates
//of the computer screen.  By the end of this book, you will understand the basics of
//how to create first-person
//and third-person applications/games.  I made this book to show programmers
//how to make the kind
//of graphics programs which they want to make, using
//math they aleady know.
//
//This book is purposely limited in scope, and
//the applications produced are not particurly pretty nor realistic-looking.
//For advanced graphics topics, you'll need to consult other references,
//such as the OpenGL "red book" and "blue book".
//Although this book fills a huge gap that other books do not address,
//those other books are excellent reference books for advanced topics.
//
//
//
//
//[[intro]]
//= Introduction
//

//== Copyright
//
//(C) 2017, William Emerison Six. This book is free, licensed under Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0).
//https://creativecommons.org/licenses/by-nc-sa/4.0/.  The source code is licensed under the Apache 2.0 license.

//[[basics]]
//== Basics
//
//
//The device attached to a computer which displays information to the user is called a *monitor*.
//The monitor is composed of a two-dimensional array of light-emitting elements called *pixel*.
//At a given time, each individual pixel is instructed to display
//one specific color, represented within the computer as a number.
//The aggregate of the colors at each pixel at one moment in time, called a *frame*,
//provides a picture that has some meaning to the human user.
//
//
//
//In OpenGL, the bottom left pixel of a window is coordinate (0,0).  The top right is (window_width,window_height)
//
//.1024x768 monitor
//[caption="Figure 1: "]
//image:monitor.png[align="center",title="Foo"]
//
//.1920x1200 monitor
//image:monitor2.png[align="center",title="Foo"]
//
//
//Frames are created within the computer and sent to the monitor
//at a rate over time, called the *framerate*,
//measured in *Hertz*.  By updating frames quickly and at a constant rate, the computer
//provides the end-user with the illusion of motion.
//
//TODO - insert 3 20x20 frames which show motion
//
//
//
//[[openWindow]]
//=== Opening a Window
//
//Desktop operating systems allow the user to run more than one
//program at a time, where each program draws into a subsect of
//the monitor called a window.
//
//
//To create and to open a window in a cross-platform manner, this
//book will call procedures provided by the widely-ported GLFW library (supporting Windows, macOS, Linux).
//GLFW also provides procedures for receiving
//keyboard input, controller inputfootnote:[tested with a wired XBox 360 controller], and
//to load images from the filesystem.
//
//Much of the code listed from here until section<<the-event-loop>> will be of little interest upon first reading.
//As such, the reader may choose to skip ahead to section<<the-event-loop>> now.
//
//The code for the entire book is available at https://github.com/billsix/modelviewprojection,
//contained within "src/main.cpp". The code, but not the contents of the book, is licenced
//using the open-source Apache 2.0 license.
//
//==== Include Headers
//
//
//[source,C,linenums]
//----
/*  src/main.cpp
 *
 * Copyright 2016-2017 - William Emerison Six
 * All rights reserved
 * main.cpp is Distributed under Apache 2.0
 */
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include "main.h"
//----
//
//
//
//==== Create Data Structure to Represent the Window
//Create a pointer for the window.  If you are new to C or C++, don't fret
//over what a pointer is, as
//the majority of this book does not require knowledge of pointers.
//Programmers of any mainstream language (Java, Python, C#, etc)
//should be able to understand the content of this book.
//
//[source,C,linenums]
//----
GLFWwindow* window;
//----

//-Log any errors.

//[source,C,linenums]
//----
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
//----
//
//
//==== Define main
//
//-Set the error-handling callback
//[source,C,linenums]
//----
int main(int argc, char *argv[])
{
  glfwSetErrorCallback(error_callback);

//----
//==== Let the User Pick the Chapter Number to Run.
//[source,C,linenums]
//----
  std::cout << "Input Chapter Number to run: (2-17): " << std::endl;
  int chapter_number;
  std::cin >> chapter_number ;
//----
//==== GLFW/OpenGL Initialization
//
//-Initialize GLFW.
//[source,C,linenums]
//----
  //initialize video support, joystick support, etc.
  if (!glfwInit()){
    return -1;
  }
//----
//
//One frame is created incrementally over time on the CPU, but the frame
//is sent to the monitor
//only when frame is completely drawn, and each pixel has a color.
//The act of sending the frame to the monitor is called *flushing*
//the frame.
//Flushing takes time,
//and if the call to flush were to blockfootnote:[meaning it would not return control back to the call-ing procedure until the flush is complete], we would
//have wasted CPU time.  To avoid this,
//OpenGL has two *framebuffers*footnote:[regions of memory which will eventually contain the full data for a frame],
//only one of which is "active", or writable, at a given time.
//"glfwSwapBuffers" is a non-blocking call which initiates the flushing
//the current buffer, and which switches the current writable framebuffer to the
//other one, thus allowing the CPU to resume.
//
//-Set the version of OpenGL
//
//OpenGL has been around a long time, and has multiple, possibly incompatible versions.
//
//[source,C,linenums]
//----
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
//----
//Create a 500 pixel by 500 pixel window, which the user can resize.
//[source,C,linenums]
//----
  /* Create a windowed mode window and its OpenGL context */
  if(!(window = glfwCreateWindow(500,
                                 500,
                                 "modelviewprojection",
                                 NULL,
                                 NULL)))
    {
      glfwTerminate();
      return -1;
    }
//----
//A native application which links against shared libraries typically knows at compile-time
//exactly which procedures are provided by the shared libraries.
//But unlike typical shared libraries, a system's OpenGL shared library is supplied
//to the user by the graphics card vendor, who may not provide every OpenGL procedure from
//every version of OpenGL (of which there are many).
//To make programming in OpenGL easier, all calls to OpenGL are actually calls to "GLEW" procedures,
//which effectively are function pointers.  To ensure that those function pointers are initialized,
//call "glewInit".
//See <<sharedLibAppendix>> for a more full explanantion.
//[source,C,linenums]
//----
  glewInit(); // make OpenGL calls possible
  /* Make the window's context current */
  glfwMakeContextCurrent(window);
//----
//For every frame drawn, each pixel has a default color, set by
//calling "glClearColor". "0,0,0,1", means black "0,0,0", without
//transparency (the "1").
//[source,C,linenums]
//----
  glClearColor(/*red*/   0.0,
               /*green*/ 0.0,
               /*blue*/  0.0,
               /*alpha*/ 1.0);
//----
//Set the default depth for all fragments
//[source,C,linenums]
//----
  glClearDepth(-1.1f);
//----
//Set the depth test for all fragments.
//[source,C,linenums]
//----
  glDepthFunc(GL_GREATER);
//----
//Enable blending of new values in a fragment with the old value.
//[source,C,linenums]
//----
  glEnable(GL_BLEND);
//----
//Specify how a given fragment's color value within the framebuffer combines with a second color.  This new
//blended value is then set for the fragment.
//[source,C,linenums]
//----
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
//----
//Map the normalized device-coordinates to screen coordinates, explained later.
//[source,C,linenums]
//----
  {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(/*min_x*/ 0,
               /*min_y*/ 0,
               /*width_x*/ w,
               /*width_y*/ h);
  }
//----
//[[the-event-loop]]
//==== The Event Loop
//
//When you pause a movie, motion stops and you see one picture.
//Movies are composed of sequence of pictures, when
//rendered in quick succession, provide the illusion of motion.
//
//Interactive computer graphics are rendered the same way,
//one "frame" at a time.
//
//Render a frame for the user-selected demo, flush the complete frame to the monitor.
//Unless the user closed the window, repeat indefinitely.
//
//[source,C,linenums]
//----
  while (!glfwWindowShouldClose(window))
    {
      // set viewport
      int width = 0, height = 0;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0,
                 width, height);

      render_scene(&chapter_number);
      // flush the frame
      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
    }
//----
//==== The User Closed the App, Exit Cleanly.
//[source,C,linenums]
//----
  glfwTerminate();
  return 0;
} // end main
//----
//=== Render the Selected Demo
//
//Regardless of which demo will be run, certain things need
//to happen every frame.  The color of each pixel withith
//the current framebuffer
//is reset to a default color.
//
//[source,C,linenums]
//----
void render_scene(int *chapter_number){
  // clear the framebuffer
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
//----
//
//When a graphics application is executing, it is creating new
//frames (pictures) at some rate (e.g. 60 frames per second).  At any given
//frame, the user of the application might do something, (e.g.
//move the mouse, click, type on the keyboard, close the application).
//
//At the beginning of every frame, ask OpenGL if it received one
//of these events since we last asked (i.e., the previous frame).
//
//
//
//== Black Screen
//
//To understand the material of this book well, the reader is advised to
//run the compile the program and run the demos.  To do so, the source
//code for this book may be obtained at https://github.com/billsix/modelviewprojection.
//It has been tested on Windows 10 (Visual Studio Community 2017),
//Linux, and OS X.
//
//Once built, execute "modelviewprojection". When prompted, type "2" and then press the "Enter" key.
//
//The first demo is the least interesting graphical program possible.
//
//The event loop, defined in section <<the-event-loop>>, executes a few procedures
//before calling the current procedure.

//-Sets the color at every pixel black.  (A constant color is better than whatever
//color happened to be the previous time it was drawn.)
//
//-If the user resized the window, reset OpenGL's mappings from *normalized-device-coordinates*
//to *screen-coordinates*.
//
//-Cleared the depth buffer (don't worry about this for now).
//
//
//
//
//
//[source,C,linenums]
//----
  if(2 == *chapter_number){
    return;
  }
//----

//When this code returns, the event loop flushes (i.e) sends the frame to the monitor.  Since
//no geometry was drawn, the color value for each pixel is still black.
//
//Each color is represende by a number, so the frame is something like this:
//
//
//....
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//....
//
//
//

//The event loop then calls this code over and over again, and since we retain no state and
//we draw nothing, a black screen will be displayed every frame until the user
//closes the window, and says to himself, "why did I buy Doom 3"?



//== Draw Paddles
//
//
//A black screen is not particularly interesting, so
//let's draw something, say, two rectangles.
//Where should they be, and what color should they be?

//"glColor3f" sets a global variable, which makes it the color to be used
//for the subsequently-drawn graphical shape.  The background will be black,
//so lets make the first paddle white, and a second paddle yellow.
//
//"glBegin(GL_QUADS)" tells OpenGL that we will soon specify 4 *vertices*,
//(i.e. points) which define the
//quadrilateral.  The vertices will be specified by calling "glVertex2f" 4 times.
//
//[source,C,linenums]
//----
  if(3 == *chapter_number){
  chapter3:
//----
//Draw paddle 1.
//[source,C,linenums]
//----
    // the color white has 1.0 for r,g,and b components.
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      glVertex2f(/*x*/ -1.0,
                 /*y*/ -0.3);
      glVertex2f(/*x*/ -0.8,
                 /*y*/ -0.3);
      glVertex2f(/*x*/ -0.8,
                 /*y*/ 0.3);
      glVertex2f(/*x*/ -1.0,
                 /*y*/ 0.3);
    }
    glEnd();
//----
//"glEnd()" tells OpenGL that we have finished providing vertices for
//the begun quadrilateral.
//

//The framebuffer, which has not yet been flushed to the monitor, has geometry which looks like this:

//image:plot1.png[align="center",title="Foo"]


//Draw paddle 2.
//[source,C,linenums]
//----
    // the color yellow has 1.0 for r and g components,
    // with 0.0 for b.
    // Why is that?  The author doesn't know, consult the internet
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  0.0);
    glBegin(GL_QUADS);
    {
      glVertex2f(/*x*/ 0.8,
                 /*y*/ -0.3);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ -0.3);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ 0.3);
      glVertex2f(/*x*/ 0.8,
                 /*y*/ 0.3);
    }
    glEnd();
//----

//The framebuffer, which has not yet been flushed to the monitor, has geometry which looks like this:

//image:plot2.png[align="center",title="Foo"]

//[source,C,linenums]
//----
    return;
  }
//----
//The frame sent to the monitor is a set of values like this:
//....
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbyyyyy
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbyyyyy
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbyyyyy
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbyyyyy
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbyyyyy
//wwwwwbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
//....

//What do we have to do to convert from normalized-device-coordinates
//into individual colors for each pixel?  Nothing, OpenGL does that for us; therefore
//we never have to think in terms of pixels, only in terms of vertices of shapes,
//specified by normalized-device-coordinates.
//
//Why is that desirable?
//
//=== Normalized-Device-Coordinates
//
//The author owns two monitors, one which has 1024x768 pixels, and one which has
//1920x1200 pixels.  When he purchases a game from Steam, he expects that his game
//will run correctly on either monitor, in full-screen mode.  If a graphics programmer
//had to explictly set each indiviual pixel's color, the the programmer would have to
//program using "screen-space"footnote:[Any "space" means a system of numbers which you're using.
//Screen-space means you're specifically using pixel coordinates, i.e, set pixel (5,10) to be red].

//What looks alright is screen-space on a large monitor...

//image:screenspace2.png[align="center",title="Programming using Screen Space on Large Monitor"]

//isn't even the same picture on a smaller monitor.

//image:screenspace.png[align="center",title="Programming using Screen Space on Small Monitor"]



//Like any good program or library, OpenGL abstracts over screen-space, thus freeing the
//programmer from caring about screen size.  If a programmer does not want to program
//in discretefootnote:[discrete means integer values, not continuous] screen-space,
//what type of numbers should he use?  Firstly, it should be a continuous space, meaning
//that it should be in decimal numbers.  Because if a real-world object is 10.3 meters long, a programmer
//should be able to enter "float foo = 10.3".  Secondly, it should be a fixed range vertically
//and an fixed range horizontally.  OpenGL will have to convert points from some space to screen-space,
//and since OpenGL does this in hardware (i.e. you can't programmatically change how the conversion
//happens), it should be a fixed size.

//OpenGL uses *normalized-device-coordinates* footnote:[normalized- meaning a distance value of
//1; device- the monitor; coordinates- the system of numbers (i.e. space) in which you are working],
//which is a continous space from -1.0 to 1.0 horizontally,
//and -1.0 to 1.0 vertically.


//By specifying geometry using normalized-device-coordinates,
//OpenGL will automatically convert from a continuous, -1.0 to 1.0 space,
//to discrete pixel-space.

//image:ndcSpace.png[align="center",title="Programming using Screen Space on Large Monitor"]

//Whether we own a small monitor

//image:ndcSpace1.png[align="center",title="Programming using Screen Space on Small Monitor"]

//or a large monitor.

//image:screenspace2.png[align="center",title="Programming using Screen Space on Large Monitor"]


//-Exercise 1.  The window is resizable by the user while it runs.
//Do the paddles both  still appear in the window if you make it really thin?  What if
//you make it very wide?


//Answer - (Regardless of the window's width to height ratio, the pixel in the upper left of
//the window still maps to normalized-device-coordinate (-1.0,-1.0), and the pixel
//in the bottom right of the window still maps to (1.0,1.0).

//-Exercise 2.  How would you convert from ndc-space to screen-space, given
//a monitor width _w_ and height _h_?

//== Keeping the Paddles Proportional
//
//=== Create procedure to ensure proportionality
//In the previous chapter, if the user resized the window, the paddles looked bad,
//as they were shrunk in one direction if the window became too thin or too fat.


//image:disproportionate1.png[align="center",title="Foo"]


//image:disproportionate2.png[align="center",title="Foo"]


//Assume that this is a problem for the application we are making.  What
//would a solution be?  Ideally, we would like to draw our paddles with
//a black background within a square region in the center of the window, regardless of the dimensions
//of the window.

//OpenGL has a solution for us.  The *viewport* is a rectangular region
//within the window into which OpenGL will render.  The normalized-device-coordinates
//will therefore resolve to the sub-screen space of the viewport, instead of the whole
//window.
//

//image:viewport.png[align="center",title="Programming using Screen Space on Large Monitor"]

//Because we will only draw in a subset of the window, and because all subsequent
//chapters will use this functionality, I have created a procedure for use
//in all chapters. "draw_in_square_viewport" is a C++ lambda, which just
//means that it's a procedure defined at runtime.  Don't worry about the details
//of lambdas, just know that the following two types are the same:

//----
  //void draw_in_square_viewport();
  //std::function<void()> draw_in_square_viewport;
//----
//
//The pattern is
//----
  //RETURN_TYPE function_name(ARG_LIST);
  //std::function<RETURN_TYPE(ARGLIST)> functionName;
//----

//
//[source,C,linenums]
//----
  std::function<void()> draw_in_square_viewport = [&](){
    // clear all of the background to grey
    glClearColor(/*red*/   0.2,
                 /*green*/ 0.2,
                 /*blue*/  0.2,
                 /*alpha*/ 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // resize drawing area
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    int min = w < h ? w : h;
    glViewport(/*min_x*/ 0 + (w - min)/2,
               /*min_y*/ 0 + (h - min)/2,
               /*width_x*/ min,
               /*width_y*/ min);

    glEnable(GL_SCISSOR_TEST);
    glScissor(/*min_x*/ 0 + (w - min)/2,
              /*min_y*/ 0 + (h - min)/2,
              /*width_x*/ min,
              /*width_y*/ min);

    glClearColor(/*red*/   0.0,
                 /*green*/ 0.0,
                 /*blue*/  0.0,
                 /*alpha*/ 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
  };
//----

//=== Set the viewport, and then execute the code from chapter 3.

//[source,C,linenums]
//----
  if(4 == *chapter_number){
    draw_in_square_viewport();
    goto chapter3;
  }
//----
//
//Yes, the author is aware that "goto" statements are frowned upon.
//But would the reader prefer for chapter 3's code to be duplicated here?

//
//== Move the Paddles using the Keyboard
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|*w*              |*Move Left Paddle Up*
//|*s*              |*Move Left Paddle Down*
//|*k*              |*Move Right Paddle Up*
//|*i*              |*Move Right Paddle Down*
//|=======================================
//Paddles which don't move are quite boring.  Let's make them move up or down
//by getting keyboard input.
//
//
//Static variables are initialized to a value only the first time the procedure is executed.
//In subsequent calls to "render_scene", they retain the value they had the last time
//"render_scene" was calledfootnote:[Since the rest of the demos are entirely defined
//within "render_scene", all statically defined variables, such as these offsets, are
//available to every demo, and as such, future demos will reference these values].
//[source,C,linenums]
//----
  static GLfloat paddle_1_offset_Y = 0.0;
  static GLfloat paddle_2_offset_Y = 0.0;
//----
//-If 's' is pressed this frame, subtract 0.1 more from paddle_1_offset_Y.  If the
//key continues to be held down over time, paddle_1_offset_Y will continue to decrease.

//-If 'w' is pressed this frame, add 0.1 more to paddle_1_offset_Y.

//-If 'k' is pressed this frame, subtract 0.1 more from paddle_2_offset_Y.

//-If 'i' is pressed this frame, add 0.1 more to paddle_2_offset_Y.

//Remember, these are static variables, so changes to these variables will
//accumulate across frames.
//
//[source,C,linenums]
//----
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    paddle_1_offset_Y -= 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    paddle_1_offset_Y += 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
    paddle_2_offset_Y -= 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
    paddle_2_offset_Y += 0.1;
  }
//----
//
//
//[source,C,linenums]
//----
  if(5 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
//----
//Add paddle_1_offset_Y to the "y" component of every vertex
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glVertex2f(/*x*/ -1.0,
                 /*y*/ -0.3+paddle_1_offset_Y);
      glVertex2f(/*x*/ -0.8,
                 /*y*/ -0.3+paddle_1_offset_Y);
      glVertex2f(/*x*/ -0.8,
                 /*y*/ 0.3+paddle_1_offset_Y);
      glVertex2f(/*x*/ -1.0,
                 /*y*/ 0.3+paddle_1_offset_Y);
    }
    glEnd();
//----

//image:plot3.png[align="center",title="Foo"]


//Draw paddle 2, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  0.0);
//----
//Add paddle_2_offset_Y to the "y" component of every vertex
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glVertex2f(/*x*/ 0.8,
                 /*y*/ -0.3+paddle_2_offset_Y);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ -0.3+paddle_2_offset_Y);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ 0.3+paddle_2_offset_Y);
      glVertex2f(/*x*/ 0.8,
                 /*y*/ 0.3+paddle_2_offset_Y);
    }
    glEnd();
    return;
  }
//----

//image:plot4.png[align="center",title="Foo"]


//== Model Vertices with a Data-Structure
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|=======================================
//
//Transforming vertices, such as translating, is the core concept
//of computer graphics.  So create a class for common transformations.

//[source,C,linenums]
//----
  class Vertex {
  public:
    // members
    GLfloat x;
    GLfloat y;
    // construtor
    Vertex(GLfloat the_x, GLfloat the_y):
      x(the_x),
      y(the_y)
    {}
//----
//
//=== Translation
//Rather than incrementing y values before calling "glVertex",
//instead call "translate" on the vertex.

//image:translate.png[align="center",title="Foo"]


//[source,C,linenums]
//----
    Vertex translate(GLfloat translate_x,
                     GLfloat translate_y)
    {
      return Vertex(/*x*/ x + translate_x,
                    /*y*/ y + translate_y);
    };
//----


//=== Scaling

//image:scale.png[align="center",title="Foo"]

//
//Similarly, we can expand or shrink the size of an object
//by "scale"ing each of the vertices of the object, assuming
//the object's center is at (0,0).
//[source,C,linenums]
//----
    Vertex scale(GLfloat scale_x,
                 GLfloat scale_y)
    {
      return Vertex(/*x*/ x * scale_x,
                    /*y*/ y * scale_y);
    };
//----


//=== Rotation Around Origin (0,0)
//
//We can also rotate an object around (0,0) by rotating
//all of the object's vertices around (0,0).  Although defined now,
//this won't
//be used until later.

//In high school math, you will have learned about sin, cos, and tangent.
//Typically the angles are described on the unit circle, where a rotation
//starts from the positive x axis.  We can expand on this knowledge, allowing
//us to rotate a given vertex around the origin (0,0).  This is done
//by separating the x and y value, rotating each of them seperately,
//and then adding the results together.

//That might not have been fully clear.  Let me try again.
//The vertex (0.5,0.4) can be separated into two vertices, (0.5,0) and (0,0.4).

//image:rotate3.png[align="center",title="Foo"]

//image:rotate4.png[align="center",title="Foo"]


//These vertices can be added together to create the original vertex.
//But, before we do that, let's rotate each of the vertices.
//
//(0.5,0) is on the x-axis, so rotating it by "angle" degrees, results
//in vertex (0.5*cos(angle), 0.5*sin(angle)).  Notice that both the x and
//y values are multiplied by 0.5.  This is because rotations should not affect
//the distance of the point from the origin (0,0).  (0.5,0) has length 0.5.
//(cos(angle), sin(angle) has length 1. By multipling both the x and y
//component by 0.5, we are scaling the vertex back to its original distance
//from the origin.

//image:rotate.png[align="center",title="Foo"]

//(0,0.4) is on the y-axis, so rotating it by "angle" degrees, results
//in vertex (0.4*-sin(angle), 0.4*cos(angle)).

//image:rotate2.png[align="center",title="Foo"]

//Wait.  Why is negative
//sin applied to the angle to make the x value, and cos applied to angle to make the y value?
//Trigonometric operations such as sin, cos, and tangent assume that the rotation is happening on
//the unit circle, starting from (1,0) on the x axis.  Since we want
//to rotate an angle starting from (0,1) on the y axis, we sin and
//cos must be swapped.  Sin is positive from 0 to 90 degrees, but
//we want a negative value for our rotation of the y axis since the rotation is happening counter-clockwise,
//hence the negative sin.
//


//After the rotations have been applied, sum the results to
//get your vertex rotated around the origin!

//(0.5*cos(angle), 0.5*sin(angle)) + (0.4*-sin(angle), 0.4*cos(angle)) =
//(0.5*cos(angle) + 0.4*-sin(angle), 0.5*sin(angle) + 0.4*cos(angle))

//[source,C,linenums]
//----
    Vertex rotate(GLfloat angle_in_radians)
    {
      return Vertex(/*x*/ x*cos(angle_in_radians) - y*sin(angle_in_radians),
                    /*y*/ x*sin(angle_in_radians) + y*cos(angle_in_radians));
    };
//----

//=== Rotation Around Arbitrary Vertex
//But what if we don't want to rotate around the origin?  What if you want to
//rotate around any other vertex?

//It's quite simple.  Translate the vertex to be rotated into a vertex which is
//relative to the rotation point. Rotate it around the origin. Then reverse the
//translation.footnote:[To the advanced reader, yes, this is unwise to do in practice.
//Real world development is Mistake-Driven-Development.  Presenting solutions
//without making mistakes deprives the learner.].

//[source,C,linenums]
//----
    Vertex rotate(GLfloat angle_in_radians,
                  Vertex center)
    {
      return translate(/*x*/ -center.x,
                       /*y*/ -center.y).
        rotate(angle_in_radians).
        translate(/*x*/ center.x,
                  /*y*/ center.y);
    };
  };
//----
//[source,C,linenums]
//----
  if(6 == *chapter_number){
    draw_in_square_viewport();
    std::vector<Vertex> paddle = {
      Vertex(-0.1, -0.3),
      Vertex(0.1, -0.3),
      Vertex(0.1, 0.3),
      Vertex(-0.1, 0.3)
    };
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex v : paddle){
        Vertex newPosition = v.translate(/*x*/ -0.9,
                                         /*y*/ paddle_1_offset_Y);
        glVertex2f(/*x*/ newPosition.x,
                   /*y*/ newPosition.y);
      }
    }
    glEnd();
//----
//Draw paddle 2, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  0.0);
    glBegin(GL_QUADS);
    {
      for(Vertex v : paddle){
        Vertex newPosition = v.translate(/*x*/ 0.9,
                                         /*y*/ paddle_2_offset_Y);
        glVertex2f(/*x*/ newPosition.x,
                   /*y*/ newPosition.y);
      }
    }
    glEnd();
    return;
  }
//----
//== Model-space
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|=======================================

//Normalized-device-coordinates are not a natural system of
//numbers for use by humans.  Imagine that the paddles in the previous
//chapters exist in real life, and are 20 meters wide and 60 meters tall.
//The graphics programmer should be able to use those numbers directly;
//they shouldn't have to manually trasform the distances into normalized-device-coordinates.
//
//Whatever a convenient numbering system is (i.e. coordinate system) for modeling objects
//is called "model-space".  Since a paddle has four corners, which corner should be a
//the origin (0,0)?  If you don't already know what you want at the origin, then
//none of the corners should be; instead put the center of the object
//at the originfootnote:[By putting the center of the object at the origin,
//scaling and rotating the object are trivial].

//[source,C,linenums]
//----
  const std::vector<Vertex> paddle = {
    Vertex(-10.0, -30.0),
    Vertex(10.0, -30.0),
    Vertex(10.0, 30.0),
    Vertex(-10.0, 30.0)
  };
//----


//image:modelspace.png[align="center",title="Foo"]

//[source,C,linenums]
//----
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    paddle_1_offset_Y -= 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    paddle_1_offset_Y += 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
    paddle_2_offset_Y -= 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
    paddle_2_offset_Y += 10.0;
  }
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
  if(7 == *chapter_number){
    draw_in_square_viewport();
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace.translate(/*x*/ -90.0,
                                                 /*y*/ paddle_1_offset_Y);
        Vertex ndcSpace = worldSpace.scale(/*x*/ 1.0/100.0,
                                           /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
//----
//Draw paddle 2, relative to the world-space origin
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glColor3f(/*red*/   1.0,
                /*green*/ 1.0,
                /*blue*/  0.0);
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace.translate(/*x*/ 90.0,
                                                 /*y*/ paddle_2_offset_Y);
        Vertex ndcSpace = worldSpace.scale(/*x*/ 1.0/100.0,
                                           /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
    return;
  }
//----
//== Rotate the Paddles About their Center
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|               |
//|*d*              |*Increase Left Paddle's Rotation*
//|*a*              |*Decrease Left Paddle's Rotation*
//|*l*              |*Increase Right Paddle's Rotation*
//|*j*              |*Decrease Right Paddle's Rotation*
//|=======================================

//// TODO - describe angle as radians
//// TODO - show unit circle
//// TODO - show orthonormal basis
//// TODO - show basic proof of rotate


//[source,C,linenums]
//----
  static GLfloat paddle_1_rotation = 0.0;
  static GLfloat paddle_2_rotation = 0.0;
  // update_rotation_of_paddles
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    paddle_1_rotation += 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    paddle_1_rotation -= 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
    paddle_2_rotation += 0.1;
  }
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
    paddle_2_rotation -= 0.1;
  }
//----
//[source,C,linenums]
//----
  if(8 == *chapter_number){
    draw_in_square_viewport();
//----
//// TODO - discuss method chaining
//// TODO - gnuplot the modelspace, rotated, and translated
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y)
          .rotate(/*radians*/ paddle_1_rotation,
                  Vertex(/*x*/ -90.0,
                         /*y*/ paddle_1_offset_Y));
        Vertex ndcSpace = worldSpace.scale(/*x*/ 1.0/100.0,
                                           /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
//----

//// TODO -- explain why the translate first, and then the rotate, is poor form. Besides inefficiency, this system doesn't compose, as a vertex subject to multiple rotations may not know where its modelspace origin is, and as such, cannot translate to the origin.
//
//Draw paddle 2, relative to the world-space origin
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glColor3f(/*red*/   1.0,
                /*green*/ 1.0,
                /*blue*/  0.0);
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .rotate(/*radians*/ paddle_2_rotation)
          .translate(/*x*/ 90.0,
                     /*y*/ paddle_2_offset_Y);
        Vertex ndcSpace = worldSpace.scale(/*x*/ 1.0/100.0,
                                           /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
    return;
  }
//----

//== Camera Management
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|               |
//|d              |Increase Left Paddle's Rotation
//|a              |Decrease Left Paddle's Rotation
//|l              |Increase Right Paddle's Rotation
//|j              |Decrease Right Paddle's Rotation
//|               |
//|*UP*             |*Move the Camera Up*
//|*DOWN*           |*Move the Camera Down*
//|*LEFT*           |*Move the Camera Left*
//|*RIGHT*          |*Move the Camera Right*
//|=======================================
//[source,C,linenums]
//// TODO - describe implicit camera at origin, and making it's location explicit
//// TODO - descriibe desire for moving camera
//----
  static GLfloat camera_x = 0.0;
  static GLfloat camera_y = 0.0;
  // update_camera_position
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
    camera_y += 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
    camera_y -= 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
    camera_x -= 10.0;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
    camera_x += 10.0;
  }
//----
//[source,C,linenums]
//----
  if(9 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .rotate(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
//----
//Draw paddle 2, relative to the world-space origin
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glColor3f(/*red*/   1.0,
                /*green*/ 1.0,
                /*blue*/  0.0);
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .rotate(/*radians*/ paddle_2_rotation)
          .translate(/*x*/ 90.0,
                     /*y*/ paddle_2_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
    return;
  }
//----
//== Relative Objects
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|               |
//|d              |Increase Left Paddle's Rotation
//|a              |Decrease Left Paddle's Rotation
//|l              |Increase Right Paddle's Rotation
//|j              |Decrease Right Paddle's Rotation
//|               |
//|UP             |Move the Camera Up
//|DOWN           |Move the Camera Down
//|LEFT           |Move the Camera Left
//|RIGHT          |Move the Camera Right
//|=======================================
//
//[source,C,linenums]
//----
  const std::vector<Vertex> square = {
    Vertex(-5.0, -5.0),
    Vertex(5.0, -5.0),
    Vertex(5.0, 5.0),
    Vertex(-5.0, 5.0)
  };
  std::function<void()> draw_paddle_1 = [&](){
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .rotate(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
  };
  std::function<void()> draw_paddle_2 = [&](){
    glBegin(GL_QUADS);
    {
      glColor3f(/*red*/   1.0,
                /*green*/ 1.0,
                /*blue*/  0.0);
      for(Vertex modelspace : paddle){
        Vertex worldSpace = modelspace
          .rotate(/*radians*/ paddle_2_rotation)
          .translate(/*x*/ 90.0,
                     /*y*/ paddle_2_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
  };
//----
//
//[source,C,linenums]
//----
  if(10 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1.
//[source,C,linenums]
//----
    draw_paddle_1();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : square){
        Vertex worldSpace = modelspace
          .translate(/*x*/ 20.0f,
                     /*y*/ 0.0f)
          .rotate(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
    }
    glEnd();
//----
//Draw paddle 2.
//[source,C,linenums]
//----
    draw_paddle_2();
    return;
  }
//----
//== Rotate the Square About Its Origin
//
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|               |
//|d              |Increase Left Paddle's Rotation
//|a              |Decrease Left Paddle's Rotation
//|l              |Increase Right Paddle's Rotation
//|j              |Decrease Right Paddle's Rotation
//|               |
//|UP             |Move the Camera Up
//|DOWN           |Move the Camera Down
//|LEFT           |Move the Camera Left
//|RIGHT          |Move the Camera Right
//|               |
//|*q*              |*Rotate the square around its center.*
//|=======================================
//[source,C,linenums]
//----
  static GLfloat square_rotation = 0.0;
  // update_square_rotation
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
    square_rotation += 0.1;
  }
  if(11 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1.
//[source,C,linenums]
//----
    draw_paddle_1();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : square){
        Vertex worldSpace  = modelspace
          .rotate(/*radians*/ square_rotation)
          .translate(/*x*/ 20.0f,
                     /*y*/ 0.0f)
          .rotate(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
      glEnd();
    }
//----
//Draw paddle 2.
//[source,C,linenums]
//----
    draw_paddle_2();
    return;
  }
//----
//== Relative Rotation
//[width="75%",options="header,footer"]
//|=======================================
//|Keyboard Input |Action
//|w              |Move Left Paddle Up
//|s              |Move Left Paddle Down
//|k              |Move Right Paddle Up
//|i              |Move Right Paddle Down
//|               |
//|d              |Increase Left Paddle's Rotation
//|a              |Decrease Left Paddle's Rotation
//|l              |Increase Right Paddle's Rotation
//|j              |Decrease Right Paddle's Rotation
//|               |
//|UP             |Move the Camera Up
//|DOWN           |Move the Camera Down
//|LEFT           |Move the Camera Left
//|RIGHT          |Move the Camera Right
//|               |
//|q              |Rotate the square around its center
//|*e*              |*Rotate the square around the left paddle*
//|=======================================
//
//[source,C,linenums]
//----
  static GLfloat rotation_around_paddle_1 = 0.0;
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
    rotation_around_paddle_1 += 0.1;
  }
//----
//[source,C,linenums]
//----
  if(12 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1.
//[source,C,linenums]
//----
    draw_paddle_1();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex modelspace : square){
        Vertex worldSpace  = modelspace
          .rotate(/*radians*/ square_rotation)
          .translate(/*x*/ 20.0f,
                     /*y*/ 0.0f)
          .rotate(/*radians*/ rotation_around_paddle_1)
          .rotate(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y);
        Vertex cameraSpace = worldSpace.translate(/*x*/ -camera_x,
                                                  /*y*/ -camera_y);
        Vertex ndcSpace = cameraSpace.scale(/*x*/ 1.0/100.0,
                                            /*y*/ 1.0/100.0);
        glVertex2f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y);
      }
      glEnd();
    }
//----
//Draw paddle 2.
//[source,C,linenums]
//----
    draw_paddle_2();
    return;
  }
//----
//
//== Adding Depth
////TODO - discuss what the z component is, show graphs.
////TODO - show X, Y, and Z rotations graphically with gnuplot.
////TODO - make appendix for rotation around arbitrary axis
//[source,C,linenums]
//----
  class Vertex3 {
  public:
    Vertex3(GLfloat the_x, GLfloat the_y, GLfloat the_z):
      x(the_x),
      y(the_y),
      z(the_z)
    {}
    Vertex3 translate(GLfloat translate_x,
                      GLfloat translate_y,
                      GLfloat translate_z)
    {
      return Vertex3(x + translate_x,
                     y + translate_y,
		     z + translate_z);
    };
    Vertex3 rotateX(GLfloat angle_in_radians)
    {
      return Vertex3(x,
                     y*cos(angle_in_radians) - z*sin(angle_in_radians),
		     y*sin(angle_in_radians) + z*cos(angle_in_radians));
    };
    Vertex3 rotateY(GLfloat angle_in_radians)
    {
      return Vertex3(z*sin(angle_in_radians) + x*cos(angle_in_radians),
                     y,
		     z*cos(angle_in_radians) - x*sin(angle_in_radians));
    };
    Vertex3 rotateZ(GLfloat angle_in_radians)
    {
      return Vertex3(x*cos(angle_in_radians) - y*sin(angle_in_radians),
                     x*sin(angle_in_radians) + y*cos(angle_in_radians),
                     z);
    };
    Vertex3 scale(GLfloat scale_x,
                  GLfloat scale_y,
                  GLfloat scale_z)
    {
      return Vertex3(x * scale_x,
                     y * scale_y,
                     z * scale_z);
    };
////TODO - explain that ortho will be decribed later
    Vertex3 ortho(GLfloat min_x,
                  GLfloat max_x,
                  GLfloat min_y,
                  GLfloat max_y,
                  GLfloat min_z,
                  GLfloat max_z)
    {
      GLfloat x_length = max_x-min_x;
      GLfloat y_length = max_y-min_y;
      GLfloat z_length = max_z-min_z;
      return
	translate(-(max_x-x_length/2.0),
		  -(max_y-y_length/2.0),
		  -(max_z-z_length/2.0))
        .scale(/*x*/ 1/(x_length/2.0),
               /*y*/ 1/(y_length/2.0),
               /*z*/ 1/(-z_length/2.0));
      // negate z length because it is already negative, and don't want
      // to flip the data
    }

#define RAD_TO_DEG(rad) (57.296 * rad)
#define DEG_TO_RAD(degree) (degree / 57.296)
////TODO -  explain that perspective will be explained later
    Vertex3 perspective(GLfloat nearZ,
                        GLfloat farZ){
      const GLfloat field_of_view =  DEG_TO_RAD(45.0/2.0);
      int w, h;
      glfwGetFramebufferSize(window, &w, &h);
      GLfloat y_angle =  (w / h) * field_of_view;

      GLfloat sheared_x = x / fabs(z) * fabs(nearZ);
      GLfloat sheared_y = y / fabs(z) * fabs(nearZ);
      Vertex3 projected =  Vertex3(/*x*/ sheared_x,
				   /*y*/ sheared_y,
				   /*z*/ z);
      GLfloat x_min_of_box = fabs(nearZ) * tan(field_of_view);
      GLfloat y_min_of_box = fabs(nearZ) * tan(y_angle);
      return projected.ortho(/*min_x*/ -x_min_of_box,
			     /*max_x*/ x_min_of_box,
                             /*min_y*/ -y_min_of_box,
			     /*max_y*/ y_min_of_box,
                             /*min_z*/ nearZ,
			     /*max_z*/ farZ);
    };
    GLfloat x;
    GLfloat y;
    GLfloat z;
  };

////TODO -  explain that we are externalizeing the aggregate transformation into a procedure

  typedef std::function<Vertex3 (Vertex3)> Vertex3_transformer;
  std::function<void (Vertex3_transformer)>
    draw_square3_programmable =
    [&](Vertex3_transformer f)
    {
      glBegin(GL_QUADS);
      {
        Vertex3 ndc_v_1 = f(Vertex3(/*x*/ -1.0,
                                    /*y*/ -1.0,
                                    /*z*/ 0.0));
        glVertex3f(/*x*/ ndc_v_1.x,
                   /*y*/ ndc_v_1.y,
                   /*z*/ ndc_v_1.z);
        Vertex3 ndc_v_2 = f(Vertex3(/*x*/ 1.0,
                                    /*y*/ -1.0,
                                    /*z*/ 0.0));
        glVertex3f(/*x*/ ndc_v_2.x,
                   /*y*/ ndc_v_2.y,
                   /*z*/ ndc_v_2.z);
        Vertex3 ndc_v_3 = f(Vertex3(/*x*/ 1.0,
                                    /*y*/ 1.0,
                                    /*z*/ 0.0));
        glVertex3f(/*x*/ ndc_v_3.x,
                   /*y*/ ndc_v_3.y,
                   /*z*/ ndc_v_3.z);
        Vertex3 ndc_v_4 = f(Vertex3(/*x*/ -1.0,
                                    /*y*/ 1.0,
                                    /*z*/ 0.0));
        glVertex3f(/*x*/ ndc_v_4.x,
                   /*y*/ ndc_v_4.y,
                   /*z*/ ndc_v_4.z);
      }
      glEnd();
    };
//----
//[source,C,linenums]
//----
  const std::vector<Vertex3> paddle3D = {
    Vertex3(/*x*/ -10.0,
            /*y*/ -30.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ 10.0,
            /*y*/ -30.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ 10.0,
            /*y*/ 30.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ -10.0,
            /*y*/ 30.0,
            /*z*/ 0.0)
  };
  const std::vector<Vertex3> square3D = {
    Vertex3(/*x*/ -5.0,
            /*y*/ -5.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ 5.0,
            /*y*/ -5.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ 5.0,
            /*y*/ 5.0,
            /*z*/ 0.0),
    Vertex3(/*x*/ -5.0,
            /*y*/ 5.0,
            /*z*/ 0.0)
  };


//// TODO -- update newposition to have better names for 3d
  if(13 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1.
//[source,C,linenums]
//----
//// TODO -- draw_paddle_1 is still using only 2D, explain implicit 3D of z have 0 for a value
    draw_paddle_1();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex3 modelspace : square3D){
        Vertex3 worldSpace = modelspace
          .rotateZ(/*radians*/ square_rotation)
          .translate(/*x*/ 20.0f,
                     /*y*/ 0.0f,
                     /*z*/ -10.0f)  // NEW, using a different Z value
          .rotateZ(/*radians*/ rotation_around_paddle_1)
          .rotateZ(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y,
                     /*z*/ 0.0);
        Vertex3 cameraSpace = worldSpace
          .translate(/*x*/ -camera_x,
                     /*y*/ -camera_y,
                     /*z*/ 0.0);
////TODO -  explain ortho
        Vertex3 ndcSpace = cameraSpace
          .ortho(/*min_x*/ -100.0f,
                 /*max_x*/ 100.0f,
                 /*min_y*/ -100.0f,
                 /*max_y*/ 100.0f,
                 /*min_z*/ 100.0f,
                 /*max_z*/ -100.0f);
        glVertex3f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y,
                   /*z*/ ndcSpace.y);
      }
      glEnd();
    }
//----
//Draw paddle 2.
//[source,C,linenums]
//----
    draw_paddle_2();
    return;
  }
//----
//
//== Moving the Camera in 3D
//[source,C,linenums]
//----
  static GLfloat moving_camera_x = 0.0;
  static GLfloat moving_camera_y = 0.0;
  static GLfloat moving_camera_z = 0.0;
  static GLfloat moving_camera_rot_y = 0.0;
  static GLfloat moving_camera_rot_x = 0.0;
  // update camera from the keyboard
  {
    const GLfloat move_multiple = 15.0;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
      moving_camera_rot_y -= 0.03;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
      moving_camera_rot_y += 0.03;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
      moving_camera_rot_x += 0.03;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS){
      moving_camera_rot_x -= 0.03;
    }
////TODO -  explaing movement on XZ-plane
////TODO -  show camera movement in graphviz
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
      moving_camera_x -= move_multiple * sin(moving_camera_rot_y);
      moving_camera_z -= move_multiple * cos(moving_camera_rot_y);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
      moving_camera_x += move_multiple * sin(moving_camera_rot_y);
      moving_camera_z += move_multiple * cos(moving_camera_rot_y);
    }
  }
//----
//[source,C,linenums]
//----
  if(14 == *chapter_number){
    draw_in_square_viewport();
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex3 modelspace : paddle3D){
        Vertex3 worldSpace = modelspace
          .rotateZ(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y,
                     /*z*/ 0.0);
        // new camera transformations
        Vertex3 cameraSpace = worldSpace
          .translate(/*x*/ -moving_camera_x,      // NEW
                     /*y*/ -moving_camera_y,      // NEW
                     /*z*/ -moving_camera_z)      // NEW
          .rotateY(/*radians*/ -moving_camera_rot_y)    // NEW
          .rotateX(/*radians*/ -moving_camera_rot_x);   // NEW
        // end new camera transformations
////TODO -  discuss order of rotations, use moving head analogy to show that rotations are not commutative
        Vertex3 ndcSpace = cameraSpace
          .ortho(/*min_x*/ -100.0f,
                 /*max_x*/ 100.0f,
                 /*min_y*/ -100.0f,
                 /*max_y*/ 100.0f,
                 /*min_z*/ 100.0f,
                 /*max_z*/ -100.0f);
        glVertex3f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y,
                   /*z*/ ndcSpace.z);
      }
    }
    glEnd();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glBegin(GL_QUADS);
    {
      for(Vertex3 modelspace : square3D){
        Vertex3 worldSpace = modelspace
          .rotateZ(/*radians*/ square_rotation)
          .translate(/*x*/ 20.0f,
                     /*y*/ 0.0f,
                     /*z*/ -10.0f)  // NEW, using a different Z value
          .rotateZ(/*radians*/ rotation_around_paddle_1)
          .rotateZ(/*radians*/ paddle_1_rotation)
          .translate(/*x*/ -90.0,
                     /*y*/ paddle_1_offset_Y,
                     /*z*/ 0.0);
        // new camera transformations
        Vertex3 cameraSpace = worldSpace
          .translate(/*x*/ -moving_camera_x,      // NEW
                     /*y*/ -moving_camera_y,      // NEW
                     /*z*/ -moving_camera_z)      // NEW
          .rotateY(/*radians*/ -moving_camera_rot_y)    // NEW
          .rotateX(/*radians*/ -moving_camera_rot_x);   // NEW
        // end new camera transformations
        Vertex3 ndcSpace = cameraSpace
          .ortho(/*min_x*/ -100.0f,
                 /*max_x*/ 100.0f,
                 /*min_y*/ -100.0f,
                 /*max_y*/ 100.0f,
                 /*min_z*/ 100.0f,
                 /*max_z*/ -100.0f);
        glVertex3f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y,
                   /*z*/ ndcSpace.z);
      }
      glEnd();
    }
//----
//Draw paddle 2, relative to the world-space origin.
//[source,C,linenums]
//----
    glBegin(GL_QUADS);
    {
      glColor3f(/*red*/   1.0,
                /*green*/ 1.0,
                /*blue*/  0.0);
      for(Vertex3 modelspace : paddle3D){
        Vertex3 worldSpace = modelspace
          .rotateZ(/*radians*/ paddle_2_rotation)
          .translate(/*x*/ 90.0,
                     /*y*/ paddle_2_offset_Y,
                     /*z*/ 0.0);
        // new camera transformations
        Vertex3 cameraSpace = worldSpace
          .translate(/*x*/ -moving_camera_x,      // NEW
                     /*y*/ -moving_camera_y,      // NEW
                     /*z*/ -moving_camera_z)      // NEW
          .rotateY(/*radians*/ -moving_camera_rot_y)    // NEW
          .rotateX(/*radians*/ -moving_camera_rot_x);   // NEW
        // end new camera transformations
        Vertex3 ndcSpace = cameraSpace
          .ortho(/*min_x*/ -100.0f,
                 /*max_x*/ 100.0f,
                 /*min_y*/ -100.0f,
                 /*max_y*/ 100.0f,
                 /*min_z*/ 100.0f,
                 /*max_z*/ -100.0f);
        glVertex3f(/*x*/ ndcSpace.x,
                   /*y*/ ndcSpace.y,
                   /*z*/ ndcSpace.z);
      }
    }
    glEnd();
    return;
  }

////TODO - discuss the framebuffer, and how it allows us to draw in
////       a depth-independent manner.  we could force the programmer
////       to sort objects by depth before drawing, but that's why mario64
////       looked good and crash bandicoot had limited perspectives.
////       also reference the section in the beginning which clears the
////       depth buffer.

//----
//== Considering Depth
//[source,C,linenums]
//----
  if(*chapter_number >= 15){
    glEnable(GL_DEPTH_TEST);
  }
////TODO - write something about how "now that depth testing is enabled for all subequent demos, rerun the previous demo to show that the square becomes hidden as the user navigates

  if(15 == *chapter_number){
    *chapter_number = 14;
    return;
  }
//----
//== Perspective Viewing


////TODO -  make a new chapter which uses perspective, but
////        doesn't use lambdas
////TODO -  afterwards, in this chapter, explain perspective
////TODO -  use graphviz to show the stack of transforamations.
////        combine them into a tree.
////        talk about how how I used to think that the important
////        "objects" were the vertices, colors, etc, but the important
////        concept is relative spaces, i.e. the coordinate frame of references.
////
////        Since it's a stack, the last operator is pushed first, but applied last.
////        This means that transformations need to be "read" backwards to understand
////        what's happening.

//[source,C,linenums]
//----
  if(*chapter_number >= 16){
    static bool first_frame = true;
    if(first_frame){
      moving_camera_z = 400.0; // for the perspective to look right
      first_frame = false;
    }
  }
//----
//[source,C,linenums]
//----
  // use stacks for transformations
  std::vector<Vertex3_transformer> transformationStack;
  Vertex3_transformer withTransformations = [&](Vertex3 v){
    Vertex3 result = v;
    for(std::vector<Vertex3_transformer>::reverse_iterator
          rit = transformationStack.rbegin();
        rit!=transformationStack.rend();
        rit++)
      {
        result = (*rit)(result);
      }
    return result;
  };
//----


//[source,C,linenums]
//----
  if(16 == *chapter_number){
    // every shape is projected the same way
    transformationStack.push_back([&](Vertex3 v){
        return v.perspective(-0.1f,
                             -1000.0f);
      });
    // THE REST IS THE SAME AS THE PREVIOUS
    // every shape is relative to the camera
    // camera transformation #3 - tilt your head down
    transformationStack.push_back([&](Vertex3 v){
        return v.rotateX(/*radians*/ -moving_camera_rot_x);
      });
    // camera transformation #2 - turn your head to the side
    transformationStack.push_back([&](Vertex3 v){
        return v.rotateY(/*radians*/ -moving_camera_rot_y);
      });
    // camera transformation #1 - move to the origin
    transformationStack.push_back([&](Vertex3 v){
        return v.translate(/*x*/ - moving_camera_x,
                           /*y*/ - moving_camera_y,
                           /*z*/ - moving_camera_z);
      });
    transformationStack.push_back([&](Vertex3 v){
        return v.translate(/*x*/ -90.0f,
                           /*y*/ 0.0f + paddle_1_offset_Y,
                           /*z*/ 0.0f);
      });
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    transformationStack.push_back([&](Vertex3 v){
        return v.rotateZ(/*radians*/ paddle_1_rotation);
      });
    transformationStack.push_back([&](Vertex3 v){
        return v.scale(/*x*/ 10.0f,
                       /*y*/ 30.0f,
                       /*z*/ 1.0f);
      });
    draw_square3_programmable(withTransformations);
    transformationStack.pop_back();
    transformationStack.pop_back();
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    {
      transformationStack.push_back([&](Vertex3 v){
          return v.rotateZ(/*radians*/ paddle_1_rotation);
        });
      transformationStack.push_back([&](Vertex3 v){
          return v.rotateZ(/*radians*/ rotation_around_paddle_1);
        });
      transformationStack.push_back([&](Vertex3 v){
          return v.translate(/*x*/ 20.0f,
                             /*y*/ 0.0f,
                             /*z*/ -10.0f); // NEW, using a non zero
        });
      transformationStack.push_back([&](Vertex3 v){
          return v.rotateZ(/*radians*/ square_rotation);
        });
      transformationStack.push_back([&](Vertex3 v){
          return v.scale(/*x*/ 5.0f,
                         /*y*/ 5.0f,
                         /*z*/ 1.0f);
        });
      draw_square3_programmable(withTransformations);
      transformationStack.pop_back();
      transformationStack.pop_back();
      transformationStack.pop_back();
      transformationStack.pop_back();
      transformationStack.pop_back();
    }
    // get back to the world-space origin
    transformationStack.pop_back();
//----
//Draw paddle 2, relative to the world-space origin.
//[source,C,linenums]
//----
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  0.0);
    transformationStack.push_back([&](Vertex3 v){
        return v.translate(/*x*/ 90.0f,
                           /*y*/ 0.0f + paddle_2_offset_Y,
                           /*z*/ 0.0f);
      });
    transformationStack.push_back([&](Vertex3 v){
        return v.rotateZ(/*radians*/ paddle_2_rotation);
      });
    transformationStack.push_back([&](Vertex3 v){
        return v.scale(/*x*/ 10.0f,
                       /*y*/ 30.0f,
                       /*z*/ 1.0f);
      });
    draw_square3_programmable(withTransformations);
    transformationStack.pop_back();
    transformationStack.pop_back();
    transformationStack.pop_back();
    transformationStack.pop_back();
    transformationStack.pop_back();
    transformationStack.pop_back();
    transformationStack.pop_back();
    return;
  }
//----
//== OpenGL 2.0 Matricies
//[source,C,linenums]
//----
  if(*chapter_number >= 17){
////TODO - discuss that OpenGL uses left hand rule by default, so Z
////TODO - into the screen is positive.  I don't know why that is,
////TODO - but perhaps it is to save a bit since you're never gonna look at stuff behind you
    glClearDepth(1.1f );
    glDepthFunc(GL_LEQUAL);
  }
//----
//[source,C,linenums]
//----
  std::function<void()> draw_square_opengl2point1 = [&](){
    glBegin(GL_QUADS);
    {
      glVertex2f(/*x*/ -1.0,
                 /*y*/ -1.0);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ -1.0);
      glVertex2f(/*x*/ 1.0,
                 /*y*/ 1.0);
      glVertex2f(/*x*/ -1.0,
                 /*y*/ 1.0);
    }
    glEnd();
  };
//----
////TODO - describe matricies as efficient substitions for our vertex transforamiotns.
////TODO - describe how they act as a stack, similarly to how was done in the above.
//[source,C,linenums]
//----
  if(17 == *chapter_number){
    /*
     *  Demo 17 - OpenGL 1.4 Matricies
     */
    // set up Camera
    {
      // define the projection
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
      {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        gluPerspective(45.0f,
                       (GLdouble)w / (GLdouble)h,
                       0.1f,
                       1000.0f);
      }
      // move the "camera"
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
////TODO - describe how these matrix rotations act on arbitrary axises, not just XYZ
      glRotatef(/*degrees*/ RAD_TO_DEG(-moving_camera_rot_x),
                /*x*/ 1.0,
                /*y*/ 0.0,
                /*z*/ 0.0);
      glRotatef(/*degrees*/ RAD_TO_DEG(-moving_camera_rot_y),
                /*x*/ 0.0,
                /*y*/ 1.0,
                /*z*/ 0.0);
      glTranslatef(/*x*/ -moving_camera_x,
                   /*y*/ -moving_camera_y,
                   /*z*/ -moving_camera_z);
    }
//----
//Draw paddle 1, relative to the world-space origin.
//[source,C,linenums]
//----
    glPushMatrix();
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  1.0);
    glTranslatef(/*x*/ -90.0f,
                 /*y*/ 0.0f + paddle_1_offset_Y,
                 /*z*/ 0.0);
    glRotatef(/*degrees*/ RAD_TO_DEG(paddle_1_rotation),
              /*x*/ 0.0,
              /*y*/ 0.0,
              /*z*/ 1.0);
    // scaling of this object should not affect the relative square
    glPushMatrix();
    {
      glScalef(/*x*/ 10.0f,
               /*y*/ 30.0f,
               /*z*/ 1.0f);
      draw_square_opengl2point1();
      glPopMatrix();
    }
//----
//Draw square, relative to paddle 1.
//[source,C,linenums]
//----
    glColor3f(/*red*/   0.0,
              /*green*/ 0.0,
              /*blue*/  1.0);
    glRotatef(/*degrees*/ RAD_TO_DEG(rotation_around_paddle_1),
              /*x*/ 0.0f,
              /*y*/ 0.0f,
              /*z*/ 1.0f);
    glTranslatef(/*x*/ 20.0f,
                 /*y*/ 0.0f,
                 /*z*/ -10.0f);
    glRotatef(/*degrees*/ RAD_TO_DEG(square_rotation),
              /*x*/ 0.0f,
              /*y*/ 0.0f,
              /*z*/ 1.0f);
    glScalef(/*x*/ 5.0f,
             /*y*/ 5.0f,
             /*z*/ 5.0f);
    draw_square_opengl2point1();
    glPopMatrix();
//----
//Draw paddle 2, relative to the world-space origin.
//[source,C,linenums]
//----
    glPushMatrix();
    glColor3f(/*red*/   1.0,
              /*green*/ 1.0,
              /*blue*/  0.0);
    glTranslatef(/*x*/ 90.0f,
                 /*y*/ 0.0f + paddle_2_offset_Y,
                 /*z*/ 0.0);
    glRotatef(/*degrees*/ RAD_TO_DEG(paddle_2_rotation),
              /*x*/ 0.0,
              /*y*/ 0.0,
              /*z*/ 1.0);
    glScalef(/*x*/ 10.0f,
             /*y*/ 30.0f,
             /*z*/ 1.0f);
    draw_square_opengl2point1();
    glPopMatrix();
    return;
  }
  // in later demos,
  //glClearDepth(1.0f );
  //glEnable(GL_DEPTH_TEST );
  //glDepthFunc(GL_LEQUAL );
  return;
}
//----
//
//[[sharedLibAppendix]]
//[appendix]
//== Shared Libraries
//
//Foo bar baz2
//
//
//[bibliography]
//Example Bibliography
//--------------------
//The bibliography list is a style of AsciiDoc bulleted list.
//
//[bibliography]
//.Books
//- [[[taoup]]] Eric Steven Raymond. 'The Art of Unix
//  Programming'. Addison-Wesley. ISBN 0-13-142901-9.
//  - [[[walsh-muellner]]] Norman Walsh & Leonard Muellner.
//    'DocBook - The Definitive Guide'. O'Reilly & Associates. 1999.
//      ISBN 1-56592-580-7.
//
//[bibliography]
//.Articles
//- [[[abc2003]]] Gall Anonim. 'An article', Whatever. 2003.
//
//
//[glossary]
//Example Glossary
//----------------
//Glossaries are optional. Glossaries entries are an example of a style
//of AsciiDoc labeled lists.
//
//[glossary]
//A glossary term::
//  The corresponding (indented) definition.
//
//A second glossary term::
//  The corresponding (indented) definition.
//
//
//[colophon]
//Example Colophon
//----------------
//Text at the end of a book describing facts about its production.
//
//
//[index]
//Example Index
//-------------
//////////////////////////////////////////////////////////////////
//The index is normally left completely empty, it's contents being
//generated automatically by the DocBook toolchain.
//////////////////////////////////////////////////////////////////
