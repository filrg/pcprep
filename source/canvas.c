#include <pcprep/canvas.h>
#include <pcprep/vec3f.h>
#include <pcprep/vec3uc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GPU
#include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static const char *default_vert_shader =
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec3 vertexCorlor;\n"
    "uniform mat4 MVP;\n"
    "out vec3 frag_color;\n"
    "void main(){\n"
    "    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);\n"
    "    gl_PointSize = 1.0;\n"
    "    frag_color = vertexCorlor;\n"
    "}";

static const char *default_frag_shader = "#version 330 core\n"
                                         "in vec3 frag_color;\n"
                                         "out vec3 color;\n"
                                         "void main(){\n"
                                         "    color = frag_color;\n"
                                         "}";

GLuint             load_shader(char *vertex_shader_code,
                               char *fragment_shader_code)
{
  // Create the shaders
  GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  if (vertex_shader_code == PCPREP_NULL)
  {
    vertex_shader_code = (char *)default_vert_shader;
  }
  if (fragment_shader_code == PCPREP_NULL)
  {
    fragment_shader_code = (char *)default_frag_shader;
  }

  GLint       Result = GL_FALSE;
  int         InfoLogLength;

  // Compile Vertex Shader
  const char *VertexSourcePointer = vertex_shader_code;
  glShaderSource(
      VertexShaderID, 1, &VertexSourcePointer, PCPREP_NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    char *VertexShaderErrorMessage =
        (char *)malloc(InfoLogLength + 1);
    glGetShaderInfoLog(VertexShaderID,
                       InfoLogLength,
                       PCPREP_NULL,
                       VertexShaderErrorMessage);
    fprintf(stderr, "%s\n", VertexShaderErrorMessage);
    free(VertexShaderErrorMessage);
  }

  // Compile Fragment Shader
  const char *FragmentSourcePointer = fragment_shader_code;
  glShaderSource(
      FragmentShaderID, 1, &FragmentSourcePointer, PCPREP_NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    char *FragmentShaderErrorMessage =
        (char *)malloc(InfoLogLength + 1);
    glGetShaderInfoLog(FragmentShaderID,
                       InfoLogLength,
                       PCPREP_NULL,
                       FragmentShaderErrorMessage);
    fprintf(stderr, "%s\n", FragmentShaderErrorMessage);
    free(FragmentShaderErrorMessage);
  }

  // Link the program
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0)
  {
    char *ProgramErrorMessage = (char *)malloc(InfoLogLength + 1);
    glGetProgramInfoLog(
        ProgramID, InfoLogLength, PCPREP_NULL, ProgramErrorMessage);
    fprintf(stderr, "%s\n", ProgramErrorMessage);
    free(ProgramErrorMessage);
  }

  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

static pcs_ret_t pcp_canvas_start_gl(pcp_canvas_t *cv)
{
  GLFWwindow *window   = PCPREP_NULL;

  cv->vertex_array_id  = 0;
  cv->program_id       = 0;
  cv->frame_buffer     = 0;
  cv->rendered_texture = 0;
  cv->depth_buffer     = 0;

  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return PCPREP_RET_FAIL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  window = glfwCreateWindow(1, 1, "Hidden", PCPREP_NULL, PCPREP_NULL);
  if (!window)
  {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return PCPREP_RET_FAIL;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    glfwTerminate();
    return PCPREP_RET_FAIL;
  }

  glClearColor(cv->bg_col.x / 255.0f,
               cv->bg_col.y / 255.0f,
               cv->bg_col.z / 255.0f,
               1.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_PROGRAM_POINT_SIZE);

  glGenVertexArrays(1, &cv->vertex_array_id);
  glBindVertexArray(cv->vertex_array_id);

  cv->program_id = load_shader(cv->vert_shader, cv->frag_shader);
  cv->matrix_id  = glGetUniformLocation(cv->program_id, "MVP");

  glGenFramebuffers(1, &cv->frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, cv->frame_buffer);

  glGenTextures(1, &cv->rendered_texture);
  glBindTexture(GL_TEXTURE_2D, cv->rendered_texture);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               cv->width,
               cv->height,
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               PCPREP_NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         cv->rendered_texture,
                         0);

  glGenRenderbuffers(1, &cv->depth_buffer);

  glBindRenderbuffer(GL_RENDERBUFFER, cv->depth_buffer);

  glRenderbufferStorage(
      GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cv->width, cv->height);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER,
                            cv->depth_buffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
      GL_FRAMEBUFFER_COMPLETE)
  {
    fprintf(stderr, "Framebuffer is not complete!\n");
    return PCPREP_RET_FAIL;
  }
  return PCPREP_RET_SUCESS;
}

pcp_ret_t pcp_canvas_free_gl(pcp_canvas_t *cv)
{
  glDeleteFramebuffers(1, &cv->frame_buffer);
  glDeleteTextures(1, &cv->rendered_texture);
  glDeleteRenderbuffers(1, &cv->depth_buffer);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  glDeleteProgram(cv->program_id);
  glDeleteVertexArrays(1, &cv->vertex_array_id);

  glfwTerminate();

  return PCPREP_RET_SUCCESS;
}

#endif

