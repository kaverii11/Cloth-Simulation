#include "Cloth.h"
#include <GL/glew.h>
#include <iostream>

#define MESH_SIZE 15

Cloth::Cloth() {
  width = MESH_SIZE;
  height = MESH_SIZE;
  int num_particles = width * height;

  positions.resize(num_particles);
  old_positions.resize(num_particles);
  velocities.resize(num_particles, Eigen::Vector3d::Zero());
  forces.resize(num_particles, Eigen::Vector3d::Zero());

  // Initialize Grid
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int idx = i * height + j;
      positions[idx] =
          Eigen::Vector3d((j - width / 2.0), (i - height / 2.0), 0.0);
      old_positions[idx] = positions[idx];
    }
  }

  // Helper to add springs
  auto add_s = [&](int i1, int j1, int i2, int j2, int type) {
    if (i2 >= 0 && i2 < width && j2 >= 0 && j2 < height) {
      Spring s = {i1 * height + j1, i2 * height + j2, type};
      springs.push_back(s);
    }
  };

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      add_s(i, j, i + 1, j, TYPE_STRUCTURAL);
      add_s(i, j, i, j + 1, TYPE_STRUCTURAL);
      add_s(i, j, i + 1, j + 1, TYPE_SHEAR);
      add_s(i + 1, j, i, j + 1, TYPE_SHEAR);
      add_s(i, j, i + 2, j, TYPE_FLEXION);
      add_s(i, j, i, j + 2, TYPE_FLEXION);
    }
  }

  // Build Triangle Indices for solid rendering
  for (int i = 0; i < width - 1; i++) {
    for (int j = 0; j < height - 1; j++) {
      int tl = i * height + j;
      int tr = i * height + (j + 1);
      int bl = (i + 1) * height + j;
      int br = (i + 1) * height + (j + 1);
      triangle_indices.push_back(tl);
      triangle_indices.push_back(bl);
      triangle_indices.push_back(tr);
      triangle_indices.push_back(tr);
      triangle_indices.push_back(bl);
      triangle_indices.push_back(br);
    }
  }
}

void Cloth::Simulate(float dt) {
  ComputeForces();
  Integration(dt);
  UpdateMesh();
}

void Cloth::Integration(float dt) {
  for (int k = 0; k < positions.size(); k++) {
    // Pin top corners
    if (k == (width - 1) * height || k == (width - 1) * height + (height - 1))
      continue;

    Eigen::Vector3d temp = positions[k];
    Eigen::Vector3d acc = forces[k] / mass;

    // Verlet step with basic damping
    positions[k] =
        positions[k] + (positions[k] - old_positions[k]) * 0.99 + acc * dt * dt;
    old_positions[k] = temp;
  }
}

void Cloth::ComputeForces() {
  std::fill(forces.begin(), forces.end(), Eigen::Vector3d::Zero());
  Eigen::Vector3d grav(0, gravity, 0);

  for (auto &f : forces)
    f += grav + wind_force;

  for (const auto &s : springs) {
    Eigen::Vector3d delta = positions[s.p2_idx] - positions[s.p1_idx];
    double dist = delta.norm();
    if (dist < 1e-6)
      continue;

    double rest = (s.type == TYPE_STRUCTURAL) ? 1.0
                  : (s.type == TYPE_SHEAR)    ? 1.414
                                              : 2.0;
    float stiff =
        (s.type == TYPE_STRUCTURAL) ? structural_stiffness : shear_stiffness;

    Eigen::Vector3d f = delta.normalized() * (dist - rest) * stiff;
    forces[s.p1_idx] += f;
    forces[s.p2_idx] -= f;
  }
}

void Cloth::InitMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &TBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * sizeof(float), NULL,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               triangle_indices.size() * sizeof(unsigned int),
               triangle_indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

void Cloth::UpdateMesh() {
  std::vector<float> p_data;
  for (auto &p : positions) {
    p_data.push_back((float)p.x());
    p_data.push_back((float)p.y());
    p_data.push_back((float)p.z());
  }
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, p_data.size() * sizeof(float),
                  p_data.data());
}

void Cloth::Draw() {
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TBO);
  glDrawElements(GL_TRIANGLES, triangle_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

Cloth::~Cloth() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &TBO);
}