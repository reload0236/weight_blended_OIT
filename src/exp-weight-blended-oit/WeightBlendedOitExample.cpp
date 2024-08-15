#include <algorithm>
#include <stdio.h>

#include <vermilion.h>

#include "vapp.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"


#include "WeightBlendedOitExample.h"

DEFINE_APP(WeightBlendedOitExample, "WeightBlendedOitExample Example")

void WeightBlendedOitExample::Initialize(const char * title)
{
    base::Initialize(title);
    
    CreateMainFrameBuffer();

    CreateOITFrameBuffer();

    CreateOpaqueProgram();
    
    CreateTranslucentProgram();
    
    CreateOITProgram();
    CreateOITOverlayProgram();
    
    CreateRenderViewProgram();

    CreateFullScreenVAO();

    // Create test scene objects.
    scene_creator.CreateOpaqueObjects(opaque_objects);
    scene_creator.CreateTranslucentObjects(translucent_objects);

    // Setup camera pose.
    eye_pos[0] = .0f;
    eye_pos[1] = 400.0f;
    eye_pos[2] = 0.0f;
    focal_point[0] = .0f;
    focal_point[1] = .0f;
    focal_point[2] = .0f;
    view_up[0] = 1.0f;
    view_up[1] = .0f;
    view_up[2] = .0f;
}

bool WeightBlendedOitExample::CreateOpaqueProgram()
{
    opaque_prog = glCreateProgram();

    vglAttachShaderSource(opaque_prog, GL_VERTEX_SHADER, render_vs.c_str());
    vglAttachShaderSource(opaque_prog, GL_FRAGMENT_SHADER, render_fs.c_str());

    glLinkProgram(opaque_prog);

    return true;
}

bool WeightBlendedOitExample::CreateTranslucentProgram()
{
    translucent_prog = glCreateProgram();

    vglAttachShaderSource(translucent_prog, GL_VERTEX_SHADER, render_vs.c_str());
    vglAttachShaderSource(translucent_prog, GL_FRAGMENT_SHADER, render_fs.c_str());

    glLinkProgram(translucent_prog);

    return true;
}

bool WeightBlendedOitExample::CreateOITProgram()
{
    oit_output_prog = glCreateProgram();
    vglAttachShaderSource(oit_output_prog, GL_VERTEX_SHADER, render_vs.c_str());
    vglAttachShaderSource(oit_output_prog, GL_FRAGMENT_SHADER, oit_output_fs.c_str());

    glLinkProgram(oit_output_prog);

    return true;
}

bool WeightBlendedOitExample::CreateOITOverlayProgram()
{
    oit_overlay_prog = glCreateProgram();
    vglAttachShaderSource(oit_overlay_prog, GL_VERTEX_SHADER, fullscreen_quad_vs.c_str());
    vglAttachShaderSource(oit_overlay_prog, GL_FRAGMENT_SHADER, oit_quad_fs.c_str());

    glLinkProgram(oit_overlay_prog);

    return true;
}

bool WeightBlendedOitExample::CreateRenderViewProgram()
{
    render_view_prog = glCreateProgram();
    vglAttachShaderSource(render_view_prog, GL_VERTEX_SHADER, fullscreen_quad_vs.c_str());
    vglAttachShaderSource(render_view_prog, GL_FRAGMENT_SHADER, render_view_fs.c_str());

    glLinkProgram(render_view_prog);

    return true;
}