pcp_ret_t pcp_canvas_init(pcp_canvas_t *cv,
                          size_t        width,
                          size_t        height,
#ifdef HAVE_GPU
                          char *vert_shader,
                          char *frag_shader,
#endif
                          pcp_vec3uc_t bg_col)
{
  cv->width       = width;
  cv->height      = height;
  cv->bg_col      = bg_col;
  cv->pixels      = (unsigned char *)malloc(sizeof(unsigned char) *
                                       cv->width * cv->height * 3);
  cv->min_z_value = (float **)malloc(sizeof(float *) * cv->height);
  for (int i = 0; i < cv->height; i++)
  {
    cv->min_z_value[i] = (float *)malloc(sizeof(float) * cv->width);
  }
#ifdef HAVE_GPU
  cv->vert_shader = vert_shader;
  cv->frag_shader = frag_shader;
  pcp_canvas_start_gl(cv);
  cv->draw_points = pcp_canvas_draw_points_gpu;
#else
  cv->draw_points = pcp_canvas_draw_points_cpu;
#endif

  cv->clear = pcp_canvas_clear;
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_canvas_free(pcp_canvas_t *cv)
{
  cv->bg_col = (pcp_vec3uc_t){0};
  if (cv->pixels)
  {
    free(cv->pixels);
    cv->pixels = 0;
  }
  if (cv->min_z_value)
  {
    for (int i = 0; i < cv->height; i++)
    {
      if (cv->min_z_value[i])
      {
        free(cv->min_z_value[i]);
        cv->min_z_value[i] = 0;
      }
    }
    free(cv->min_z_value);
    cv->min_z_value = 0;
  }
  cv->width  = 0;
  cv->height = 0;

#ifdef HAVE_GPU
  cv->vert_shader = 0;
  cv->frag_shader = 0;
  pcp_canvas_free_gl(cv);
#endif

  return PCPREP_RET_SUCCESS;
}
#ifdef HAVE_GPU
pcp_ret_t pcp_canvas_draw_points_gpu(pcp_canvas_t  *cv,
                                     float         *mvp,
                                     float         *pos,
                                     unsigned char *rgb,
                                     size_t         count)
{
  unsigned int vbuffer = 0;
  unsigned int rbuffer = 0;

  glUseProgram(cv->program_id);
  glUniformMatrix4fv(cv->matrix_id, 1, GL_FALSE, mvp);

  glGenBuffers(1, &vbuffer);
  glGenBuffers(1, &rbuffer);

  glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
  glBufferData(GL_ARRAY_BUFFER,
               count * 3 * sizeof(float),
               pos,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, rbuffer);
  glBufferData(GL_ARRAY_BUFFER,
               count * 3 * sizeof(unsigned char),
               rgb,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, PCPREP_NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, rbuffer);
  glVertexAttribPointer(
      1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, PCPREP_NULL);

  glBindFramebuffer(GL_FRAMEBUFFER, cv->frame_buffer);
  glViewport(0, 0, cv->width, cv->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawArrays(GL_POINTS, 0, count);

  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0,
               0,
               cv->width,
               cv->height,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               cv->pixels);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  glDeleteBuffers(1, &rbuffer);
  glDeleteBuffers(1, &vbuffer);

  return PCPREP_RET_SUCCESS;
}
#endif

pcp_ret_t pcp_canvas_draw_points_cpu(pcp_canvas_t  *cv,
                                     float         *mvp,
                                     float         *pos,
                                     unsigned char *rgb,
                                     size_t         count)
{
  pcp_vec3f_t  *positions  = PCPREP_NULL;
  pcp_vec3uc_t *colors     = PCPREP_NULL;
  pcp_vec3uc_t *rgb_pixels = PCPREP_NULL;
  size_t        screen_w   = 0;
  size_t        screen_h   = 0;

  rgb_pixels               = (pcp_vec3uc_t *)cv->pixels;

  for (int i = 0; i < cv->height; i++)
  {
    for (int j = 0; j < cv->width; j++)
    {
      cv->min_z_value[i][j]         = 2.0f;
      rgb_pixels[i * cv->width + j] = cv->bg_col;
    }
  }

  positions = (pcp_vec3f_t *)pos;
  colors    = (pcp_vec3uc_t *)rgb;

  for (int i = 0; i < count; i++)
  {
    pcp_vec3f_t ndc = pcp_vec3f_mvp_mul(positions[i], mvp);
    if (ndc.x >= -1 && ndc.x <= 1 && ndc.y >= -1 && ndc.y <= 1 &&
        ndc.z >= 0 && ndc.z <= 1)
    {
      screen_w = (int)((ndc.x + 1.0) * 0.5 * cv->width);
      screen_h = (int)((ndc.y + 1.0) * 0.5 * cv->height);

      if (screen_w >= cv->width)
        screen_w = cv->width - 1;
      if (screen_h >= cv->height)
        screen_h = cv->height - 1;

      if (ndc.z < cv->min_z_value[screen_h][screen_w])
      {
        cv->min_z_value[screen_h][screen_w] = ndc.z;
        int idx         = screen_h * cv->width + screen_w;
        rgb_pixels[idx] = colors[i];
      }
    }
  }
  return PCPREP_RET_SUCCESS;
}

pcp_ret_t pcp_canvas_clear(pcp_canvas_t *cv)
{
  memset(cv->pixels,
         0,
         sizeof(unsigned char) * cv->width * cv->height * 3);
  for (int i = 0; i < cv->height; i++)
  {
    memset(cv->min_z_value[i], 0, sizeof(float) * cv->width);
  }
  return PCPREP_RET_SUCCESS;
}
