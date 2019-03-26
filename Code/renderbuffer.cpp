#include "renderbuffer.h"

Renderbuffer::Renderbuffer() {
    initializeOpenGLFunctions();
    glGenRenderbuffers(1, &renderbufferID);
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &renderbufferID);
}
