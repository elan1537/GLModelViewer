#pragma once
#include <glad/glad.h>

namespace TrainPathWindow
{
    struct ImageControl
    {
        bool next;
        bool prev;
    };

    ImageControl Draw(GLuint texID, int img_w, int img_h);
}