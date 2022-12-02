const TILE_SIZE = 64;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

const FOV_ANGLE = 60 * (Math.PI / 180);

const WALL_STRIP_WIDTH = 1;
const NUM_RAYS = WINDOW_WIDTH / WALL_STRIP_WIDTH;

const MINIMAP_SCALE_FACTOR = 0.2;



class Map 
{
    constructor() 
    {
        this.grid = 
        [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], // 0
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1], // 1
            [1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1], // 2
            [1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1], // 3
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1], // 4
            [1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1], // 5
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1], // 6
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1], // 7
            [1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1], // 8
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1], // 9
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]  // 10 (11 rows)
          // 0  1  2  3  4  5  6  7  8  9  0  1  2  3  4   (15 columns)  
        ];
    }

    hasWallAt(x, y)
    {
        if( x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT)
        {
            return true;
        }

        //note: X and Y are given in pixels, we need to translate this to tiles coordinates
        var mapGridIndexX = Math.floor( x / TILE_SIZE );
        var mapGridIndexY = Math.floor( y / TILE_SIZE )

        return (this.grid[mapGridIndexY][mapGridIndexX] != 0);
    }


    render() 
    {
        for (var i = 0; i < MAP_NUM_ROWS; i++) 
        {
            for (var j = 0; j < MAP_NUM_COLS; j++) 
            {
                var tileX = j * TILE_SIZE; 
                var tileY = i * TILE_SIZE;
                var tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(
                    MINIMAP_SCALE_FACTOR * tileX, 
                    MINIMAP_SCALE_FACTOR * tileY, 
                    MINIMAP_SCALE_FACTOR * TILE_SIZE, 
                    MINIMAP_SCALE_FACTOR * TILE_SIZE
                    );
            }
        }
    }
}

class Player
{
    constructor()
    {
        this.x = WINDOW_WIDTH / 2;
        this.y = WINDOW_HEIGHT / 7;
        this.radius = 3;
        this.turnDirection = 0; // -1 if left, +1 if right
        this.walkDirection = 0; // -1 if back, +1 if front
        this.rotationAngle = Math.PI / 2; // looking 90 deg, but since the screen start left to right top to bottom, it will point down
        this.moveSpeed = 4.0;
        this.rotationSpeed = 3 * (Math.PI / 180); // 2 * 0.0174 = 0.0349

    }
    update()
    {
        this.rotationAngle += this.turnDirection * this.rotationSpeed; //( +1 or -1 ) * rotationSpeed
        var moveStep = this.walkDirection * this.moveSpeed; //( +1 or -1 ) * moveSpeed

        var newPlayerX = this.x + Math.cos(this.rotationAngle) * moveStep;
        var newPlayerY = this.y + Math.sin(this.rotationAngle) * moveStep;

        if(!grid.hasWallAt(newPlayerX, newPlayerY))
        {
            this.x = newPlayerX;
            this.y = newPlayerY;
        }


    }
    render()
    {
        noStroke();
        fill("blue");
        circle(
            MINIMAP_SCALE_FACTOR * this.x, 
            MINIMAP_SCALE_FACTOR * this.y, 
                                   this.radius
            );
        stroke("lime");
        line(
            MINIMAP_SCALE_FACTOR * this.x, 
            MINIMAP_SCALE_FACTOR * this.y, 
            MINIMAP_SCALE_FACTOR * this.x + Math.cos(this.rotationAngle) * 15,
            MINIMAP_SCALE_FACTOR * this.y + Math.sin(this.rotationAngle) * 15
        );
    }
}

class Ray
{
    constructor(rayAngle)
    {
        this.rayAngle = normalizeAngle(rayAngle);
        this.wallHitX = 0;
        this.wallHitY = 0;
        this.distance = 0;
        this.wasHitVertical = false;

        //angles: for the purpose of this application (and the way JS works) the angle 0 will be 2 * PI
        // or 6.283185307179586
        // So UP is everything between: 2*PI or zero, and PI (6.28 or 0, and 3.14), 
        //and by exclusion, down when the flag UP is not set
        //
        // Right: Right is when we have angles between 0deg and 90deg, and, between 270 and 360
        // to put differently, between 0 and 0.5*PI or between 1.5*PI and 2*PI
        this.isRayFacingDown = this.rayAngle > 0 && this.rayAngle < Math.PI;
        this.isRayFacingUp = !this.isRayFacingDown;


        this.isRayFacingRight = this.rayAngle < 0.5 * Math.PI || this.rayAngle > 1.5 * Math.PI;
        this.isRayFacingLeft = !this.isRayFacingRight;
    }

