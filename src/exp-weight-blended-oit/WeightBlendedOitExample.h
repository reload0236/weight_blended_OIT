#pragma once

#include <memory>
#include <string>
#include <vector>
#include "vapp.h"
#include "SceneCreator.h"
#include "VBObjectExt.h"

BEGIN_APP_DECLARATION(WeightBlendedOitExample)

public:
    // Override functions from base class
    void Initialize(const char* title) override;
    void Display(bool auto_redraw) override;
    void Finalize(void) override;
    void Resize(int width, int height) override;
    void OnKey(int key, int scancode, int action, int mods) override;

    bool CreateOpaqueProgram();
    bool CreateTranslucentProgram();
    bool CreateOITProgram();
    bool CreateOITOverlayProgram();
    bool CreateRenderViewProgram();

    bool CreateMainFrameBuffer();
    bool CreateOITFrameBuffer();

    void RenderOpaque(float time);
    /// Render translucent in triditional painter's algorithom.
    void RenderTranslucent(float time);
    void RenderWeightBlendedOIT(float time);

protected:
    bool CreateFullScreenVAO();

    void RenderOITQuad(float time);
    void RenderViewQuad();

    void SortBackToFront(std::vector<std::shared_ptr<VBObjectExt>>& objects);

    void HandleCameraTransform(int key);

private:

    // Main FBO and attachments.
    GLuint main_fbo{};
    GLuint main_color_texture{};
    GLuint main_depth_rbo{};

    // OIT FBO and attachments.
    GLuint oit_fbo{};
    GLuint accum_texture{};
    GLuint reveal_texture{};

    /// Opaque objects rendering program.
    GLuint opaque_prog{};
    /// The program renders translucent object in a regular way.
    GLuint translucent_prog{};
    /// The program renders translucent object by OIT strategy.
    GLuint oit_output_prog{};
    /// The program renders OIT result into full screen quad.
    GLuint oit_overlay_prog{};
    /// The program renders into display view.
    GLuint render_view_prog{};

    GLuint fullscreen_quad_vao{};
    GLuint fullscreen_quad_vbo{};

    vmath::mat4 view_matrix;
    vmath::mat4 proj_matrix;

    std::vector<std::shared_ptr<VBObjectExt>> opaque_objects;
    std::vector<std::shared_ptr<VBObjectExt>> translucent_objects;

    SceneCreator scene_creator;

    float near_dist = .1f;
    float far_dist = 1000.0f;
    vmath::vecN<float, 3> eye_pos;
    vmath::vecN<float, 3> focal_point;
    vmath::vecN<float, 3> view_up;
    float camera_move_speed = 10.0f;
    float camera_rotate_speed = 3.0f;

    int window_width = 800;
    int window_height = 600;

    bool oit_active = true;
    bool transparent_active = true;

    unsigned last_time = 0;
    bool paused = false;
    bool force_redraw = false;

    static const std::string render_vs;
    static const std::string render_fs;

    static const std::string oit_output_fs;

    static const std::string fullscreen_quad_vs;
    static const std::string oit_quad_fs;

    static const std::string render_view_fs;

END_APP_DECLARATION()