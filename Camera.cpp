#include "Camera.hpp"

Camera::Camera(){
    pos[0] = pos[1] = pos[2] = 0;
    dir[0] = 1;
    dir[1] = 0;
    dir[2] = 1;
    up[0] = 0;
    up[1] = 1;
    up[2] = 0;
    pivot[0] = pivot[1] = pivot[2] = 0;
    near_plane = 1.0;
    far_plane = 64;
    fov = 45;
    tang = (float)tan(GRAD2RAD * fov * 0.5) ;
    
    Z[0] = dir[0];
    Z[1] = dir[1];
    Z[2] = dir[2];
    
    normalize(Z);
    normalize(up);
    
    X[0] = Z[1]*up[2] - Z[2]*up[1];
    X[1] = Z[2]*up[0] - Z[0]*up[2];
    X[2] = Z[0]*up[1] - Z[1]*up[0];
    normalize(X);
    
    Y[0] = X[1]*Z[2] - X[2]*Z[1];
    Y[1] = X[2]*Z[0] - X[0]*Z[2];
    Y[2] = X[0]*Z[1] - X[1]*Z[0];
    normalize(Y);
    
}

void Camera::init(){
    pos[0] = pos[1] = pos[2] = 0;
    dir[0] = 1;
    dir[1] = 0;
    dir[2] = 1;
    up[0] = 0;
    up[1] = 1;
    up[2] = 0;
    pivot[0] = pivot[1] = pivot[2] = 0;
    near_plane = 1.0;
    far_plane = 64;
    fov = 45;
}


