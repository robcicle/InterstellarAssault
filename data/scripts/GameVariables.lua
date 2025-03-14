-- Player configuration variables:
-- playerSprite variable: Defines the path to the sprite image used for the player's ship.
playerSprite = "sprites/ship2.dds"
-- playerLifes variable: Defines the number of lives the player starts with.
playerLifes = 5

-- bgnd_01 variable: Defines the path to the first image of the background's layers.
bgnd_01 = "background_layers/background02_001.dds"
-- bgnd_02 variable: Defines the path to the second image of the background's layers.
bgnd_02 = "background_layers/background02_002.dds"

-- Enemies configuration variables:
-- enemiesPerRow: Specifies the number of enemies in each row.
enemiesPerRow = 11
-- numOfRows: Specifies the total number of rows of enemies.
numOfRows = 5
-- rowXSpacing: Defines the horizontal spacing between enemies within a row.
rowXSpacing = 10
-- rowYSpacing: Defines the vertical spacing between each row of enemies.
rowYSpacing = 50
-- enemyInitialY: Sets the initial Y position for the first row of enemies.
enemyInitialY = 120
-- ufoInitialY: Sets the initial Y position for UFO enemies.
ufoInitialY = 50

-- Enemy behavior variables:
-- enemyDownstep: Specifies the vertical step enemies take when moving downward.
enemyDownstep = 15
-- enemyLimitOffset: Sets the horizontal movement limit offset for enemies.
enemyLimitOffset = 25