bool WeightBlendedOitExample::CreateFullScreenVAO()
{
    /// Fullscreen 'quad' geometry setup
    float fullscreen_quad_verts[] = {
        // left triangle
        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0,  -1.0, 0.0,

        // right triangle
        1.0,  -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0};

    if (!fullscreen_quad_vao) {
        glCreateVertexArrays( 1, &fullscreen_quad_vao );
        glCreateBuffers( 1, &fullscreen_quad_vbo );

        glNamedBufferData( fullscreen_quad_vbo, sizeof(fullscreen_quad_verts), fullscreen_quad_verts, GL_STATIC_DRAW );
    
        glVertexArrayVertexBuffer( fullscreen_quad_vao, 0, fullscreen_quad_vbo, 0, 3 * sizeof( float ) );
        glEnableVertexArrayAttrib( fullscreen_quad_vao, 0 );
        glVertexArrayAttribFormat( fullscreen_quad_vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
        glVertexArrayAttribBinding( fullscreen_quad_vao, 0, 0 );
    }

    return true;
}

bool WeightBlendedOitExample::CreateMainFrameBuffer()
{
    glGenFramebuffers(1, &main_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);

    // 创建并附加颜色纹理
    glGenTextures(1, &main_color_texture);
    glBindTexture(GL_TEXTURE_2D, main_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_color_texture, 0);

    // 创建并附加深度纹理（可选，取决于是否需要深度信息）
    //GLuint depthTexture;
    //glGenTextures(1, &depthTexture);
    //glBindTexture(GL_TEXTURE_2D, depthTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, window_width, window_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // Render buffer should be a better choise for non-readabldepth attachment.
    glGenRenderbuffers( 1, &main_depth_rbo );
    glBindRenderbuffer( GL_RENDERBUFFER, main_depth_rbo );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, main_depth_rbo );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf( "Framebuffer not complete!\n");
    }

    // unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

bool WeightBlendedOitExample::CreateOITFrameBuffer()
{
    if (oit_fbo) {
        glDeleteFramebuffers(1, &oit_fbo);
        oit_fbo = 0;
    }

    if (accum_texture) {
        glDeleteTextures(1, &accum_texture);
        accum_texture = 0;
    }

    if (reveal_texture) {
        glDeleteTextures(1, &reveal_texture);
        reveal_texture = 0;
    }

    glGenFramebuffers(1, &oit_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo );

    glGenTextures(1, &accum_texture);
    glBindTexture(GL_TEXTURE_2D, accum_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accum_texture, 0);

    glGenTextures(1, &reveal_texture);
    glBindTexture(GL_TEXTURE_2D, reveal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, window_width, window_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, reveal_texture, 0);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // Render buffer for depth attachment.
    //glGenRenderbuffers( 1, &rbo_depth );
    //glBindRenderbuffer( GL_RENDERBUFFER, rbo_depth );
    //glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height );
    //glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth );

    // Reuse the main FBO depth buffer.
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, main_depth_rbo );

    if( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
        printf( "Framebuffer not complete!\n");
        return false;
    }

    // recorver to default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void WeightBlendedOitExample::Display(bool auto_redraw)
{
    static unsigned last_time = 0;
    
    unsigned cur_time = app_time();
    unsigned deltaTime = cur_time - last_time;

    view_matrix = vmath::lookat(eye_pos, focal_point, view_up);
    proj_matrix = vmath::perspective(60.0f, 1.333f, 0.1f, far_plane_distance);

    RenderOpaque(deltaTime * 0.001f);

    if (transparent_active) {
        if (oit_active) {
            RenderWeightBlendedOIT(deltaTime * 0.001f);
            RenderOITQuad(deltaTime * 0.001f);
        } else {
            SortBackToFront(translucent_objects);
            RenderTranslucent(deltaTime * 0.001f);
        }
    }

    RenderViewQuad();

    base::Display();
    
    last_time = cur_time;
}

