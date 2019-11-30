#include "cParticle.hpp"

Particle::Particle()
{
    alive = true;
    pos[0] = pos[1] = pos[2] = 0;
    oDiffuse[0] = 1;
    oDiffuse[1] = oDiffuse[2] = 0;
    diffuse[0] = oDiffuse[0];
    diffuse[1] = oDiffuse[1];
    diffuse[2] = oDiffuse[2];
    mass = 5.0f; // 5kg
    oldPos[0] = pos[0];
    oldPos[1] = pos[1];
    oldPos[2] = pos[2];
    forces[0] = forces[1] = forces[2] = 0;
    dragForce[0] = dragForce[1] = dragForce[2] = 0;
    specular[0] = specular[1] = specular[2] = 0;
    shininess = 100;
    radius = mass / 10;
    age = 0;
    restitutionCoefficient = 0.7f; //Elastic with the floor, how much it bounces with the floor
    area = 4.0f * M_PI * radius * radius;
}

void Particle::draw(){
    glPushMatrix();
    {
        glTranslatef(pos[0], pos[1], pos[2]);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
        glutSolidSphere(radius, 20, 20);
    }
    glPopMatrix();
}

void Particle::stepSimulation(float dt){
    //checkFloorCollisions();
    integrateVerlet(dt);
}

void Particle::addForce(float *force){
    forces[0] += force[0];
    forces[1] += force[1];
    forces[2] += force[2];
}

void Particle::integrateVerlet(float dt){
    float rho = 150.0f;
    float dist[3] = {abs(pos[0] - oldPos[0]), abs(pos[1] - oldPos[1]),abs(pos[2] - oldPos[2])};
    float v[3] = {dist[0]/dt, dist[1]/dt, dist[2]/dt};
    float v2[3] = {v[0] * v[0], v[1] * v[1], v[2] * v[2]};
    float denom[3] = {rho*v2[0]*area, rho*v2[1]*area, rho*v2[2]*area};
    float numer[3] = {forces[0]*2, forces[1]*2, forces[2]*2};
    float cd[3] = {numer[0]/denom[0], numer[1]/denom[1], numer[2]/denom[2]};
    
    if(denom[0] != 0 && dist[0] != 0) forces[0] -= cd[0];
    if(denom[1] != 0 && dist[1] != 0) forces[1] -= cd[1];
    if(denom[2] != 0 && dist[2] != 0) forces[2] -= cd[2];
    
    //Compute acceleration
    float accel[3] = { forces[0] / mass, forces[1] / mass, forces[2] / mass };
    // Sabe a temporary copy of the current position
    float temp[3] = { pos[0], pos[1], pos[2] };
    // Integrate the new position using Verlet's method:
    pos[0] = 2 * pos[0] - oldPos[0] + accel[0] * dt * dt;
    pos[1] = 2 * pos[1] - oldPos[1] + accel[1] * dt * dt;
    pos[2] = 2 * pos[2] - oldPos[2] + accel[2] * dt * dt;
    oldPos[0] = temp[0];
    oldPos[1] = temp[1];
    oldPos[2] = temp[2];
    
    
    

}

void Particle::checkFloorCollisions(){
    if (pos[1] <= 0) {
        forces[1] = -forces[1] * restitutionCoefficient;
        pos[1] = 0;
    }
}

bool Particle::inCollision(Particle *other){
    float dx = pos[0] - other -> pos[0];
    float dy = pos[1] - other -> pos[1];
    float dz = pos[2] - other -> pos[2];
    
    float dx2 = dx * dx;
    float dy2 = dy * dy;
    float dz2 = dz * dz;
    
    float dist2 = dx2 + dy2 + dz2;
    float sumR = radius + other-> radius;
    float sumR2 = sumR * sumR;
    
    return dist2 <= sumR2;
}

Particle::~Particle(){
    
}
