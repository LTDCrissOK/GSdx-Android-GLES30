#pragma once
#if !defined(__ANDROID__)
#error "AndroidGLESContext is only available on Android."
#endif
#include <EGL/egl.h>
#include <android/native_window.h>
#include <string>
namespace gsdx::android {
class AndroidGLESContext final {
public:
    AndroidGLESContext() = default;
    ~AndroidGLESContext();
    AndroidGLESContext(const AndroidGLESContext&) = delete;
    AndroidGLESContext& operator=(const AndroidGLESContext&) = delete;
    // The caller owns ANativeWindow and must keep it alive until Destroy().
    bool Create(ANativeWindow* window, std::string* error = nullptr);
    bool MakeCurrent(std::string* error = nullptr) const;
    bool SwapBuffers(std::string* error = nullptr) const;
    void Destroy();
    bool IsCreated() const noexcept { return m_display != EGL_NO_DISPLAY; }
    EGLDisplay Display() const noexcept { return m_display; }
    EGLContext Context() const noexcept { return m_context; }
    EGLSurface Surface() const noexcept { return m_surface; }
private:
    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLConfig m_config = nullptr;
    EGLContext m_context = EGL_NO_CONTEXT;
    EGLSurface m_surface = EGL_NO_SURFACE;
};
} // namespace gsdx::android