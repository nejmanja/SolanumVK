#pragma once

enum class KeyCode
{
    None,
    KeyW,
    KeyA,
    KeyS,
    KeyD,
    KeyQ,
    KeyE
};

enum class KeyAction
{
    None,
    Press,
    Release
};

struct MouseOffset
{
    float x{0.0f};
    float y{0.0f};
};

struct ScrollOffset
{
    float x{0.0f};
    float y{0.0f};
};