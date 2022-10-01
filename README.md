# BubbleTrouble
## CS 101 : Course Project

This project was the *final course project* in CS 101 : Computer Programming and Utilization. The game implemented is **Bubble Trouble**, a popular game where a shooter has to pop bubbles that move under gravity. Bigger bubbles may pop into two. 

This was implemented in `C++` using the [`simplecpp`](https://www.cse.iitb.ac.in/~ranade/simplecpp/) graphics library. 

Features Implemented:

1. Parabolic motion of the bubble, collision with ground.
2. Collision between bubble and bullet.
3. Collision between shooter and bubble.
4. Using bubbles of different sizes in other levels. The larger bubbles split 
   into bubbles of half the radius when hit.
5. Score, countdown timer, Health and bullet ammunition.
6. Different levels with changing number, colour of bubbles. In levels after 2 there will be 
   bubbles of double radius. Also another parameter called 'difficulty' was implemented. Each level 
   of the game can be played in 3 different difficulties. Difficulty 1 (easy) has no obstacles, 
   difficulty 2 (medium) has 1 obstacle and difficulty 3 (hard) has 2 obstacles.
