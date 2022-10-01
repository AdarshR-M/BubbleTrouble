#include <simplecpp>
#include "shooter.h"
#include "bubble.h"
#include "obstacle.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);

// _________ initializing required variables ____________________________________________
int Score = 0, prevScore = 0;       // +10 points each time you destroy a bubble
int Level;           // Level of the game.
int health, prevhealth;
double time = 0; int Time, prevTime;
int Game = 0; // Tells the state of game. 0 - in progress, 1 - win, 2 - lose because of time, 3 - lose because of health.
int difficulty; // difficulty 1-3
int Ammo = 5, prevAmmo = 5;   // Ammunition for the bullets
// _____________________________________________________________________________________


//______________________________________________________________________________________

int Health(int Level){           // Health function. Takes in Level and gives the total initial health count.
    return (Level + 3)/3;
}

// ______________________________________________________________________________________

int TIME(int Level){
    if (Level <= 3){
        return 60;
    }
    else {
        return 60 +10*(Level-3);
    }
}

// ______________________________________________________________________________________

void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

// ______________________________________________________________________________________

void Collision_Bullet_Bubble(vector<Bullet> &bullets, vector<Bubble> &bubbles){
                                                                                  // This function checks for collision between
    for (unsigned int i=0; i < bubbles.size(); i++){                              // the bullet and the bubble, updates the score
        for (unsigned int j=0; j < bullets.size(); j++){                          // and erases vector objects, if collision occurs.
            if (abs(bullets[j].get_center_x()-bubbles[i].get_center_x()) <= bubbles[i].get_radius()+bullets[j].get_width()/2+0.1
            // checking for collision in x coordinates
            &&
             abs(bullets[j].get_center_y()-bubbles[i].get_center_y()) <= bubbles[i].get_radius()+bullets[j].get_height()/2+0.1
             // checking for collision in y coordinates
             ){
                if (bubbles[i].get_radius() <= BUBBLE_DEFAULT_RADIUS + 1){
                    bullets.erase(bullets.begin()+j);       // erasing the colliding bullet from the bullets vector
                    bubbles.erase(bubbles.begin()+i);       // erasing the colliding bubble from the bubbles vector
                    prevScore = Score;
                    Score += 10;                            // Score increases by 10 each time a collision happens
                    prevAmmo = Ammo;
                    Ammo += 5;                              // We get 5 more bullets each time we split/destroy a bullet
                }
                else if (bubbles[i].get_radius() > BUBBLE_DEFAULT_RADIUS + 1){
                    bullets.erase(bullets.begin()+j);
                    bubbles.push_back(Bubble(bubbles[i].get_center_x()-BUBBLE_DEFAULT_RADIUS,
                     bubbles[i].get_center_y(),BUBBLE_DEFAULT_RADIUS,
                    -BUBBLE_DEFAULT_VX, 50, COLOR(255,105,180)));
                    bubbles.push_back(Bubble(bubbles[i].get_center_x()+BUBBLE_DEFAULT_RADIUS,
                     bubbles[i].get_center_y(),BUBBLE_DEFAULT_RADIUS,
                    BUBBLE_DEFAULT_VX, 50, COLOR(255,105,180)));
                    bubbles.erase(bubbles.begin()+i);
                    prevScore = Score;
                    Score += 10;
                    prevAmmo = Ammo;
                    Ammo += 5;

                }

             }
        }
    }
}

// _____________________________________________________________________________________

void Collision_Shooter_Bubble(Shooter &shooter, vector<Bubble> &bubbles){
    for (unsigned int i=0; i < bubbles.size(); i++){
        if (((bubbles[i].get_center_x() > shooter.get_head_center_x() - bubbles[i].get_radius() -shooter.get_body_width()/2)
        &&(bubbles[i].get_center_x() < shooter.get_head_center_x() + bubbles[i].get_radius() + shooter.get_body_width()/2))
        && ((bubbles[i].get_center_y() > shooter.get_head_center_y() - shooter.get_head_radius() - bubbles[i].get_radius())
        && (bubbles[i].get_center_y() < shooter.get_head_center_y()+shooter.get_head_radius()+bubbles[i].get_radius()+shooter.get_body_height())
        )){      // checking for collision of bubble with shooter.
                // Here we model shooter as a rectangle of width and height = height + 2*radius
            if (bubbles[i].get_center_y() > shooter.get_head_center_y() - shooter.get_head_radius()){
                bubbles[i].set_vx(-bubbles[i].get_vx()); // if collides with height of the shooter
                if (bubbles[i].get_center_x() < shooter.get_head_center_x()){
                    bubbles[i].set_center(shooter.get_head_center_x() - 2*bubbles[i].get_radius()
                    -shooter.get_body_width()/2, bubbles[i].get_center_y());    // This is to ensure that the bubble doesn't dance around
                    }                                                           // the shooter and freely escapes after rebounding.
                else {
                    bubbles[i].set_center(shooter.get_head_center_x() + 2*bubbles[i].get_radius()
                    +shooter.get_body_width()/2, bubbles[i].get_center_y());
                }
                prevhealth = health;
                health -= 1;
            }
            else {                                       // if collides with width of the shooter
                bubbles[i].set_vy(-bubbles[i].get_vy());
                bubbles[i].set_center(bubbles[i].get_center_x(), shooter.get_head_center_y()
                - shooter.get_head_radius() - 2*bubbles[i].get_radius());       // This is to ensure that the bubble doesn't dance around
                                                                                // the shooter and freely escapes after rebounding.
                prevhealth = health;
                health -= 1;
            }


        }
    }
}
// ____________________________________________________________________________________

