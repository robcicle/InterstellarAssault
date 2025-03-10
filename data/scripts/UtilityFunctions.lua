-- randomNumber function: Generates and returns a random number between the specified min and max values.
function randomNumber(min, max)
    -- Uses Lua's math.random to compute a random number in the provided range.
    return math.random(min, max)
end

-- lerpNumber function: Performs linear interpolation between two values based on parameter t.
function lerpNumber(a, b, t)
    -- Calculates the interpolated value using the formula: a + t * (b - a).
    return a + t * (b - a)
end