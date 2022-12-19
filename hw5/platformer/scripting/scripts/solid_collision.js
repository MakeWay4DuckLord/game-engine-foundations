{

    let character_bounds = JSON.parse(character.shape);
    let obstacle_bounds = JSON.parse(obstacle.shape);

    if(character_bounds.y + character_bounds.height / 3 < obstacle_bounds.y) {
        //colliding from top
        character_bounds.y = obstacle_bounds.y - character_bounds.height;
        
    } else if (character_bounds.x < obstacle_bounds.x + obstacle_bounds.width && character_bounds.x + character_bounds.width > obstacle_bounds.x + obstacle_bounds.width) {
        //colliding from right
        character_bounds.x = obstacle_bounds.x + obstacle_bounds.width;
        
    } else if (character_bounds.x < obstacle_bounds.x) {
        //colliding from left
        character_bounds.x = obstacle_bounds.x - character_bounds.width;
    } else {
        //colliding from bottom
        character_bounds.y = obstacle_bounds.y + obstacle_bounds.height;
    }

    character.shape = JSON.stringify(character_bounds);
}
    
    