void Camera::setPos(float x, float y, float z){
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void Camera::setDirVec(float x, float y, float z){
    float mag = sqrt(x*x+y*y+z*z);
    if(mag > 0){
        dir[0] = x/mag;
        dir[1] = y/mag;
        dir[2] = z/mag;
    }
}

void Camera::setUpVec(float x, float y, float z){
    float mag = sqrt(x*x+y*y+z*z);
    if(mag > 0){
        up[0] = x/mag;
        up[1] = y/mag;
        up[2] = z/mag;
    }
}

void Camera::setPivot(float x, float y, float z){
    pivot[0] = x;
    pivot[1] = y;
    pivot[2] = z;
}

void Camera::moveForward(float dist){
    pos[0] += dist*dir[0];
    pos[1] += dist*dir[1];
    pos[2] += dist*dir[2];
}

void Camera::moveBackward(float dist){
    pos[0] -= dist*dir[0];
    pos[1] -= dist*dir[1];
    pos[2] -= dist*dir[2];
}

void Camera::moveRight(float dist){
    float right[3];
    crossProduct(dir, up, right);
    pos[0] += dist*right[0];
    pos[1] += dist*right[1];
    pos[2] += dist*right[2];
}

void Camera::moveLeft(float dist){
    float right[3];
    crossProduct(dir, up, right);
    pos[0] -= dist*right[0];
    pos[1] -= dist*right[1];
    pos[2] -= dist*right[2];
}

void Camera::moveUp(float dist){
    pos[0] += dist*up[0];
    pos[1] += dist*up[1];
    pos[2] += dist*up[2];
}

void Camera::moveDown(float dist){
    pos[0] -= dist*up[0];
    pos[1] -= dist*up[1];
    pos[2] -= dist*up[2];
}

void Camera::move(float dist, float angle){
    float right[3], moveDir[3];
    float cosAng, sinAng, radAng;
    
    radAng = angle*M_PI/180.0;
    cosAng = cos(radAng);
    sinAng = sin(radAng);
    
    crossProduct(dir, up, right);
    
    moveDir[0] = dir[0]*cosAng - right[0]*sinAng;
    moveDir[1] = dir[1]*cosAng - right[1]*sinAng;
    moveDir[2] = dir[2]*cosAng - right[2]*sinAng;
    
    pos[0] += dist*moveDir[0];
    pos[1] += dist*moveDir[1];
    pos[2] += dist*moveDir[2];
}

void Camera::setAgentCamera(float *p, float d, float height){
    //void Camera::setAgentCamera(Agent *agent){
    this->dir[0] = cos(d * 3.1415927 / 180);
    this->dir[1] = 0;
    this->dir[2] = sin(d * 3.1415927 / 180);
    
    this->pos[0] = p[0] - (dir[0]*1.0);
    this->pos[2] = p[1] - (dir[2]*1.0);
    
    this->pos[1] = height;
}

#define CAMERA_ROTATE_AXIS

void Camera::rotate(float angle, float x, float y, float z){
    float newDir[3], newUp[3], axis[3], radAng;
#ifdef CAMERA_ROTATE_AXIS
    float origAxis[3];
    float right[3];
    
    crossProduct(dir, up, right);
    origAxis[0] = x;
    origAxis[1] = y;
    origAxis[2] = z;
    unitVector(origAxis);
    axis[0] = origAxis[0]*right[0] + origAxis[1]*up[0] + origAxis[2]*dir[0];
    axis[1] = origAxis[0]*right[1] + origAxis[1]*up[1] + origAxis[2]*dir[1];
    axis[2] = origAxis[0]*right[2] + origAxis[1]*up[2] + origAxis[2]*dir[2];
#else
    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
    unitVector(axis);
#endif
    radAng = angle * 3.1415927 / 180.0;
    rotatePointAroundAxis(dir, axis, radAng, newDir);
    rotatePointAroundAxis(up,  axis, radAng, newUp);
    
    dir[0] = newDir[0];
    dir[1] = newDir[1];
    dir[2] = newDir[2];
    up[0]  = newUp[0];
    up[1]  = newUp[1];
    up[2]  = newUp[2];
}

void Camera::moveAround(float angle, float x, float y, float z){
    float negPos[3];
    float newDir[3], newUp[3], newPos[3], axis[3], radAng;
    float distPivot;
#ifdef CAMERA_ROTATE_AXIS
    float origAxis[3];
    float right[3];
    
    crossProduct(dir, up, right);
    origAxis[0] = x;
    origAxis[1] = y;
    origAxis[2] = z;
    unitVector(origAxis);
    axis[0] = origAxis[0]*right[0] + origAxis[1]*up[0] + origAxis[2]*dir[0];
    axis[1] = origAxis[0]*right[1] + origAxis[1]*up[1] + origAxis[2]*dir[1];
    axis[2] = origAxis[0]*right[2] + origAxis[1]*up[2] + origAxis[2]*dir[2];
#else
    axis[0] = x;
    axis[1] = y;
    axis[2] = z;
    unitVector(axis);
#endif
    
    negPos[0] = pos[0] - pivot[0];
    negPos[1] = pos[1] - pivot[1];
    negPos[2] = pos[2] - pivot[2];
    distPivot = vecMagnitude(negPos);
    unitVector(negPos);
    
    radAng = -angle * 3.1415927 / 180.0;
    
    rotatePointAroundAxis(negPos, axis, radAng, newPos);
    rotatePointAroundAxis(dir,    axis, radAng, newDir);
    rotatePointAroundAxis(up,     axis, radAng, newUp);
    
    pos[0] = pivot[0] + newPos[0]*distPivot;
    pos[1] = pivot[1] + newPos[1]*distPivot;
    pos[2] = pivot[2] + newPos[2]*distPivot;
    dir[0] = newDir[0];
    dir[1] = newDir[1];
    dir[2] = newDir[2];
    up[0]  = newUp[0];
    up[1]  = newUp[1];
    up[2]  = newUp[2];
    
}

void Camera::getWorldFrustumCoords(float *coords){
    float tans[2];
    int viewportDims[4];
    float vecX[3];
    float deltaNear[3][3], deltaFar[3][3];
    float dx, dy;
    float aspect;
    
    glGetIntegerv(GL_VIEWPORT, viewportDims);
    aspect = (float) viewportDims[2] / (float) viewportDims[3];
    tans[1] = tan(fov/2.0);
    tans[0] = tans[1] * aspect;
    //    crossProduct(dir, up, vecX);
    crossProduct(up, dir, vecX);
    
    dx = near_plane * tans[0]; //nearW
    dy = near_plane * tans[1]; // nearH
    deltaNear[0][0] = vecX[0] * dx;
    deltaNear[0][1] = vecX[1] * dx;
    deltaNear[0][2] = vecX[2] * dx;
    deltaNear[1][0] = up[0]   * dy;
    deltaNear[1][1] = up[1]   * dy;
    deltaNear[1][2] = up[2]   * dy;
    deltaNear[2][0] = dir[0]  * near_plane;
    deltaNear[2][1] = dir[1]  * near_plane;
    deltaNear[2][2] = dir[2]  * near_plane;
    
    dx = far_plane * tans[0];
    dy = far_plane * tans[1];
    deltaFar[0][0] = vecX[0] * dx;
    deltaFar[0][1] = vecX[1] * dx;
    deltaFar[0][2] = vecX[2] * dx;
    deltaFar[1][0] = up[0]   * dy;
    deltaFar[1][1] = up[1]   * dy;
    deltaFar[1][2] = up[2]   * dy;
    deltaFar[2][0] = dir[0]  * far_plane;
    deltaFar[2][1] = dir[1]  * far_plane;
    deltaFar[2][2] = dir[2]  * far_plane;
    
    coords[0]  = pos[0] - deltaNear[0][0] - deltaNear[1][0] + deltaNear[2][0];
    coords[1]  = pos[1] - deltaNear[0][1] - deltaNear[1][1] + deltaNear[2][1];
    coords[2]  = pos[2] - deltaNear[0][2] - deltaNear[1][2] + deltaNear[2][2];
    coords[3]  = pos[0] + deltaNear[0][0] - deltaNear[1][0] + deltaNear[2][0];
    coords[4]  = pos[1] + deltaNear[0][1] - deltaNear[1][1] + deltaNear[2][1];
    coords[5]  = pos[2] + deltaNear[0][2] - deltaNear[1][2] + deltaNear[2][2];
    coords[6]  = pos[0] - deltaNear[0][0] + deltaNear[1][0] + deltaNear[2][0];
    coords[7]  = pos[1] - deltaNear[0][1] + deltaNear[1][1] + deltaNear[2][1];
    coords[8]  = pos[2] - deltaNear[0][2] + deltaNear[1][2] + deltaNear[2][2];
    coords[9]  = pos[0] + deltaNear[0][0] + deltaNear[1][0] + deltaNear[2][0];
    coords[10] = pos[1] + deltaNear[0][1] + deltaNear[1][1] + deltaNear[2][1];
    coords[11] = pos[2] + deltaNear[0][2] + deltaNear[1][2] + deltaNear[2][2];
    
    coords[12] = pos[0] - deltaFar[0][0] - deltaFar[1][0] + deltaFar[2][0];
    coords[13] = pos[1] - deltaFar[0][1] - deltaFar[1][1] + deltaFar[2][1];
    coords[14] = pos[2] - deltaFar[0][2] - deltaFar[1][2] + deltaFar[2][2];
    coords[15] = pos[0] + deltaFar[0][0] - deltaFar[1][0] + deltaFar[2][0];
    coords[16] = pos[1] + deltaFar[0][1] - deltaFar[1][1] + deltaFar[2][1];
    coords[17] = pos[2] + deltaFar[0][2] - deltaFar[1][2] + deltaFar[2][2];
    coords[18] = pos[0] - deltaFar[0][0] + deltaFar[1][0] + deltaFar[2][0];
    coords[19] = pos[1] - deltaFar[0][1] + deltaFar[1][1] + deltaFar[2][1];
    coords[20] = pos[2] - deltaFar[0][2] + deltaFar[1][2] + deltaFar[2][2];
    coords[21] = pos[0] + deltaFar[0][0] + deltaFar[1][0] + deltaFar[2][0];
    coords[22] = pos[1] + deltaFar[0][1] + deltaFar[1][1] + deltaFar[2][1];
    coords[23] = pos[2] + deltaFar[0][2] + deltaFar[1][2] + deltaFar[2][2];
    
}

void Camera::draw(){
    float coords[24];
    //float linePoints[24];
    
    getWorldFrustumCoords(coords);
    
    GLboolean enabled=false;
    glGetBooleanv(GL_LIGHTING, &enabled);
    if (enabled)
        glDisable(GL_LIGHTING);
    
    glColor3f(1.0,0.0,0.0);
    glLineWidth(2.0);
    
    glBegin(GL_LINE_STRIP);
    glVertex3f(coords[0],  coords[1],  coords[2]); //nbr
    glVertex3f(coords[3],  coords[4],  coords[5]); //nbl
    glVertex3f(coords[9],  coords[10], coords[11]);//ntl
    glVertex3f(coords[6],  coords[7],  coords[8]); //ntr
    glVertex3f(coords[0],  coords[1],  coords[2]);
    glVertex3f(coords[12], coords[13], coords[14]);
    glVertex3f(coords[18], coords[19], coords[20]);
    glVertex3f(coords[6],  coords[7],  coords[8]);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glVertex3f(coords[15], coords[16], coords[17]); //fbl
    glVertex3f(coords[12], coords[13], coords[14]); //fbr
    glVertex3f(coords[18], coords[19], coords[20]); //ftr
    glVertex3f(coords[21], coords[22], coords[23]); //ftl
    glVertex3f(coords[15], coords[16], coords[17]);
    glVertex3f(coords[3],  coords[4],  coords[5]);
    glVertex3f(coords[9],  coords[10], coords[11]);
    glVertex3f(coords[21], coords[22], coords[23]);
    glEnd();
    
    if (enabled)
        glEnable(GL_LIGHTING);
}

void Camera::setView(){
    int viewportCoords[4];
    
    glGetIntegerv(GL_VIEWPORT, viewportCoords);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)viewportCoords[2] / (GLfloat)viewportCoords[3], near_plane, far_plane);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
              pos[0], pos[1], pos[2],
              pos[0]+dir[0], pos[1]+dir[1], pos[2]+dir[2],
              up[0], up[1], up[2]);
}

