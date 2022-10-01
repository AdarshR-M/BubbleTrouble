#include <simplecpp>

class Obstacle{

private :
    Rectangle rect;
    Color color;

public :
        Obstacle(double center_x, double center_y, double width, double height, Color color){
        rect = Rectangle(center_x, center_y, width, height);
        rect.setColor(color);
        rect.setFill(true);
    }
    double get_width()
    {
        // return the width of the bullet
        return rect.getWidth();
    }

    double get_height()
    {
        // return the height of the bullet
        return rect.getHeight();
    }

    double get_center_x()
    {
        // return the x coordinate of the center of the bullet
        return rect.getX();
    }

    double get_center_y()
    {
        // return the y coordinate of the center of the bullet
        return rect.getY();
    }

};
