#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(7, 0, 0);
const Color ORANGE(7, 4, 0);
const Color YELLOW(7, 7, 0);
const Color GREEN(0, 7, 0);
const Color BLUE(0, 0, 7);
const Color PURPLE(7, 0, 7);
const Color WHITE(7, 7, 7);
const Color LIME(4, 7, 0);
const Color AQUA(0, 7, 7);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      y++;
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      switch(strength){
      case 1: draw_with_rgb(RED, AQUA);
      break;
      case 2: draw_with_rgb(ORANGE, AQUA);
      break;
      case 3: draw_with_rgb(YELLOW, AQUA);
      break;
      case 4: draw_with_rgb(GREEN, AQUA);
      break;
      case 5: draw_with_rgb(BLUE, AQUA);
      break;
      case 6: draw_with_rgb(PURPLE, AQUA);
      break;
      case 7: draw_with_rgb(WHITE, AQUA);
      break;
      default:
      draw_with_rgb(BLACK, BLACK);
      }
      return;
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
      return;
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      if(strength > 1){
        strength--;
        draw();
      }
      if(strength == 1){
        strength--;
        erase();
      }
      return;
    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      for(int i = x; i < x + 4; i++){
        for(int j = y; j < y + 4; j++){
          matrix.drawPixel(i, j, body_color.to_333());
        }
      }
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x + 3, y, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 3, BLACK.to_333());
      matrix.drawPixel(x + 2, y + 3, BLACK.to_333());
      matrix.drawPixel(x + 1, y + 1, eye_color.to_333());
      matrix.drawPixel(x + 2, y + 1, eye_color.to_333());
      return;
    }
};

class Cannonball {
  public:
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    void fire(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      fired = true;
      return;
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    void move() {
      y--;
      if(y < 0){
        fired = false;
      }
      return;
    }
    
    // resets private data members to initial values
    void hit() {
      fired = false;
      return;
    }
    
    // draws the Cannonball, if it is fired
    void draw() {
      if(fired){
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x, y - 1, ORANGE.to_333());
        matrix.drawPixel(x, y - 2, ORANGE.to_333());        
      }
      return;
    }
    
    // draws black where the Cannonball used to be
    void erase() {
        matrix.drawPixel(x, y, BLACK.to_333());
        matrix.drawPixel(x, y - 1, BLACK.to_333());
        matrix.drawPixel(x, y - 2, BLACK.to_333());  
    }

  private:
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      x = 0;
      y = 0;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      x = x_arg;
    }
    
    // Modifies: lives
    void die() {
      lives--;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      initialize(16, 15);
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
      return;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
       matrix.drawPixel(x, 15, color.to_333());
       matrix.drawPixel(x, 14, color.to_333());       
       matrix.drawPixel(x - 1, 15, color.to_333());
       matrix.drawPixel(x + 1, 15, color.to_333());
    }
};

class Game {
  public:
    Game() {
      level = 0;
      time = 0;
    }
    
    // returns enemy at specified index, if in bounds
    Invader get_enemy(int index) {
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    // see http://arduino.cc/en/Reference/Setup
    void setup() {
         for (int i = 0; i < NUM_ENEMIES; i++){
             enemies[i].initialize(4 * i % 32, i / 8 * 4, 2);
             enemies[i].draw();
          }       
    }
    
    // displays and runs Space Invaders!
    // see spec for details of game
    // Modifies: global variable matrix
    void loop(int potentiometer_value, bool button_pressed) {
         player.draw();
         player.set_x(potentiometer_value);
         cannon.reset();
         while (!cannon.has_been_fired()) {
            if (button_pressed) {
                cannon.fire(player.get_x(),15);
                cannon.draw();
                player.draw();
                while (cannon.has_been_fired()){
                  delay(200);
                  cannon.erase();
                  cannon.move();
                  cannon.draw();
                  player.draw();
              }
            }
    }
}
  private:
    int level;
    int time;
    Player player;
    Cannonball cannon;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
    }

    // set up a level
    void reset_level() {
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see http://arduino.cc/en/Reference/Setup
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();

}

// see http://arduino.cc/en/Reference/Loop
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.setup();
  while(button_pressed){
    game.loop(potentiometer_value, button_pressed);
  }
  
}

// displays Level
void print_level(int level) {
}

// displays number of lives
void print_lives(int lives) {
}

// displays "game over"
void game_over() {
}


