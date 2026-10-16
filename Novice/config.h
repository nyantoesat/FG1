#pragma once

// ===== Window =====
const int kWindowWidth = 1280;
const int kWindowHeight = 720;

// ===== Map (one screen = one circle) =====
const int kTileSize = 40; // 32 x 18 tiles = 1280 x 720
const int kMapCols = 32;
const int kMapRows = 18;
const int kWallTiles = 9;                                               // wall thickness on each side (tiles)
const float kShaftLeft = (float)(kWallTiles * kTileSize);               // 360
const float kShaftRight = (float)((kMapCols - kWallTiles) * kTileSize); // 920

// ===== Cage (hellvator) =====
const float kCageWidth = 240.0f;
const float kCageHeight = 200.0f;
const float kCageX = kWindowWidth * 0.5f - kCageWidth * 0.5f;
const float kCageRestY = 520.0f; // top of the cage roof when it is parked

// ===== Player =====
const float kPlayerWidth = 28.0f;
const float kPlayerHeight = 28.0f;
const float kPlayerMaxSpeed = 5.5f;
const float kPlayerGroundAccel = 0.9f;
const float kPlayerAirAccel = 0.6f;
const float kPlayerGroundFriction = 0.75f;
const float kPlayerAirFriction = 0.9f;
const float kJumpPower = 13.0f;
const float kGravity = 0.6f;
const float kMaxFallSpeed = 13.0f;
const int kCoyoteFrames = 6;     // jump still allowed a few frames after leaving the ground
const int kJumpBufferFrames = 8; // jump pressed a bit before landing still works

// Wall hang / climb (Mittens style)
const float kClimbUpSpeed = 3.5f;
const float kClimbDownSpeed = 5.0f;
const float kWallSlideSpeed = 0.8f; // slow slide when hanging without pressing up/down
const float kWallJumpX = 7.0f;
const float kWallJumpY = 12.0f;
const int kWallJumpLockFrames = 10; // no horizontal control right after a wall jump

// Health
const int kMaxHearts = 6;
const int kStartLives = 9;
const int kInvincibleFrames = 60;

// ===== Run / waves =====
const int kCircleCount = 9;
const int kWavesPerCircle = 3;
const int kWaveFrames = 900;      // 15 seconds per wave
const int kWaveBreakFrames = 150; // 2.5 second pause between waves