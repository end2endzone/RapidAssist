#include <cstdio>
#include <cstdlib> //for printf()
#include "rapidassist/strings.h"

int main(void)
{
  std::string whoiam = "My name is Antoine and I am a superhero.";

  //process with search and replace in string.
  int numReplaced = ra::strings::strReplace(whoiam, "hero", "vilan");
  
  //output result on console.
  printf("%s", whoiam.c_str());
  
  return 0;
}