    cast(columnId)
    {
        var xintercept, yintercept;
        var xstep, ystep;

        ///////////////////////////////////////////
        // HORIZONTAL RAY-GRID INTERSECTION CODE
        ///////////////////////////////////////////
        var foundHorzWallHit = false;
        var horzWallHitX = 0;
        var horzWallHitY = 0;

        // Find the y-coordinate of the closest horizontal grid intersenction
        //  take the player coordinates and divide it by the tile size and round down the division
        //  to find which tile (row) is directly above the player. Keep in mind the tile (rows) start
        //  from top to bottom, so it's 32,64,96,...
        // Now, in case the player is facing down, the nearest ray intersection is, the same calculation
        //  as above, but we add ONE TILE
        yintercept = Math.floor(player.y / TILE_SIZE) * TILE_SIZE;
        yintercept += this.isRayFacingDown ? TILE_SIZE : 0;

        // Find the x-coordinate of the closest horizontal grid intersection
        //  note: we have the yintercept, which in a triangle this would be the hypotenuse
        //   and we are trying to find the adjacent side, and we have the angle
        //  So the formula: tan(x) = opp / ajd -> adj = opp / tan(x)
        xintercept = player.x + (yintercept - player.y) / Math.tan(this.rayAngle);

        // Calculate the increment xstep and ystep
        ystep = TILE_SIZE;
        ystep *= this.isRayFacingUp ? -1 : 1;

        //again: adj = opp / tan(x)
        xstep = TILE_SIZE / Math.tan(this.rayAngle);
        //console.log("this.rayAngle: " + this.rayAngle.toFixed(4) + "  Math.tan(this.rayAngle): " + Math.tan(this.rayAngle).toFixed(4) + "   xstep: " + parseFloat(xstep).toFixed(4)   );
        // if the ray is supposed to face left but the xstep is positive, then multiply by -1, so the orientation is correct
        // if the ray is supposed to face right but the xstep is negative, then multiply by -1, so the orientation is correct
        xstep *= (this.isRayFacingLeft && xstep > 0) ? -1 : 1;
        xstep *= (this.isRayFacingRight && xstep < 0) ? -1 : 1;

        var nextHorzTouchX = xintercept;
        var nextHorzTouchY = yintercept;


        // Increment xstep and ystep until we find a wall
        while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH && 
               nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT ) 
        {
            if (grid.hasWallAt(nextHorzTouchX, nextHorzTouchY - (this.isRayFacingUp ? 1 : 0))) 
            {
                foundHorzWallHit = true;
                horzWallHitX = nextHorzTouchX;
                horzWallHitY = nextHorzTouchY;
                
                break;
            } 
	    else 
            {
                nextHorzTouchX += xstep;
                nextHorzTouchY += ystep;
            }
        }  
        
        
        ///////////////////////////////////////////
        // VERTICAL RAY-GRID INTERSECTION CODE
        ///////////////////////////////////////////
        var foundVertWallHit = false;
        var vertWallHitX = 0;
        var vertWallHitY = 0;

        // Find the x-coordinate of the closest vertical grid intersenction
        xintercept = Math.floor(player.x / TILE_SIZE) * TILE_SIZE;
        xintercept += this.isRayFacingRight ? TILE_SIZE : 0;

        // Find the y-coordinate of the closest vertical grid intersection
        yintercept = player.y + (xintercept - player.x) * Math.tan(this.rayAngle);

        // Calculate the increment xstep and ystep
        xstep = TILE_SIZE;
        xstep *= this.isRayFacingLeft ? -1 : 1;

        ystep = TILE_SIZE * Math.tan(this.rayAngle);
        ystep *= (this.isRayFacingUp && ystep > 0) ? -1 : 1;
        ystep *= (this.isRayFacingDown && ystep < 0) ? -1 : 1;

        var nextVertTouchX = xintercept;
        var nextVertTouchY = yintercept;


