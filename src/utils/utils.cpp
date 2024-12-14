#include "utils.h"

namespace utils
{
    float getZFromXY(vec2 xy)
    {
        float length = xy.x * xy.x + xy.y * xy.y;
        if (length <= 1.0f)
        {
            return sqrt(1 - length);
        }
        else
        {
            return 0.0f;
        }
    }

    mat3 qvec2rotmat(const vec4 &qvec)
    {
        double x = qvec.x;
        double y = qvec.y;
        double z = qvec.z;
        double w = qvec.w;

        return mat3(
            1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z, 2.0 * z * x + 2.0 * w * y,
            2.0 * x * y + 2.0 * w * z, 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z - 2.0 * w * x,
            2.0 * z * x - 2.0 * w * y, 2.0 * y * z + 2.0 * w * x, 1.0 - 2.0 * x * x - 2.0 * y * y);
    }

    vec4 rotmat2qvec(const mat3 &R)
    {
        double m00 = R[0][0], m10 = R[1][0], m20 = R[2][0];
        double m01 = R[0][1], m11 = R[1][1], m21 = R[2][1];
        double m02 = R[0][2], m12 = R[1][2], m22 = R[2][2];

        mat4 K(
            m00 - m11 - m22, m10 + m01, m20 + m02, m12 - m21,
            m10 + m01, m11 - m00 - m22, m21 + m12, m20 - m02,
            m20 + m02, m21 + m12, m22 - m00 - m11, m01 - m10,
            m12 - m21, m20 - m02, m01 - m10, m00 + m11 + m22);
        K *= 1.0 / 3.0;
        vec4 q(1.0, 0.0, 0.0, 0.0); // Initial guess
        for (int iter = 0; iter < 10; iter++)
        { // Usually converges in a few iterations
            vec4 q_next = K * q;
            double norm = sqrt(dot(q_next, q_next));
            if (norm < 1e-10)
                break;
            q = q_next / norm;
        }

        // Ensure w is positive
        if (q[0] < 0)
        {
            q = -q;
        }

        // Normalize quaternion
        double norm = sqrt(dot(q, q));
        if (norm > 0)
        {
            q /= norm;
        }

        return q;
    }
    // Helper function to convert vector<double> to vec4
    vec4 vector_to_vec4(const std::vector<double> &v)
    {
        if (v.size() != 4)
        {
            throw std::runtime_error("Vector size must be 4");
        }
        return vec4(v[0], v[1], v[2], v[3]);
    }

    // Helper function to convert vector<double> to vec3
    vec3 vector_to_vec3(const std::vector<double> &v)
    {
        if (v.size() != 3)
        {
            throw std::runtime_error("Vector size must be 3");
        }
        return vec3(v[0], v[1], v[2]);
    }

    double fov2focal(double fov, double pixels)
    {
        return pixels / (2.0 * tan(fov / 2.0));
    }

    double focal2fov(double focal, double pixels)
    {
        return 2 * atan(pixels / (2.0 * focal));
    }
}