void Collision_Obstacle_Bubble(vector<Obstacle> &obstacles, vector<Bubble> &bubbles){

    for (unsigned int i = 0; i < bubbles.size(); i++){
        for (unsigned int j = 0; j < obstacles.size(); j++){
            if ((bubbles[i].get_center_x() < obstacles[j].get_center_x() + obstacles[j].get_width()/2 + bubbles[i].get_radius()
            && bubbles[i].get_center_x() > obstacles[j].get_center_x() - obstacles[j].get_width()/2 - bubbles[i].get_radius())
            && (bubbles[i].get_center_y() < obstacles[j].get_center_y() + obstacles[j].get_height()/2 + bubbles[i].get_radius()
            && bubbles[i].get_center_y() > obstacles[j].get_center_y() - obstacles[j].get_height()/2 - bubbles[i].get_radius())){
                if (bubbles[i].get_center_x() < obstacles[j].get_center_x() + obstacles[j].get_width()/2
                && bubbles[i].get_center_x() > obstacles[j].get_center_x() - obstacles[j].get_width()/2){
                    bubbles[i].set_vy(-bubbles[i].get_vy());
                    if (bubbles[i].get_center_y() > obstacles[j].get_center_y()){
                        bubbles[i].set_center(bubbles[i].get_center_x(), obstacles[j].get_center_y()
                         + obstacles[j].get_height()/2 + 1.2*bubbles[i].get_radius());
                    }
                    else {
                        bubbles[i].set_center(bubbles[i].get_center_x(), obstacles[j].get_center_y()
                         - obstacles[j].get_height()/2 - 1.2*bubbles[i].get_radius());
                    }
                }
                else {
                    bubbles[i].set_vx(-bubbles[i].get_vx());
                    if (bubbles[i].get_center_x() > obstacles[j].get_center_x()){
                        bubbles[i].set_center( obstacles[j].get_center_x() + obstacles[j].get_width()/2
                        + 1.2*bubbles[i].get_radius(), bubbles[i].get_center_y());
                    }
                    else {
                        bubbles[i].set_center( obstacles[j].get_center_x() - obstacles[j].get_width()/2
                        - 1.2*bubbles[i].get_radius(), bubbles[i].get_center_y());
                    }
                }
            }
        }
    }
}

// ____________________________________________________________________________________
void Collision_Obstacle_Bullet(vector<Obstacle> &obstacles, vector<Bullet> &bullets){

    for (unsigned int i = 0; i < bullets.size(); i++){
        for (unsigned int j = 0; j < obstacles.size(); j++){
            if (abs(bullets[i].get_center_x()-obstacles[j].get_center_x()) < (bullets[i].get_width() + obstacles[j].get_width())/2
            &&abs(bullets[i].get_center_y()-obstacles[j].get_center_y()) < (bullets[i].get_height() + obstacles[j].get_height())/2)
                bullets.erase(bullets.begin()+i);
        }
    }
}


// _____________________________________________________________________________________

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

// _____________________________________________________________________________________

vector<Bubble> create_bubbles(int n)
{
    int small = (n+4)/2;
    int large = n/3;
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    for (int i = 0; i < small; i++){
        bubbles.push_back(Bubble((i+1)*WINDOW_X*1.0/(small+1), BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS,
        pow(-1, i)*BUBBLE_DEFAULT_VX, -pow(-1, i)*50, COLOR((25+150*n)%256,(105+50*n)%256,(180+200*n)%256)));
    }

    for (int i = 0; i < large; i++){
        bubbles.push_back(Bubble((i+1)*WINDOW_X*1.0/(large+1), BUBBLE_START_Y+100, 2*BUBBLE_DEFAULT_RADIUS,
        pow(-1, i)*BUBBLE_DEFAULT_VX*0.5, 50, COLOR((100+50*n)%256,(25+100*n)%256,(80-300*n)%256)));
    }

    return bubbles;
}
// ______________________________________________________________________________________
vector<Obstacle> create_obstacles(int difficulty){
    vector<Obstacle> obstacles;
    if (difficulty == 2){
        obstacles.push_back(Obstacle(250, 250, 100, 30, COLOR(255, 0, 0)));
    }
    if (difficulty == 3){
        obstacles.push_back(Obstacle(150, 250, 20, 150, COLOR(0, 0, 255)));
        obstacles.push_back(Obstacle(400, 150, 100, 30, COLOR(0, 255, 0)));
    }
    return obstacles;
}
// ______________________________________________________________________________________

