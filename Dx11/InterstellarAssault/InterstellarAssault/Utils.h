#pragma once


// Clamp for int: Clamps an integer value between a specified minimum and maximum.
int Clamp(int val, int min, int max)
{
    if (val > max)
        val = max;  // Set to max if value exceeds the maximum.
    else if (val < min)
        val = min;  // Set to min if value falls below the minimum.

    return val;  // Return the clamped value.
}

// Clamp for float: Clamps a float value between a specified minimum and maximum.
float Clamp(float val, float min, float max)
{
    if (val > max)
        val = max;  // Set to max if value exceeds the maximum.
    else if (val < min)
        val = min;  // Set to min if value falls below the minimum.

    return val;  // Return the clamped value.
}

// Lerp: Performs linear interpolation between two values based on a t parameter.
float Lerp(float a, float b, float t)
{
    return a + t * (b - a);  // Return the interpolated value.
}