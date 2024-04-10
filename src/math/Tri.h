#include "Point3d.h"

class Tri {
public:
    Tri();
    Tri(double coords[9]);

    Tri& multiply4d(Matrix4d b);
    Tri& translate(const Point3d& p);
    Tri& scale(double xFactor, double yFactor, double zFactor);

    Point3d points[3];
private:
};