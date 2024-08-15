#include <vermilion.h>

#include "vapp.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

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
    vglAttachShaderSource(oit_overlay_prog, GL_FRAGMENT_SHADER, oit_overlay_fs.c_str());

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

    // ������������ɫ����
    glGenTextures(1, &main_color_texture);
    glBindTexture(GL_TEXTURE_2D, main_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, main_color_texture, 0);

    // ���������������������ѡ��ȡ�����Ƿ���Ҫ�����Ϣ��
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

    if (rbo_depth) {
        glDeleteRenderbuffers(1, &rbo_depth);
        rbo_depth = 0;
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
    float time = float(app_time() & 0xFFFF) / float(0xFFFF);

    vmath::vecN<float, 3> eye;
    eye[0] = .0f;
    eye[1] = 400.0f;
    eye[2] = 0.0f;
    vmath::vecN<float, 3> center;
    center[0] = .0f;
    center[1] = .0f;
    center[2] = .0f;
    vmath::vecN<float, 3> up;
    up[0] = 1.0f;
    up[1] = .0f;
    up[2] = .0f;

    view_matrix = vmath::lookat(eye, center, up);
    proj_matrix = vmath::perspective(60.0f, 1.333f, 0.1f, 1000.0f);

    RenderOpaque(time * 0.5f);

    if (transparent_active) {
        if (oit_active) {
            RenderWeightBlendedOIT(time * 0.25f);
            RenderOITQuad(time * 0.25f);
        }
        else {
            RenderTranslucent(time * 0.25f);
        }
    }

    RenderViewQuad();

    base::Display();
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
        object->Rotate(-987.0f * time * 3.14159f, 0.0f, 0.0f, 1.0f);
        object->Rotate(-1234.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

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
        object->Rotate(987.0f * time * 3.14159f, 0.0f, 0.0f, 1.0f);
        object->Rotate(1234.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

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
    // Copy depth buffer to oit frame buffer depth. Note this is the most serious performance hit.
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0 );
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oit_fbo);
    //glBlitFramebuffer(0, 0, window_width, window_height, 0, 0, window_width, window_width, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, oit_fbo);
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

    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);

    glEnable( GL_BLEND );

    glUseProgram(oit_output_prog);
    
    mv_mat_loc = glGetUniformLocation(translucent_prog, "model_matrix");
    prj_mat_loc = glGetUniformLocation(translucent_prog, "proj_matrix");
    col_amb_loc = glGetUniformLocation(translucent_prog, "color_ambient");
    col_diff_loc = glGetUniformLocation(translucent_prog, "color_diffuse");
    col_spec_loc = glGetUniformLocation(translucent_prog, "color_specular");

    glUniformMatrix4fv(prj_mat_loc, 1, GL_FALSE, proj_matrix);

    int count = 0;
    for (auto& object : translucent_objects) {
        object->Rotate(987.0f * time * 3.14159f, 0.0f, 0.0f, 1.0f);
        object->Rotate(1234.0f * time * 3.14159f, 1.0f, 0.0f, 0.0f);

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
            case GLFW_KEY_1:
                //oit_visualization_mode = 1;
                break;
            case GLFW_KEY_2:
                //oit_visualization_mode = 2;
                break;
            case GLFW_KEY_3:
                //oit_visualization_mode = 3;
                break;
            default:
                break;
        }
    }
}
