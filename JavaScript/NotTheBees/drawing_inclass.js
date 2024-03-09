//Javascript file
"use strict"

//only gets first element 
let canvas = document.getElementById("canvasDrawing");
let context = canvas.getContext("2d")

//Grabbing width and height
canvas.width = window.innerWidth ;
canvas.height = window.innerHeight;

let cWidth = canvas.width;
let cHeight = canvas.height;

//boolean for game over
let gameEnd = false;



//background image
let backgroundImage = new Image();
backgroundImage.src = "forest.jpg";
backgroundImage.width = window.width;
backgroundImage.height = window.height;

//Game over image
let gameOverImage = new Image();
gameOverImage.src = "caught1.jpg";
gameOverImage.width = window.width;
gameOverImage.height = window.height;

//image for pokeball to be chased
let pokeballImage = new Image();

//Pull in file from working directory 
pokeballImage.src = "ultraball.png";
pokeballImage.width = 100;
pokeballImage.height = 100;

//Initialize array to store bees
let beesArray = [];

function generateBee() {
    let bee = new Image();
    bee.src = "bee.png";
    bee.width = 50;
    bee.height = 50;
    bee.xPos = Math.random() * (cWidth - bee.width);
    bee.yPos = Math.random() * (cHeight - bee.height);
    beesArray.push(bee);
    
}

//Initialize bees 
for ( let i = 0; i < 6; i++ ){
   generateBee();
}


//Draw item to be chased 
context.drawImage(pokeballImage, 20, 20, 100, 100);


function animateImg() {

    eraseOld();

    // Drawing pokeball image
    context.drawImage(pokeballImage, pokeballImage.xPos, pokeballImage.yPos, pokeballImage.width, pokeballImage.height);

    // Draw array of bees
    for (let bee of beesArray) {
        context.drawImage(bee, bee.xPos, bee.yPos, 65, 65);
    }

    // Check for collisions and make bees track
    for (let bee of beesArray) {

        checkCollision(pokeballImage, bee);

        if ( pokeballImage.xPos > bee.xPos ){
                    bee.xPos += 2; 
        }
        if ( pokeballImage.yPos > bee.yPos ){
            bee.yPos += 2; 
        }
        if ( pokeballImage.xPos < bee.xPos ){
            bee.xPos -= 2; 
        }
        if ( pokeballImage.yPos < bee.yPos ){
            bee.yPos -= 2; 
        }
        
    }

    // If game is not over, call the animation frame again
    if (!gameEnd) {
        window.requestAnimationFrame(animateImg);
    }
}

function checkCollision(obj1, obj2) {
    if (
        obj1.xPos < obj2.xPos + obj2.width &&
        obj1.xPos + obj1.width > obj2.xPos &&
        obj1.yPos < obj2.yPos + obj2.height &&
        obj1.yPos + obj1.height > obj2.yPos
    ) {
        gameOver();
    }
}

function mainDrawing(){

  //background music 
  document.getElementById('backgroundMusic').play();




  //debug
  console.log("main", cHeight, cWidth);

  //call animate which calls it once 
  window.requestAnimationFrame(animateImg);
  
}

function handleClick (e){
    //update the x and y pos of the pokeball
    pokeballImage.xPos = e.x - 3 - (pokeballImage.width /2);
    pokeballImage.yPos = e.y - 3 - (pokeballImage.height /2);
} 

function eraseOld(){
    //add color to window
    context.fillStyle = "#000000";
    context.fillRect(0,0,cWidth,cHeight);

    context.drawImage(backgroundImage, 0,0, cWidth, cHeight)
}

function gameOver(){
    context.drawImage(gameOverImage, 0,0, cWidth, cHeight)
    gameEnd = true;
    document.getElementById('backgroundMusic').pause();
    document.getElementById('gameOverMusic').play();
}




//animate 
window.onload = mainDrawing;

//Option 2 handle click follows mouse movement
document.onmousemove = handleClick;