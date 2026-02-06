#pragma once
#include "Spring.h"
#include <Eigen/Dense>
#include <vector>

class Cloth {
public:
  int width, height;

  // Data-Oriented Layout
  std::vector<Eigen::Vector3d> positions;
  std::vector<Eigen::Vector3d> old_positions; // For Verlet Stability
  std::vector<Eigen::Vector3d> velocities;
  std::vector<Eigen::Vector3d> forces;

  std::vector<Spring> springs;
  std::vector<unsigned int> spring_indices;
  std::vector<unsigned int> triangle_indices; // For Solid Mesh

  // Simulation Parameters
  float mass = 1.0f;
  float gravity = -9.8f;
  float damping = 0.01f;
  float structural_stiffness = STRUCTURAL_STIFFNESS;
  float shear_stiffness = SHEAR_STIFFNESS;
  float flexion_stiffness = FLEXION_STIFFNESS;

  Eigen::Vector3d wind_force = Eigen::Vector3d(0, 0, 0);

  // OpenGL Buffers
  unsigned int VAO, VBO, EBO, TBO;

  Cloth();
  ~Cloth();

  void Simulate(float dt);
  void InitMesh();
  void UpdateMesh();
  void Draw();

private:
  void ComputeForces();
  void Integration(float dt);
};