int main()
{

    cout << "Welcome to the game BUBBLE TROUBLE!!" << endl;
    cout << "Rules:" << endl;
    cout << "You have to destroy as many bubbles as you can before the count down ends using the bullets." << endl;
    cout << "Beware! The bubbles can destroy you, try to evade them." << endl;
    cout << "Press 'a' - to move left, 'd' - to move right, 'w' - to shoot and 'q' - to quit the game." << endl;
    cout << "Are you ready..." << endl;
    cout << "Enter the level you want to start with:" << endl;
    cin >> Level;
    cout << "Enter the difficulty: " << endl;
    cin >> difficulty;
    health = Health(Level);             // Initializing health based on the level.

    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    XEvent event;

    while (true){
    {
    Text HEADING(240, 250, "Level"), HEADINGV(270, 250, Level);
    Text HEADING1(250, 270, "Let us Begin!!");
    HEADING.setColor(COLOR(255, 0, 0)); HEADINGV.setColor(COLOR(255, 0, 0));
    wait(2);
    }
    {
// _________ Initial Score, health, time displays _________
    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);      // Keyboard command

    Text SCORE(WINDOW_X/2,BOTTOM_MARGIN, "Score: "), SCOREV(WINDOW_X/2+27,BOTTOM_MARGIN,0);             // Score

    Text HEALTH(WINDOW_X-100,BOTTOM_MARGIN, "Health: "), HEALTHV(WINDOW_X-70,BOTTOM_MARGIN, Health(Level));    // health

    Text Countdown(40,10, "Time Left :"), CountdownV(85, 10, TIME(Level));      // time

    Text AMMO(440, 10, "Ammo :" ), AMMOV(470 ,10, Ammo);    // Ammo
// ________________________________________________________


    // Generate the bubbles
    vector<Bubble> bubbles = create_bubbles(Level);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    // Initialize the obstacles
    vector<Obstacle> obstacles = create_obstacles(difficulty);


    // Main game loop
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w'){
                bullets.push_back(shooter.shoot());
                prevAmmo = Ammo;
                Ammo -= 1;
            }
            else if(c == 'q')
                return 0;
        }


        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        // ________ check for collisions ________________
        Collision_Shooter_Bubble(shooter, bubbles);
        Collision_Bullet_Bubble(bullets, bubbles);
        Collision_Obstacle_Bubble(obstacles, bubbles);
        Collision_Obstacle_Bullet(obstacles, bullets);


        // ____ update time, score, health________________
        if (health-prevhealth != 0)           // updates the health if it changes.
            HEALTHV.reset(WINDOW_X-70,BOTTOM_MARGIN, health);
        if (Score-prevScore > 0)              // updates the score if it changes.
            SCOREV.reset(WINDOW_X/2+27,BOTTOM_MARGIN, Score);
        prevTime = Time;
        time += STEP_TIME;
        Time = time*(2+Level/3);
        Time = TIME(Level) - Time;
        if (prevTime-Time!=0){                 // updates the countdown timer
            CountdownV.reset(90,10, Time);
        }
        if (Ammo - prevAmmo != 0){              // updates the Ammo
            AMMOV.reset(470 ,10, Ammo);
        }



        // _____ Check whether the game is finished or not _____

        if (health <= 0) {
            Game = 3;
            break;
        }

        else if (Time <= 0){
            Game = 2;
            break;
        }
        else if (Score == ((Level+4)/2)*10 + (Level/3)*30){
            Game = 1;
            break;
        }
        else if (Ammo <= 0){
            Game = 4;
            break;
        }
        wait(STEP_TIME);
    }
    };

    bool pending;


    if (Game == 2){
        Text Comment(250, 250, "PLAY FASTER!");
        wait(2);
    }
    if (Game == 3){
        Text Comment(250, 250, "Take the Shooter to a Hospital!");
        wait(2);
    }
    if (Game == 4){
        Text Comment(250, 250, "The bullets are costly! Don't waste them.");
        wait(2);
    }


    if (Game == 1){
        Text Result(250, 250, "You Win!!!");
        Result.setColor(COLOR(255, 0, 0));
        Text Next1(250, 270, "Press 'a' to proceed to the next level");
        Text Next2(250, 290, "Press 'q' to exit");
        while (true){
            pending = checkEvent(event);
            if (pending){
                char c = charFromEvent(event);
                if (c == 'q'){
                return 0;
                }
                else if (c == 'a'){
                    Level += 1;
                    break;
                }
            }
        }
    }
    else if (Game == 2 || Game == 3 || Game == 4){
        Text Result(250, 250, "You Lose!!!");
        Result.setColor(COLOR(255, 0, 0));
        Text Next1(250, 270, "Press 'a' to play again");
        Text Next2(250, 290, "Press 'q' to exit");
        while (true){
            pending = checkEvent(event);
            if (pending){
                char c = charFromEvent(event);
                if (c == 'q'){
                    return 0;
                }
                else if (c == 'a'){
                    break;
                }
            }
        }
    }
    time = 0;
    Time = prevTime = TIME(Level);
    Score = prevScore = 0;
    health = prevhealth = Health(Level);
    Ammo = prevAmmo = 5;

}
}