        // Increment xstep and ystep until we find a wall
        while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH && 
               nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) 
        {
            if (grid.hasWallAt(nextVertTouchX - (this.isRayFacingLeft ? 1 : 0), nextVertTouchY))
            {
                foundVertWallHit = true;
                vertWallHitX = nextVertTouchX;
                vertWallHitY = nextVertTouchY;
                break;
            } 
            else 
            {
                nextVertTouchX += xstep;
                nextVertTouchY += ystep;
            }
        }

        // Calculate both horizontal and vertical distances and choose the smallest value
        //  if you found a wall hit, calculate the distance
        var horzHitDistance = (foundHorzWallHit)
            ? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
            : Number.MAX_VALUE;
        var vertHitDistance = (foundVertWallHit)
            ? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
            : Number.MAX_VALUE;

        // only store the smallest of the distances
        this.wallHitX = (horzHitDistance < vertHitDistance) ? horzWallHitX : vertWallHitX;
        this.wallHitY = (horzHitDistance < vertHitDistance) ? horzWallHitY : vertWallHitY;
        this.distance = (horzHitDistance < vertHitDistance) ? horzHitDistance : vertHitDistance;
        this.wasHitVertical = (vertHitDistance < horzHitDistance);        



    }

    render()
    {
        stroke("rgba(255, 0, 0, 0.05)");
        line(
            MINIMAP_SCALE_FACTOR * player.x, 
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * this.wallHitX,
            MINIMAP_SCALE_FACTOR * this.wallHitY
        );
    }
}

var grid = new Map();
var player = new Player();
var rays = [];

function normalizeAngle(angle)
{
    angle = angle % ( 2 * Math.PI );
    if(angle < 0)
    {
        //2*pi would reset the angle to the start (or end, depending of your preference) position
        // but that might translate into a hiccup
        // by doing (2*pi) + angle  the transition is seamless
        angle = ( 2 * Math.PI ) + angle;
    }

    return angle;
}

function keyPressed() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = +1;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = -1;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = +1;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = -1;
    }
}

function keyReleased() {
    if (keyCode == UP_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == DOWN_ARROW) {
        player.walkDirection = 0;
    } else if (keyCode == RIGHT_ARROW) {
        player.turnDirection = 0;
    } else if (keyCode == LEFT_ARROW) {
        player.turnDirection = 0;
    }
}

function castAllRays()
{
    var columnId = 0;
    // start first ray subtracting half of the FOV
    var rayAngle = player.rotationAngle - (FOV_ANGLE / 2); 

    rays = [];
    // loop all columns casting the rays
    for(var i = 0; i < NUM_RAYS ; i++)
    {
        var ray = new Ray(rayAngle);
        ray.cast(columnId);
        rays.push(ray);

        rayAngle += FOV_ANGLE / NUM_RAYS;

        columnId++;
    }
}

function render3DProjectedWalls()
{
    // loop every ray in the array of rays
    for(var i = 0; i < NUM_RAYS ; i++)
    {
        var ray = rays[i];

        var rayDistance = ray.distance;

        // calculate the distance to the projection plane
        //   note: we are trying to calculate the distance or the adjacent side of a 90deg triangle
        //    we have the angle and we have the opposite size (WINDOW_WIDTH / 2), so the formula is:
        //    adj = opp/tan(x)
        var distanceProjectionPlane = (WINDOW_WIDTH / 2) / Math.tan(FOV_ANGLE / 2);

        // projected wall height
        //  the equality formula is: 'actual wall height'/'distance to wall' = 'projected wall height'/'distance form player to proj. plane'
        //  since we want to find 'projected wall height', we reorganize the equation to:
        //   'projected wall height' = 'actual wall height'/'distance to wall' * 'distance form player to proj. plane'
        var wallStripHeight = (TILE_SIZE / rayDistance) * distanceProjectionPlane;

        fill("rgba(255, 255, 255, 1.0)");
        noStroke();
        // rect(x_start, y_start, width, height)
        //  note: for the y_start, we want to place the rectangle aligned with the center of the projection
        //   so that would be:  A = WINDOW_HEIGHT / 2    , but the rectangle also needs to be centered, so to find
        //   its center we do: B = wallStripHeight / 2   , now we need to do  C = A - B
        //   for instance suppose you have a WINDOW_HEIGHT = 1000px, and wallStripHeight = 300px
        //   you would have (1000/2) - (300/2) = 500 - 150 = 350. 
        //   That means, from the Y axis we would have 350 blank pixels, draw 300, then another 350 blank pixels
        //   resulting in the 'strip' aligned with the center of the screen

        rect(
            i * WALL_STRIP_WIDTH,
            (WINDOW_HEIGHT / 2) - (wallStripHeight / 2),
            WALL_STRIP_WIDTH,
            wallStripHeight
        );


    }
}

function distanceBetweenPoints(x1, y1, x2, y2)
{
    return Math.sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

function setup() 
{
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function update() 
{
    player.update();
    castAllRays();
}

function draw() 
{
    clear("#212121");
    update();

    render3DProjectedWalls();

    grid.render();
    for(ray of rays)
    {
        ray.render();
    }
    player.render();
}
