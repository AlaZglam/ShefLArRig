#include "AgMD2_GPM.h"

int main(int argc, char** argv)
{
  TApplication app("gpm",&argc, argv);
  app.ExitOnException();

  AgMD2_GPM gpmApp;
  gpmApp.app();

  app.Run();

  return 0;
}