void WeightBlendedOitExample::RenderOpaque(float time)
{
    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glUseProgram(opaque_prog);

    mv_mat_loc = glGetUniformLocation(opaque_prog, "model_matrix");
    prj_mat_loc = glGetUniformLocation(opaque_prog, "proj_matrix");
    col_amb_loc = glGetUniformLocation(opaque_prog, "color_ambient");
    col_diff_loc = glGetUniformLocation(opaque_prog, "color_diffuse");
    col_spec_loc = glGetUniformLocation(opaque_prog, "color_specular");

    glUniformMatrix4fv(prj_mat_loc, 1, GL_FALSE, proj_matrix);

    glEnable(GL_DEPTH_TEST);

    for (auto& object : opaque_objects) {
        object->Rotate(-10.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

        const vmath::mat4& modelMatrix = object->GetTransform();
        vmath::mat4 mv_matrix = view_matrix * modelMatrix;

        glUniformMatrix4fv(mv_mat_loc, 1, GL_FALSE, mv_matrix);

        float color[4];
        object->GetColor(color);
        glUniform4fv(col_diff_loc, 1, color);

        object->Render();
    }
}

void WeightBlendedOitExample::RenderTranslucent(float time)
{
    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glUseProgram(translucent_prog);

    mv_mat_loc = glGetUniformLocation(translucent_prog, "model_matrix");
    prj_mat_loc = glGetUniformLocation(translucent_prog, "proj_matrix");
    col_amb_loc = glGetUniformLocation(translucent_prog, "color_ambient");
    col_diff_loc = glGetUniformLocation(translucent_prog, "color_diffuse");
    col_spec_loc = glGetUniformLocation(translucent_prog, "color_specular");

    glUniformMatrix4fv(prj_mat_loc, 1, GL_FALSE, proj_matrix);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto& object : translucent_objects) {
        object->Rotate(5.0f * time * 3.14159f, 0.0f, 0.0f, 1.0f);
        object->Rotate(10.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

        const vmath::mat4& modelMatrix = object->GetTransform();
        vmath::mat4 mv_matrix = view_matrix * modelMatrix;

        glUniformMatrix4fv(mv_mat_loc, 1, GL_FALSE, mv_matrix);

        float color[4];
        object->GetColor(color);
        glUniform4fv(col_diff_loc, 1, color);

        object->Render();
    }

    glDisable(GL_BLEND);
}

void WeightBlendedOitExample::RenderWeightBlendedOIT(float time)
{
    // Note the depth buffer is shared with main_fbo to make correct depth testing.
    glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_BLEND );
    glViewport(0, 0, window_width, window_height);

    // clear accum texture.
    static const float clear_accum[] = { 0, 0, 0, 0 };
    glClearBufferfv( GL_COLOR, 0, clear_accum );
    glBlendEquationi( 0, GL_FUNC_ADD );
    glBlendFunci( 0, GL_ONE, GL_ONE );

    // clear reveal texture.
    static const float clear_reveal[] = { 1, 1, 1, 1 };
    glClearBufferfv( GL_COLOR, 1, clear_reveal );
    glBlendEquationi( 1, GL_FUNC_ADD );
    glBlendFunci( 1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR );

    glUseProgram(oit_output_prog);
    
    mv_mat_loc = glGetUniformLocation(oit_output_prog, "model_matrix");
    prj_mat_loc = glGetUniformLocation(oit_output_prog, "proj_matrix");
    col_amb_loc = glGetUniformLocation(oit_output_prog, "color_ambient");
    col_diff_loc = glGetUniformLocation(oit_output_prog, "color_diffuse");
    z_scalar_loc = glGetUniformLocation(oit_output_prog, "z_scalar");

    glUniformMatrix4fv(prj_mat_loc, 1, GL_FALSE, proj_matrix);
    // The equation from McGuire & Bavoil is tuned for z range 0.1 <= |z| <= 500,
    // so we should have a scalar to adapt that.
    glUniform1f(z_scalar_loc, 500.0 / far_plane_distance);

    int count = 0;
    for (auto& object : translucent_objects) {
        object->Rotate(5.0f * time * 3.14159f, 0.0f, 0.0f, 1.0f);
        object->Rotate(10.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

        const vmath::mat4& modelMatrix = object->GetTransform();
        vmath::mat4 mv_matrix = view_matrix * modelMatrix;

        glUniformMatrix4fv(mv_mat_loc, 1, GL_FALSE, mv_matrix);

        float color[4];
        object->GetColor(color);
        glUniform4fv(col_diff_loc, 1, color);

        object->Render();
    }

    glDisable(GL_BLEND);
}

void WeightBlendedOitExample::RenderOITQuad(float time)
{
    // Composite over the default buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, main_fbo);

    glDepthMask( GL_FALSE );
    glDisable(GL_DEPTH_TEST);
    
    glEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA );
    
    glViewport(0, 0, window_width, window_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accum_texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reveal_texture);
    
    glUseProgram( oit_overlay_prog );
    glUniform1i( 0, 0 );
    glUniform1i( 1, 1 );
    
    glBindVertexArray( fullscreen_quad_vao );
    
    glDrawArrays( GL_TRIANGLES, 0, 6 );

    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
}

