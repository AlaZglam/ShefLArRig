#include <stdio.h>
#include <visa.h>

int main()
{
  ViSession rm = VI_NULL;
  ViSession vi = VI_NULL;
  viOpenDefaultRM( &rm );
  ViChar search[] = "*";//"PXI?*::INSTR";
  ViFindList find = VI_NULL;
  ViUInt32 count = 0;
  ViChar rsrc[256];
  ViChar modelName[256];
  ViStatus status = viFindRsrc( rm, search, &find, &count, rsrc );

if ( status==VI_SUCCESS && count>0 )
  {
    do
      {
	viOpen(rm, rsrc, 0, 0, &vi);
	viGetAttribute(vi, VI_ATTR_MODEL_NAME, modelName);
	viClose(vi);
	printf( "Found: \"%s\" - Model Name: %s\n", rsrc, modelName);
	status = viFindNext( find, rsrc );
      } while( status==VI_SUCCESS );
    viClose( find );
  }
 viClose( rm );
}
