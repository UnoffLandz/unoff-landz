/**
 * @file
 * @ingroup wrapper
 * @brief file i/o functions with support for zip and gzip files
 */
#ifndef UUID_ff56ac18_4412_418c_b07e_b67a8634ec2c
#define UUID_ff56ac18_4412_418c_b07e_b67a8634ec2c

#include "../platform.h"
#include "../cal3d_wrapper.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern CalCoreAnimation *CalLoader_ELLoadCoreAnimation(CalLoader *self, const char *strFilename);
extern CalCoreMaterial *CalLoader_ELLoadCoreMaterial(CalLoader *self, const char *strFilename);
extern CalCoreMesh *CalLoader_ELLoadCoreMesh(CalLoader *self, const char *strFilename);
extern CalCoreSkeleton *CalLoader_ELLoadCoreSkeleton(CalLoader *self, const char *strFilename);
extern int CalCoreModel_ELLoadCoreAnimation(CalCoreModel *self, const char *strFilename, float scale);
extern int CalCoreModel_ELLoadCoreMaterial(CalCoreModel *self, const char *strFilename);
extern int CalCoreModel_ELLoadCoreMesh(CalCoreModel *self, const char *strFilename);
extern enum CalBoolean CalCoreModel_ELLoadCoreSkeleton(CalCoreModel *self, const char *strFilename);
extern void set_invert_v_coord();

#ifdef __cplusplus
}
#endif


#endif	/* UUID_ff56ac18_4412_418c_b07e_b67a8634ec2c */

