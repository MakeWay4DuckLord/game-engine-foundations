// character_bounds_bounds and obstacle_bounds

/*
    sf::FloatRect ourBounds = character_bounds->getShape()->getGlobalBounds();
    sf::FloatRect theirBounds = other->getBounds();

    //colliding from top
    if(ourBounds.top + ourBounds.height / 3 < theirBounds.top) {
        character_bounds->getShape()->setPosition(ourBounds.left,theirBounds.top - ourBounds.height);
    //colliding from right
    } else if(ourBounds.left < theirBounds.left + theirBounds.width && ourBounds.left + ourBounds.width > theirBounds.left + theirBounds.width) {
        character_bounds->getShape()->setPosition(theirBounds.left + theirBounds.width, ourBounds.top);
    //colliding from left    
    } else if(ourBounds.left < theirBounds.left ){
        character_bounds->getShape()->setPosition(theirBounds.left - ourBounds.width, ourBounds.top);
    } else {
        character_bounds->getShape()->setPosition(ourBounds.left, theirBounds.top + theirBounds.height);
    }
*/

{

    let character_bounds = JSON.parse(character.shape);
    let obstacle_bounds = JSON.parse(obstacle.shape);

    if(character_bounds.y + character_bounds.height / 3 < obstacle_bounds.y) {
        //colliding from top
        character_bounds.y = obstacle_bounds.y - character_bounds.height;
        print("top");
        
    } else if (character_bounds.x < obstacle_bounds.x + obstacle_bounds.width && character_bounds.x + character_bounds.width > obstacle_bounds.x + obstacle_bounds.width) {
        //colliding from right
        character_bounds.x = obstacle_bounds.x + character_bounds.width;
        print("right");
        
    } else if (character_bounds.x < obstacle_bounds.x) {
        //colliding from left
        character_bounds.x = obstacle_bounds.x - character_bounds.width;
        print("left");
    } else {
        character_bounds.y = obstacle_bounds.y + obstacle_bounds.height;
        print("bottom");
    }

    character.shape = JSON.stringify(character_bounds);
}
    
    