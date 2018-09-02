/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "TestConsole.h"
#include "rapidassist/console.h"
#include "rapidassist/gtesthelp.h"
#include "rapidassist/time_.h"

namespace ra { namespace console { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestConsole::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestConsole::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testZeroBased)
  {
    //set x to the far left
    printf("Running test %s()\n", ra::gtesthelp::getTestQualifiedName().c_str());

    int after_x = 0;
    int after_y = 0;
    ra::console::getCursorPos(after_x, after_y);

    ASSERT_EQ(0, after_x);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetCursorPos)
  {
    int before_x = 0;
    int before_y = 0;
    ra::console::getCursorPos(before_x, before_y);
    
    //new value for y
    printf("Running test %s()\n", ra::gtesthelp::getTestQualifiedName().c_str());
    
    //new value for x
    printf("done!");

    int after_x = 0;
    int after_y = 0;
    ra::console::getCursorPos(after_x, after_y);

    printf("\n");

    ASSERT_NE(before_x, after_x);
    ASSERT_NE(before_y, after_y);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testSetCursorPos)
  {
    printf("Running test %s()", ra::gtesthelp::getTestQualifiedName().c_str());

    int before_x = 0;
    int before_y = 0;
    ra::console::getCursorPos(before_x, before_y);
    
    ra::console::setCursorPos(0, before_y);

    //erase running
    printf("-->    ");

    int after_x = 0;
    int after_y = 0;
    ra::console::getCursorPos(after_x, after_y);

    ASSERT_NE(before_x, after_x);
    ASSERT_EQ(before_y, after_y);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetAnimationSpriteLoop)
  {
    static const double refreshrate = 0.01;
    static int MAX_LOOP_COUNT = 100000;
    char current = ra::console::getAnimationSprite(refreshrate);

    //wait for the sprite to change
    int loopcount = 0;
    while(current == ra::console::getAnimationSprite(refreshrate) && loopcount <= MAX_LOOP_COUNT)
    {
      loopcount++;
    }
    ASSERT_NE(loopcount, MAX_LOOP_COUNT);

    //wait for returning to the first frame
    loopcount = 0;
    while(current != ra::console::getAnimationSprite(refreshrate) && loopcount <= MAX_LOOP_COUNT)
    {
      loopcount++;
    }
    ASSERT_NE(loopcount, MAX_LOOP_COUNT);

    //loop completed
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testPrintAnimationCursor)
  {
    static const double MAX_TIME_SECONDS = 5.0; //maximum running time is 5 seconds

    double time_start = ra::time::getMillisecondsTimer();
    double time_end = time_start + MAX_TIME_SECONDS;

    while(ra::time::getMillisecondsTimer() < time_end)
    {
      ra::console::printAnimationCursor();
      ra::time::millisleep(80);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testForeGroundColor)
  {
    int color_first = (int)ra::console::Black;
    int color_last = NUM_TEXT_COLOR - 1;

    //show all colors possibilities
    //for(int i=color_first; i <= color_last; i++)
    //{
    //  for(int j=color_first; j <= color_last; j++)
    //  {
    //    ra::console::TextColor foreground = (ra::console::TextColor)i;
    //    ra::console::TextColor background = (ra::console::TextColor)j;
    //    ra::console::setTextColor(foreground, background);
    //    printf("%s on %s\n", ra::console::getTextColorName(foreground), ra::console::getTextColorName(background)); 
    //  }
    //}

    //all foreground colors
    for(int i=color_first; i <= color_last; i++)
    {
      ra::console::TextColor foreground = (ra::console::TextColor)i;
      ra::console::TextColor background = ra::console::Black;
      if (foreground == ra::console::Black)
        background = ra::console::White;
      ra::console::setTextColor(foreground, background);
      printf("%s ", ra::console::getTextColorName(foreground)); 
    }
    printf("\n");

    //all background colors
    for(int i=color_first; i <= color_last; i++)
    {
      ra::console::TextColor foreground = ra::console::Black;
      ra::console::TextColor background = (ra::console::TextColor)i;
      if (background == ra::console::Black)
        foreground = ra::console::White;
      ra::console::setTextColor(foreground, background);
      printf("%s ", ra::console::getTextColorName(background)); 
    }
    printf("\n");

    //reset to default
    ra::console::setDefaultTextColor();
    printf("Back to normal...\n");
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testDesktopConsole)
  {
    printf("isDesktopGuiAvailable()=%d\n", ra::console::isDesktopGuiAvailable());
    printf("isRunFromDesktop()=%d\n", ra::console::isRunFromDesktop());
    printf("hasConsoleOwnership()=%d\n", ra::console::hasConsoleOwnership());
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace console
} //namespace ra
