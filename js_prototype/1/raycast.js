const TILE_SIZE = 32;
const MAP_NUM_ROWS = 11;
const MAP_NUM_COLS = 15;

const WINDOW_WIDTH = MAP_NUM_COLS * TILE_SIZE;
const WINDOW_HEIGHT = MAP_NUM_ROWS * TILE_SIZE;

class Map 
{
    constructor() 
    {
        // console.log("Map.constructor");
        this.grid = 
        [
            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1], // 0
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1], // 1
            [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1], // 2
            [1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1], // 3
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
    render() 
    {
        // console.log("Map.render");
        for (var i = 0; i < MAP_NUM_ROWS; i++) 
        {
            for (var j = 0; j < MAP_NUM_COLS; j++) 
            {
                var tileX = j * TILE_SIZE; 
                var tileY = i * TILE_SIZE;
                var tileColor = this.grid[i][j] == 1 ? "#222" : "#fff";
                stroke("#222");
                fill(tileColor);
                rect(tileX, tileY, TILE_SIZE, TILE_SIZE);
            }
        }
    }
}

class Player
{
    constructor()
    {
        this.x = WINDOW_WIDTH / 2;
        this.y = WINDOW_HEIGHT / 2;
        this.radius = 3;
        this.turnDirection = 0; // -1 if left, +1 if right
        this.walkDirection = 0; // -1 if back, +1 if front
        this.rotationAngle = Math.PI / 2; // looking 90 deg, but since the screen start left to right top to bottom, it will point down
        this.moveSpeed = 2.0;
        this.rotationSpeed = 2 * (Math.PI / 180); // 2 * 0.0174 = 0.0349

    }
    update()
    {
        this.rotationAngle += this.turnDirection * this.rotationSpeed; //( +1 or -1 ) * rotationSpeed
        var moveStep = this.walkDirection * this.moveSpeed; //( +1 or -1 ) * moveSpeed

        this.x = this.x + Math.cos(this.rotationAngle) * moveStep;
        this.y = this.y + Math.sin(this.rotationAngle) * moveStep;
    }
    render()
    {
        noStroke();
        fill("red");
        circle(this.x, this.y, this.radius);
        stroke("red");
        line(this.x, 
             this.y, 
             this.x + Math.cos(this.rotationAngle) * 20,
             this.y + Math.sin(this.rotationAngle) * 20
        );
    }
}

var grid = new Map();
var player = new Player();

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

function setup() 
{
    // console.log("setup");
    createCanvas(WINDOW_WIDTH, WINDOW_HEIGHT);

}

function update() 
{
    player.update();
}

function draw() 
{
    // console.log("draw");
    update();

    grid.render();
    player.render();
}