void Camera::readMatrices(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    setView();
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
    //    glMatrixMode(GL_PROJECTION);
    //    glMultMatrixf(modelviewMatrix);
    //    glGetFloatv(GL_PROJECTION_MATRIX, modelviewProjectionMatrix);
    //    glMatrixMode(GL_MODELVIEW);
    //    glMultMatrixf(projectionMatrix);
    //    glGetFloatv(GL_MODELVIEW_MATRIX, modelviewProjectionMatrix);
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
/*
 void Camera::initVisibility(int res){
 if(visMap == NULL){
 visMap = new VisibilityMap();
 visMap->width = visMap->height = res;
 visMap->pos[0] = visMap->pos[1] = 0;
 visMap->pos[2] = -1;
 visMap->size[0] = visMap->size[1] = visMap->size[2] = res;
 visMap->allocData();
 }
 }
 */
/*
 void Camera::updateVisibility(){
 visMap->updateVisibility2d(this);
 }
 */
void Camera::normalize(float *p)
{
    double len;
    
    len = sqrt (p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    if (len > 0.0){
        p[0] /= len;
        p[1] /= len;
        p[2] /= len;
    }
    else {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
    }
}

float dotProd( const vector3f &v1,  const vector3f &v2 )
{
    return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z  );
}

vector3f crossProd( const vector3f &v1,  const vector3f &v2 )
{
    vector3f vCrossProduct;
    
    vCrossProduct.x = v1.y * v2.z - v1.z * v2.y;
    vCrossProduct.y = v1.z * v2.x - v1.x * v2.z;
    vCrossProduct.z = v1.x * v2.y - v1.y * v2.x;
    
    return vCrossProduct;
}

float distanceToPlane(const vector3f &planeNormal, const float planeDistance, const vector3f &point){
    return (planeNormal.x*point.x + planeNormal.y*point.y + planeNormal.z*point.z) + planeDistance;
}

bool Camera::pointInFrustrum(float* p) {
    vector3f point = {p[0],p[1],p[2]};
    
    float coords[24];
    
    getWorldFrustumCoords(coords);
    
    
    vector3f nbr = {coords[0],  coords[1],  coords[2]}; //nbr
    vector3f nbl = {coords[3],  coords[4],  coords[5]}; //nbl
    vector3f ntl = {coords[9],  coords[10], coords[11]};//ntl
    vector3f ntr = {coords[6],  coords[7],  coords[8]}; //ntr
    vector3f fbl = {coords[15], coords[16], coords[17]}; //fbl
    vector3f fbr = {coords[12], coords[13], coords[14]}; //fbr
    vector3f ftr = {coords[18], coords[19], coords[20]}; //ftr
    vector3f ftl = {coords[21], coords[22], coords[23]}; //ftl
    
    // Getting frustrum planes
    
    vector3f front, right, back, left, top, bottom, vector1, vector2, crossFront, crossRight, crossBack, crossLeft, crossTop, crossBottom;
    float dFront, dRight, dBack, dLeft, dTop, dBottom;
    
    // Front
    vector1.x = ntl.x - nbl.x;
    vector1.y = ntl.y - nbl.y;
    vector1.z = ntl.z - nbl.z;
    
    vector2.x = nbr.x - nbl.x;
    vector2.y = nbr.y - nbl.y;
    vector2.z = nbr.z - nbl.z;
    
    crossFront = crossProd(vector1, vector2);
    
    dFront = -1 * (nbl.x*crossFront.x + nbl.y*crossFront.y + nbl.z*crossFront.z);
    
    // Back
    
    vector1.x = ftr.x - fbr.x;
    vector1.y = ftr.y - fbr.y;
    vector1.z = ftr.z - fbr.z;
    
    vector2.x = fbl.x - fbr.x;
    vector2.y = fbl.y - fbr.y;
    vector2.z = fbl.z - fbr.z;
    
    crossBack = crossProd(vector1, vector2);
    
    dBack = -1 * (fbr.x*crossBack.x + fbr.y*crossBack.y + fbr.z*crossBack.z);
    
    // Right
    
    vector1.x = ntr.x - nbr.x;
    vector1.y = ntr.y - nbr.y;
    vector1.z = ntr.z - nbr.z;
    
    vector2.x = fbr.x - nbr.x;
    vector2.y = fbr.y - nbr.y;
    vector2.z = fbr.z - nbr.z;
    
    crossRight = crossProd(vector1, vector2);
    
    dRight = -1 * (nbr.x*crossRight.x + nbr.y*crossRight.y + nbr.z*crossRight.z);
    
    // Left
    
    vector1.x = fbl.x - nbl.x;
    vector1.y = fbl.y - nbl.y;
    vector1.z = fbl.z - nbl.z;
    
    vector2.x = ntl.x - nbl.x;
    vector2.y = ntl.y - nbl.y;
    vector2.z = ntl.z - nbl.z;
    
    crossLeft = crossProd(vector1, vector2);
    
    dLeft = -1 * (nbl.x*crossLeft.x + nbl.y*crossLeft.y + nbl.z*crossLeft.z);
    
    // Top
    
    vector1.x = ftl.x - ntl.x;
    vector1.y = ftl.y - ntl.y;
    vector1.z = ftl.z - ntl.z;
    
    vector2.x = ntr.x - ntl.x;
    vector2.y = ntr.y - ntl.y;
    vector2.z = ntr.z - ntl.z;
    
    crossTop = crossProd(vector1, vector2);
    
    dTop = -1 * (ntl.x*crossTop.x + ntl.y*crossTop.y + ntl.z*crossTop.z);
    
    // Bottom
    
    vector1.x = nbr.x - nbl.x;
    vector1.y = nbr.y - nbl.y;
    vector1.z = nbr.z - nbl.z;
    
    vector2.x = fbl.x - nbl.x;
    vector2.y = fbl.y - nbl.y;
    vector2.z = fbl.z - nbl.z;
    
    crossBottom = crossProd(vector1, vector2);
    
    dBottom = -1 * (nbl.x*crossBottom.x + nbl.y*crossBottom.y + nbl.z*crossBottom.z);
    
    float distanceFront, distanceRight, distanceBack, distanceLeft, distanceTop, distanceBottom;
    
    distanceFront = distanceToPlane(crossFront, dFront, point);
    distanceRight = distanceToPlane(crossRight, dRight, point);
    distanceBack = distanceToPlane(crossBack, dBack, point);
    distanceLeft = distanceToPlane(crossLeft, dLeft, point);
    distanceTop = distanceToPlane(crossTop, dTop, point);
    distanceBottom = distanceToPlane(crossBottom, dBottom, point);
    
    if (distanceFront >= 0 && distanceRight >= 0 && distanceBack >= 0 && distanceLeft >= 0 && distanceTop >= 0 && distanceBottom >= 0){
        return true;
    }
    
    return false;
}
 
/*
bool Camera::pointInFrustrum(float* p) {
    int viewportDims[4];
        
    glGetIntegerv(GL_VIEWPORT, viewportDims);
    
    ratio = (float)viewportDims[2] / (float) viewportDims[3];
    
    vector3f z;
    vector3f y;
    vector3f x;
    
    z.x = dir[0];
    z.y = dir[1];
    z.z = dir[2];
    
    y.x = up[0];
    y.y = up[1];
    y.z = up[2];
    
    x = crossProd(z,y);
    
    float tang = 2 * tan(fov / 2);
    float height = far_plane * tang;
    float Wnear = height * ratio;
    float vx = p[0]-pos[0];
    float vy = p[1]-pos[1];
    float vz = p[2]-pos[2];
    
    float pcz, pcx, pcy;
    
    pcz = vx*dir[0] + vy*dir[1] + vz*dir[2];
    if (pcz > far_plane || pcz < near_plane) {
        printf("En la primera\n");
        return false;
    }
    
    pcy = vx*up[0] + vy*up[1] + vz*up[2];
    float aux = pcz * tang;
    if (pcy > aux || pcy < -aux) {
        printf("En la segunda\n");
        return false;
    }
    
    pcx = vx*x.x + vy*x.y + vz*x.z;
    aux = aux * ratio;
    if (pcx > aux || pcx < -aux) {
        printf("En la tercera\n");
        return false;
    }
    
    return true;
}

*/
