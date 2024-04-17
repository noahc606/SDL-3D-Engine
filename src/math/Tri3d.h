#include "Vec3d.h"
#include "Poly3d.h"

class Tri3d : public Poly3d {
public:
    Tri3d(const Vec3d& p0, const Vec3d& p1, const Vec3d& p2);
    Tri3d(double coords[9]);

    Tri3d& operator=(const Poly3d& other);
    Vec3d normal();
private:
};