void WeightBlendedOitExample::RenderViewQuad()
{
    // Bind to default frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    glViewport(0, 0, window_width, window_height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, main_color_texture);

    glUseProgram(render_view_prog);
    glUniform1i( 0, 0 );
    
    glBindVertexArray( fullscreen_quad_vao );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
    glBindVertexArray(0);
}

void WeightBlendedOitExample::Finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(opaque_prog);
}

void WeightBlendedOitExample::Resize(int width, int height)
{
    glViewport(0, 0, width, height);

    window_width = width;
    window_height = height;

    // Reset frame buffers with new window size.
    CreateMainFrameBuffer();
    CreateOITFrameBuffer();
}

void WeightBlendedOitExample::SortBackToFront(std::vector<std::shared_ptr<VBObjectExt>>& objects)
{
    std::sort(objects.begin(), objects.end(), [this](std::shared_ptr<VBObjectExt>& a, std::shared_ptr<VBObjectExt>& b) {
        vmath::mat4 transA = a->GetTransform();
        vmath::vecN<float, 3> posA;
        posA[0] = transA[3][0];
        posA[1] = transA[3][1];
        posA[2] = transA[3][2];
        float disA = vmath::distance(eye_pos, posA);

        vmath::mat4 transB = b->GetTransform();
        vmath::vecN<float, 3> posB;
        posB[0] = transB[3][0];
        posB[1] = transB[3][1];
        posB[2] = transB[3][2];
        float disB = vmath::distance(eye_pos, posB);
        
        return disA > disB;
    });
}

void WeightBlendedOitExample::OnKey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_SPACE:
                oit_active = !oit_active;
                break;
            case GLFW_KEY_T:
                transparent_active = !transparent_active;
                break;
            case GLFW_KEY_S:
                break;
            case GLFW_KEY_A:
                break;
            case GLFW_KEY_L:
                break;
            default:
                break;
        }
    }

    if (action == GLFW_REPEAT) {
        HandleCameraTransform(key);
    }
}

void WeightBlendedOitExample::HandleCameraTransform(int key)
{
    switch (key) {
        // Front
        case GLFW_KEY_W:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                front = normalize(front);
                vmath::vecN<float, 3> offset = front * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Back
        case GLFW_KEY_S:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                front = normalize(front);
                vmath::vecN<float, 3> offset = -front * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Left
        case GLFW_KEY_A:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                front = normalize(front);
                vmath::vecN<float, 3> right = vmath::cross(front, view_up);
                right = normalize(right);
                vmath::vecN<float, 3> offset = -right * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Right
        case GLFW_KEY_D:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                front = normalize(front);
                vmath::vecN<float, 3> right = vmath::cross(front, view_up);
                right = normalize(right);
                vmath::vecN<float, 3> offset = right * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Up
        case GLFW_KEY_Q:
            {
                vmath::vecN<float, 3> offset = view_up * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Down
        case GLFW_KEY_Z:
            {
                vmath::vecN<float, 3> offset = -view_up * camera_move_speed;
                eye_pos += offset;
                focal_point += offset;
            }
            break;
        // Rotate to left
        case GLFW_KEY_LEFT:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                auto rotateMatrix = vmath::rotate(-camera_rotate_speed, view_up);

                vmath::vecN<float, 4> homoFront;
                homoFront[0] = front[0];
                homoFront[1] = front[1];
                homoFront[2] = front[2];
                homoFront[3] = 1.0;
                auto result = homoFront * rotateMatrix;
                focal_point[0] = eye_pos[0] + result[0];
                focal_point[1] = eye_pos[1] + result[1];
                focal_point[2] = eye_pos[2] + result[2];
            }
            break;
        // Rotate to right
        case GLFW_KEY_RIGHT:
            {
                vmath::vecN<float, 3> front = focal_point - eye_pos;
                auto rotateMatrix = vmath::rotate(camera_rotate_speed, view_up);

                vmath::vecN<float, 4> homoFront;
                homoFront[0] = front[0];
                homoFront[1] = front[1];
                homoFront[2] = front[2];
                homoFront[3] = 1.0;
                auto result = homoFront * rotateMatrix;
                focal_point[0] = eye_pos[0] + result[0];
                focal_point[1] = eye_pos[1] + result[1];
                focal_point[2] = eye_pos[2] + result[2];
            }
            break;
        default:
            break;
    }
}

