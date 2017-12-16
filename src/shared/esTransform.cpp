#include "esUtil.h"
#include <math.h>
#include <string.h>

#define PI 3.1415926535897932384626433832795f

void ESUTIL_API
esScale(ESMatrix *result, float sx, float sy, float sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;

}

void ESUTIL_API esScale(ESMatrix *result, float s){
    esScale(result,s,s,s);
    result->m[3][0] *= s;
    result->m[3][1] *= s;
    result->m[3][2] *= s;
    result->m[3][3] *= s;
}

void ESUTIL_API
esTranslate(ESMatrix* result, float tx, float ty, float tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void ESUTIL_API
esRotate(ESMatrix* result, float angle, float x, float y, float z)
{
   float sinAngle, cosAngle;
   float mag = sqrtf(x * x + y * y + z * z);
      
   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f )
   {
      float xx, yy, zz, xy, yz, zx, xs, ys, zs;
      float oneMinusCos;
      ESMatrix rotMat;
   
      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.m[0][1] = (oneMinusCos * xy) - zs;
      rotMat.m[0][2] = (oneMinusCos * zx) + ys;
      rotMat.m[0][3] = 0.0F; 

      rotMat.m[1][0] = (oneMinusCos * xy) + zs;
      rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.m[1][2] = (oneMinusCos * yz) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = (oneMinusCos * zx) - ys;
      rotMat.m[2][1] = (oneMinusCos * yz) + xs;
      rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.m[2][3] = 0.0F; 

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      esMatrixMultiply( result, &rotMat, result );
   }
}

void ESUTIL_API
esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    frust;

    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
         (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
         return;

    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;

    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

    esMatrixMultiply(result, &frust, result);
}


void ESUTIL_API 
esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
   float frustumW, frustumH;
   
   frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void ESUTIL_API esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    ortho;

    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;

    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

    esMatrixMultiply(result, &ortho, result);
}

#include <stdio.h>

void printMat(ESMatrix* m){
    printf("\n\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n\n",m->m[0][0],m->m[0][1],m->m[0][2],m->m[0][3],
            m->m[1][0],m->m[1][1],m->m[1][2],m->m[1][3],
            m->m[2][0],m->m[2][1],m->m[2][2],m->m[2][3],
            m->m[3][0],m->m[3][1],m->m[3][2],m->m[3][3]);
}

void ESUTIL_API esMatrixAdd(ESMatrix *res, ESMatrix *srcA, ESMatrix *srcB)
{
    for (int i=0; i<4; i++){
        for (int j=0; j<4; j++){
            res->m[i][j] = srcA->m[i][j]+srcB->m[i][j];
        }
    }
}

void ESUTIL_API
esMatrixMultiply(ESMatrix* res, const ESMatrix* srcA, const ESMatrix* srcB)
{
    ESMatrix    tmp;
    int         i;

	for (i=0; i<4; i++)
	{
        tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
                        (srcA->m[i][1] * srcB->m[1][0]) +
                        (srcA->m[i][2] * srcB->m[2][0]) +
                        (srcA->m[i][3] * srcB->m[3][0]) ;

        tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) +
                        (srcA->m[i][1] * srcB->m[1][1]) +
                        (srcA->m[i][2] * srcB->m[2][1]) +
                        (srcA->m[i][3] * srcB->m[3][1]) ;

        tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) +
                        (srcA->m[i][1] * srcB->m[1][2]) +
                        (srcA->m[i][2] * srcB->m[2][2]) +
                        (srcA->m[i][3] * srcB->m[3][2]) ;

        tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) +
                        (srcA->m[i][1] * srcB->m[1][3]) +
                        (srcA->m[i][2] * srcB->m[2][3]) +
                        (srcA->m[i][3] * srcB->m[3][3]) ;
	}
    memcpy(res, &tmp, sizeof(ESMatrix));
}

ESMatrix ESUTIL_API esMatrixMultiply(const ESMatrix *srcA, const ESMatrix *srcB){
    ESMatrix res;
    esMatrixMultiply(&res,srcA,srcB);
    return res;
}

void ESUTIL_API esMatrixLoadIdentity(ESMatrix* model) {
    memset(model->m, 0x0, sizeof(float)*4*4);
    model->m[0][0] = 1.0f;
    model->m[1][1] = 1.0f;
    model->m[2][2] = 1.0f;
    model->m[3][3] = 1.0f;
}

void ESUTIL_API RPY2DCM(float R, float P, float Y, ESMatrix& A)
{
  float t[12];
  t[0] = sin(Y);
  t[1] = cos(Y);
  t[2] = sin(P);
  t[3] = cos(P);
  t[4] = sin(R);
  t[5] = cos(R);
  t[6] = t[4]*t[2];
  t[7] = t[5]*t[2];
  A.m[0][0] = t[3]*t[1];
  A.m[0][1] = t[3]*t[0];
  A.m[0][2] = -t[2];
  A.m[1][0] = t[6]*t[1]-t[5]*t[0];
  A.m[1][1] = t[6]*t[0]+t[5]*t[1];
  A.m[1][2] = t[4]*t[3];
  A.m[2][0] = t[7]*t[1]+t[4]*t[0];
  A.m[2][1] = t[7]*t[0]-t[4]*t[1];
  A.m[2][2] = t[5]*t[3];
}

void ESUTIL_API DCM2RPY(ESMatrix A, float& R, float& P, float& Y)
{
  R =  atan2( A.m[1][2],A.m[2][2]);
  P = -atan2( A.m[0][2],sqrt(A.m[1][2]*A.m[1][2]+A.m[2][2]*A.m[2][2]));
  Y =  atan2( A.m[0][1],A.m[0][0]);
  if (Y<0)
    Y = Y+M_PI*2;
}

void ESUTIL_API esTranspose(ESMatrix& result,  const ESMatrix& in){
    result.m[0][0] = in.m[0][0];
    result.m[0][1] = in.m[1][0];
    result.m[0][2] = in.m[2][0];
    result.m[0][3] = in.m[3][0];

    result.m[1][0] = in.m[0][1];
    result.m[1][1] = in.m[1][1];
    result.m[1][2] = in.m[2][1];
    result.m[1][3] = in.m[3][1];

    result.m[2][0] = in.m[0][2];
    result.m[2][1] = in.m[1][2];
    result.m[2][2] = in.m[2][2];
    result.m[2][3] = in.m[3][2];

    result.m[3][0] = in.m[0][3];
    result.m[3][1] = in.m[1][3];
    result.m[3][2] = in.m[2][3];
    result.m[3][3] = in.m[3][3];
}
