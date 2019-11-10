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
#include "rapidassist/testing.h"
#include "rapidassist/timing.h"

namespace ra { namespace console { namespace test
{
  //--------------------------------------------------------------------------------------------------
  void TestConsole::SetUp() {
  }
  //--------------------------------------------------------------------------------------------------
  void TestConsole::TearDown() {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testZeroBased) {
    //set x to the far left
    printf("Running test %s()\n", ra::testing::GetTestQualifiedName().c_str());

    int after_x = 0;
    int after_y = 0;
    ra::console::GetCursorPos(after_x, after_y);

    ASSERT_EQ(0, after_x);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetCursorPosYAxis) {
    int before_x = 0;
    int before_y = 0;
    ra::console::GetCursorPos(before_x, before_y);

    printf("Line1\nLine2\n");

    int after_x = 0;
    int after_y = 0;
    ra::console::GetCursorPos(after_x, after_y);

    //note: if Line1 and Line2 was printed at the bottom of the console/terminal, then the text scrolled up leaving Line1 and Line2 at the same y offset.

    //detect if Line2 was printed at the bottom of the screen
    int width = -1;
    int height = -1;
    ra::console::GetDimension(width, height);
    if (after_y == height) {
      printf("inconclusive...\n");
    }
    else {
      //test proceed as normal
      ASSERT_LT(before_y, after_y);
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetCursorPosXAxis) {
    int before_x = 0;
    int before_y = 0;
    ra::console::GetCursorPos(before_x, before_y);

    printf("Offset1");

    int after_x = 0;
    int after_y = 0;
    ra::console::GetCursorPos(after_x, after_y);

    printf("\n");

    ASSERT_LT(before_x, after_x);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetCursorPosPerformance) {
    //loop for 3 seconds
    static const double time_length = 3.0; //seconds
    double time_start = ra::timing::GetMicrosecondsTimer();
    double time_end = time_start + time_length;

    int loop_count = 0;
    while (ra::timing::GetMicrosecondsTimer() < time_end) {
      int before_x = 0;
      int before_y = 0;
      ra::console::GetCursorPos(before_x, before_y);

      loop_count++;
    }

    //compute average time per call
    double timePerCallSec = time_length / double(loop_count);
    double timePerCallMicro = timePerCallSec * 1000.0 * 1000.0;
    printf("Time per GetCursorPos() calls: %.6fus\n", timePerCallMicro);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, DISABLED_testSetCursorPos) {
    printf("Running test %s()", ra::testing::GetTestQualifiedName().c_str());

    int before_x = 0;
    int before_y = 0;
    ra::console::GetCursorPos(before_x, before_y);

    ra::console::SetCursorPos(0, before_y);

    //erase running
    printf("-->    ");

    int after_x = 0;
    int after_y = 0;
    ra::console::GetCursorPos(after_x, after_y);

    printf("\n");

    ASSERT_NE(before_x, after_x);
    ASSERT_EQ(before_y, after_y);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetDimension) {
    int width = -1;
    int height = -1;
    ra::console::GetDimension(width, height);
    printf("Console dimension: %dx%d\n", width, height);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testGetAnimationSpriteLoop) {
    static const double refreshrate = 0.01;
    static int MAX_LOOP_COUNT = 100000;
    char current = ra::console::GetAnimationSprite(refreshrate);

    //wait for the sprite to change
    int loopcount = 0;
    while (current == ra::console::GetAnimationSprite(refreshrate) && loopcount <= MAX_LOOP_COUNT) {
      loopcount++;
    }
    ASSERT_NE(loopcount, MAX_LOOP_COUNT);

    //wait for returning to the first frame
    loopcount = 0;
    while (current != ra::console::GetAnimationSprite(refreshrate) && loopcount <= MAX_LOOP_COUNT) {
      loopcount++;
    }
    ASSERT_NE(loopcount, MAX_LOOP_COUNT);

    //loop completed
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testPrintAnimationCursor) {
    static const double MAX_TIME_SECONDS = 3.0; //maximum running time of the test in seconds

    double time_start = ra::timing::GetMillisecondsTimer();
    double time_end = time_start + MAX_TIME_SECONDS;

    while (ra::timing::GetMillisecondsTimer() < time_end) {
      ra::console::PrintAnimationCursor();
      ra::timing::Millisleep(80);
    }
    printf("\n");
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, DISABLED_testAllAnsiColors) {
    int format_attributes[] = {
      ansi::FormatAttribute::DEFAULT,
      ansi::FormatAttribute::BOLD,
      ansi::FormatAttribute::DIM,
      ansi::FormatAttribute::UNDERLINED,
      //ansi::FormatAttribute::BLINK,
      //ansi::FormatAttribute::REVERSE,
      //ansi::FormatAttribute::HIDDEN,
    };
    int format_attributes_count = sizeof(format_attributes) / sizeof(format_attributes[0]);

    int foreground_colors[] = {
      //ansi::ForegroundColor::DEFAULT,
      ansi::ForegroundColor::BLACK,
      ansi::ForegroundColor::RED,
      ansi::ForegroundColor::GREEN,
      ansi::ForegroundColor::YELLOW,
      ansi::ForegroundColor::BLUE,
      ansi::ForegroundColor::MAGENTA,
      ansi::ForegroundColor::CYAN,
      ansi::ForegroundColor::LIGHT_GRAY,
      ansi::ForegroundColor::DARK_GRAY,
      ansi::ForegroundColor::LIGHT_RED,
      ansi::ForegroundColor::LIGHT_GREEN,
      ansi::ForegroundColor::LIGHT_YELLOW,
      ansi::ForegroundColor::LIGHT_BLUE,
      ansi::ForegroundColor::LIGHT_MAGENTA,
      ansi::ForegroundColor::LIGHT_CYAN,
      ansi::ForegroundColor::WHITE,
    };
    int foreground_colors_count = sizeof(foreground_colors) / sizeof(foreground_colors[0]);

    int background_colors[] = {
      //ansi::BackgroundColor::DEFAULT,
      ansi::BackgroundColor::BLACK,
      ansi::BackgroundColor::RED,
      ansi::BackgroundColor::GREEN,
      ansi::BackgroundColor::YELLOW,
      ansi::BackgroundColor::BLUE,
      ansi::BackgroundColor::MAGENTA,
      ansi::BackgroundColor::CYAN,
      ansi::BackgroundColor::LIGHT_GRAY,
      ansi::BackgroundColor::DARK_GRAY,
      ansi::BackgroundColor::LIGHT_RED,
      ansi::BackgroundColor::LIGHT_GREEN,
      ansi::BackgroundColor::LIGHT_YELLOW,
      ansi::BackgroundColor::LIGHT_BLUE,
      ansi::BackgroundColor::LIGHT_MAGENTA,
      ansi::BackgroundColor::LIGHT_CYAN,
      ansi::BackgroundColor::WHITE,
    };
    int background_colors_count = sizeof(background_colors) / sizeof(background_colors[0]);

    for (int j = 0; j < foreground_colors_count; j++) {
      for (int k = 0; k < background_colors_count; k++) {
        for (int i = 0; i < format_attributes_count; i++) {
          ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::Attr(format_attributes[i]);
          ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::Color(foreground_colors[j]);
          ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::Color(background_colors[k]);

          printf("\033[%d;%d;%dm", ansi_attr, ansi_foreground, ansi_background);
          //printf("{%d;%d;%d}\033[0m ", format_attributes[i], foreground_colors[j], background_colors[k]);
          printf("{%s;%s;%s}\033[0m ", ansi::FormatAttribute::ToString(ansi_attr), ansi::ForegroundColor::ToString(ansi_foreground), ansi::BackgroundColor::ToString(ansi_background));
        }
        printf("\n\n");
      }
    }
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testTextGroundColor) {
    int color_first = (int)ra::console::BLACK;
    int color_last = NUM_TEXT_COLOR - 1;

    //show all colors possibilities
    //for(int i=color_first; i <= color_last; i++)
    //{
    //  for(int j=color_first; j <= color_last; j++)
    //  {
    //    ra::console::TextColor foreground = (ra::console::TextColor)i;
    //    ra::console::TextColor background = (ra::console::TextColor)j;
    //    ra::console::SetTextColor(foreground, background);
    //    printf("%s on %s\n", ra::console::GetTextColorName(foreground), ra::console::GetTextColorName(background)); 
    //  }
    //}

    //all foreground colors
    for (int i = color_first; i <= color_last; i++) {
      ra::console::TextColor foreground = (ra::console::TextColor)i;
      ra::console::TextColor background = ra::console::BLACK;
      if (foreground == ra::console::BLACK)
        background = ra::console::WHITE;
      ra::console::SetTextColor(foreground, background);
      printf("%s ", ra::console::GetTextColorName(foreground));
    }
    ra::console::SetDefaultTextColor();
    printf("\n");

    //all background colors
    for (int i = color_first; i <= color_last; i++) {
      ra::console::TextColor foreground = ra::console::BLACK;
      ra::console::TextColor background = (ra::console::TextColor)i;
      if (background == ra::console::BLACK)
        foreground = ra::console::WHITE;
      ra::console::SetTextColor(foreground, background);
      printf("%s ", ra::console::GetTextColorName(background));
    }
    ra::console::SetDefaultTextColor();
    printf("\n");

    //reset to default
    ra::console::SetDefaultTextColor();
    printf("Back to normal...\n");
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testDesktopConsole) {
    printf("IsDesktopGuiAvailable()=%d\n", ra::console::IsDesktopGuiAvailable());
    printf("IsRunFromDesktop()=%d\n", ra::console::IsRunFromDesktop());
    printf("HasConsoleOwnership()=%d\n", ra::console::HasConsoleOwnership());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestConsole, testWaitKeyPress) {
    printf("Press 10 keys to continue. Pressing 2+ bytes keys will only register the first key.\n");

    int line = 0;
    for (int i = 0; i < 10; i++) {
      int keycode = ra::console::WaitKeyPress();
      char c = (char)keycode;
      line++;
      line = line % 256;

      if (c != 0x1B)
        printf("line=%03d, key=0x%x(hex) %d(dec) '%c'\n", line, keycode, keycode, c);
      else
        printf("line=%03d, key=0x%x(hex) %d(dec)\n", line, keycode, keycode);
    }
  }
  //--------------------------------------------------------------------------------------------------
} //namespace test
} //namespace console
} //namespace ra
