#pragma once

#define TYPE_STRUCTURAL 0
#define TYPE_SHEAR 1
#define TYPE_FLEXION 2

// Base stiffness values
#define STRUCTURAL_STIFFNESS 1000.0f
#define SHEAR_STIFFNESS 500.0f
#define FLEXION_STIFFNESS 500.0f

#define STRUCTURAL_NATURAL_LENGTH 1.0f
// Corrected typo: NARTURAL -> NATURAL
#define SHEAR_NATURAL_LENGTH (STRUCTURAL_NATURAL_LENGTH * 1.41421356f)
#define FLEXION_NATURAL_LENGTH (STRUCTURAL_NATURAL_LENGTH * 2.0f)

struct Spring {
  int p1_idx;
  int p2_idx;
  int type;
};