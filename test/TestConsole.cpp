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
  TEST_F(TestConsole, testGetDimension)
  {
    int width = -1;
    int height = -1;
    ra::console::getDimension(width, height);
    printf("Console dimension: %dx%d\n", width, height);
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
    static const double MAX_TIME_SECONDS = 3.0; //maximum running time of the test in seconds

    double time_start = ra::time::getMillisecondsTimer();
    double time_end = time_start + MAX_TIME_SECONDS;

    while(ra::time::getMillisecondsTimer() < time_end)
    {
      ra::console::printAnimationCursor();
      ra::time::millisleep(80);
    }
    printf("\n");
  }
  //--------------------------------------------------------------------------------------------------
    namespace ansi
    {
      //https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences
      //https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal

      namespace FormatAttribute
      {
        enum Attr
        {
          Default = 0,
          Bold = 1,
          Dim = 2,
          Underlined = 3,
          Blink = 5,
          Reverse = 7,
          Hidden = 8
        };
        const char * toString(const Attr & attr)
        {
          switch(attr)
          {
            case Default    : return "Default"   ; break;
            case Bold       : return "Bold"      ; break;
            case Dim        : return "Dim"       ; break;
            case Underlined : return "Underlined"; break;
            case Blink      : return "Blink"     ; break;
            case Reverse    : return "Reverse"   ; break;
            case Hidden     : return "Hidden"    ; break;
          };
          return "Unknown";
        }
        static const int NUM_COLOR_ATTR = 7;
      };
      namespace ForegroundColor
      {
        enum Color
        {
          Default = 39,
          Black = 30,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          LightGray,
          DarkGray = 90,
          LightRed,
          LightGreen,
          LightYellow,
          LightBlue,
          LightMagenta,
          LightCyan,
          White,
        };
        const char * toString(const Color & color)
        {
          switch(color)
          {
            case Default      : return "Default"      ; break;
            case Black        : return "Black"        ; break;
            case Red          : return "Red"          ; break;
            case Green        : return "Green"        ; break;
            case Yellow       : return "Yellow"       ; break;
            case Blue         : return "Blue"         ; break;
            case Magenta      : return "Magenta"      ; break;
            case Cyan         : return "Cyan"         ; break;
            case LightGray    : return "LightGray"    ; break;
            case DarkGray     : return "DarkGray"     ; break;
            case LightRed     : return "LightRed"     ; break;
            case LightGreen   : return "LightGreen"   ; break;
            case LightYellow  : return "LightYellow"  ; break;
            case LightBlue    : return "LightBlue"    ; break;
            case LightMagenta : return "LightMagenta" ; break;
            case LightCyan    : return "LightCyan"    ; break;
            case White        : return "White"        ; break;
          };
          return "Unknown";
        }
        static const int NUM_FOREGROUND_COLOR = 17;
      }; //namespace ForegroundColor
      namespace BackgroundColor
      {
        enum Color
        {
          Default = 49,
          Black = 40,
          Red,
          Green,
          Yellow,
          Blue,
          Magenta,
          Cyan,
          LightGray,
          DarkGray = 100,
          LightRed,
          LightGreen,
          LightYellow,
          LightBlue,
          LightMagenta,
          LightCyan,
          White,
        };
        const char * toString(const Color & color)
        {
          switch(color)
          {
            case Default      : return "Default"      ; break;
            case Black        : return "Black"        ; break;
            case Red          : return "Red"          ; break;
            case Green        : return "Green"        ; break;
            case Yellow       : return "Yellow"       ; break;
            case Blue         : return "Blue"         ; break;
            case Magenta      : return "Magenta"      ; break;
            case Cyan         : return "Cyan"         ; break;
            case LightGray    : return "LightGray"    ; break;
            case DarkGray     : return "DarkGray"     ; break;
            case LightRed     : return "LightRed"     ; break;
            case LightGreen   : return "LightGreen"   ; break;
            case LightYellow  : return "LightYellow"  ; break;
            case LightBlue    : return "LightBlue"    ; break;
            case LightMagenta : return "LightMagenta" ; break;
            case LightCyan    : return "LightCyan"    ; break;
            case White        : return "White"        ; break;
          };
          return "Unknown";
        }
        static const int NUM_BACKGROUND_COLOR = 17;
      }; //namespace BackgroundColor
    }; //namespace ansi

  TEST_F(TestConsole, testAllAnsiColors)
  {
    int format_attributes[] = {
      ansi::FormatAttribute::Default,
      ansi::FormatAttribute::Bold,
      ansi::FormatAttribute::Dim,
      ansi::FormatAttribute::Underlined,
      //ansi::FormatAttribute::Blink,
      //ansi::FormatAttribute::Reverse,
      //ansi::FormatAttribute::Hidden,
    };
    int format_attributes_count = sizeof(format_attributes)/sizeof(format_attributes[0]);

    int foreground_colors[] = {
      //ansi::ForegroundColor::Default,
      ansi::ForegroundColor::Black,
      ansi::ForegroundColor::Red,
      ansi::ForegroundColor::Green,
      ansi::ForegroundColor::Yellow,
      ansi::ForegroundColor::Blue,
      ansi::ForegroundColor::Magenta,
      ansi::ForegroundColor::Cyan,
      ansi::ForegroundColor::LightGray,
      ansi::ForegroundColor::DarkGray,
      ansi::ForegroundColor::LightRed,
      ansi::ForegroundColor::LightGreen,
      ansi::ForegroundColor::LightYellow,
      ansi::ForegroundColor::LightBlue,
      ansi::ForegroundColor::LightMagenta,
      ansi::ForegroundColor::LightCyan,
      ansi::ForegroundColor::White,
    };
    int foreground_colors_count = sizeof(foreground_colors)/sizeof(foreground_colors[0]);

    int background_colors[] = {
      //ansi::BackgroundColor::Default,
      ansi::BackgroundColor::Black,
      ansi::BackgroundColor::Red,
      ansi::BackgroundColor::Green,
      ansi::BackgroundColor::Yellow,
      ansi::BackgroundColor::Blue,
      ansi::BackgroundColor::Magenta,
      ansi::BackgroundColor::Cyan,
      ansi::BackgroundColor::LightGray,
      ansi::BackgroundColor::DarkGray,
      ansi::BackgroundColor::LightRed,
      ansi::BackgroundColor::LightGreen,
      ansi::BackgroundColor::LightYellow,
      ansi::BackgroundColor::LightBlue,
      ansi::BackgroundColor::LightMagenta,
      ansi::BackgroundColor::LightCyan,
      ansi::BackgroundColor::White,
    };
    int background_colors_count = sizeof(background_colors)/sizeof(background_colors[0]);

    for(int j=0; j<foreground_colors_count; j++)
    {
      for(int k=0; k<background_colors_count; k++)
      {
        for(int i=0; i<format_attributes_count; i++)
        {
          ansi::FormatAttribute::Attr ansi_attr = ansi::FormatAttribute::Attr(format_attributes[i]);
          ansi::ForegroundColor::Color ansi_foreground = ansi::ForegroundColor::Color(foreground_colors[j]);
          ansi::BackgroundColor::Color ansi_background = ansi::BackgroundColor::Color(background_colors[k]);

          printf("\033[%d;%d;%dm", ansi_attr, ansi_foreground, ansi_background);
          //printf("{%d;%d;%d}\033[0m ", format_attributes[i], foreground_colors[j], background_colors[k]);
          printf("{%s;%s;%s}\033[0m ", ansi::FormatAttribute::toString(ansi_attr), ansi::ForegroundColor::toString(ansi_foreground), ansi::BackgroundColor::toString(ansi_background));
        }
        printf("\n\n");
      }
    }
  }

  TEST_F(TestConsole, testTextGroundColor)
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
    ra::console::setDefaultTextColor();
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
    ra::console::setDefaultTextColor();
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
