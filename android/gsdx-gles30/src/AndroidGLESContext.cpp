#include "gsdx/AndroidGLESContext.h"
#include <GLES3/gl3.h>
#include <sstream>
namespace gsdx::android {
namespace {
void SetError(std::string* out, const char* operation, EGLint code) {
    if (!out) return;
    std::ostringstream stream;
    stream << operation << " failed (EGL error 0x" << std::hex << code << ")";
    *out = stream.str();
}
}
AndroidGLESContext::~AndroidGLESContext() { Destroy(); }
bool AndroidGLESContext::Create(ANativeWindow* window, std::string* error) {
    Destroy();
    if (!window) { if (error) *error = "ANativeWindow is null"; return false; }
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) { SetError(error, "eglGetDisplay", eglGetError()); Destroy(); return false; }
    EGLint major = 0, minor = 0;
    if (eglInitialize(m_display, &major, &minor) != EGL_TRUE) { SetError(error, "eglInitialize", eglGetError()); Destroy(); return false; }
    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) { SetError(error, "eglBindAPI", eglGetError()); Destroy(); return false; }
    const EGLint configAttrs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE
    };
    EGLint count = 0;
    if (eglChooseConfig(m_display, configAttrs, &m_config, 1, &count) != EGL_TRUE || count != 1) {
        SetError(error, "eglChooseConfig(ES3)", eglGetError()); Destroy(); return false;
    }
    const EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    m_context = eglCreateContext(m_display, m_config, EGL_NO_CONTEXT, contextAttrs);
    if (m_context == EGL_NO_CONTEXT) { SetError(error, "eglCreateContext(ES3)", eglGetError()); Destroy(); return false; }
    m_surface = eglCreateWindowSurface(m_display, m_config, window, nullptr);
    if (m_surface == EGL_NO_SURFACE) { SetError(error, "eglCreateWindowSurface", eglGetError()); Destroy(); return false; }
    if (!MakeCurrent(error)) { Destroy(); return false; }
    if (!glGetString(GL_VERSION)) { if (error) *error = "No GL_VERSION after creating GLES 3 context"; Destroy(); return false; }
    return true;
}
bool AndroidGLESContext::MakeCurrent(std::string* error) const {
    if (!IsCreated() || m_surface == EGL_NO_SURFACE || m_context == EGL_NO_CONTEXT) {
        if (error) *error = "EGL context has not been created"; return false;
    }
    if (eglMakeCurrent(m_display, m_surface, m_surface, m_context) != EGL_TRUE) {
        SetError(error, "eglMakeCurrent", eglGetError()); return false;
    }
    return true;
}
bool AndroidGLESContext::SwapBuffers(std::string* error) const {
    if (m_display == EGL_NO_DISPLAY || m_surface == EGL_NO_SURFACE) {
        if (error) *error = "EGL window surface has not been created"; return false;
    }
    if (eglSwapBuffers(m_display, m_surface) != EGL_TRUE) {
        SetError(error, "eglSwapBuffers", eglGetError()); return false;
    }
    return true;
}
void AndroidGLESContext::Destroy() {
    if (m_display != EGL_NO_DISPLAY) {
        if (eglGetCurrentContext() == m_context)
            eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_surface != EGL_NO_SURFACE) eglDestroySurface(m_display, m_surface);
        if (m_context != EGL_NO_CONTEXT) eglDestroyContext(m_display, m_context);
        eglTerminate(m_display);
    }
    m_surface = EGL_NO_SURFACE; m_context = EGL_NO_CONTEXT;
    m_config = nullptr; m_display = EGL_NO_DISPLAY;
}
} // namespace gsdx::android