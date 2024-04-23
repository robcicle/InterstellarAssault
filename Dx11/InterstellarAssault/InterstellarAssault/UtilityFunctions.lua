-- Function for generating a random number then returning it.
function randomNumber(min, max)
	return math.random(min, max)
end

-- Function for linear interpolation between two values based on a t parameter.
function lerpNumber(a, b, t)
	return a + t * (b - a)
end