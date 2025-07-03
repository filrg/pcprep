#include <pcprep/point_cloud.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcprep/utils.h>


int parse_aspect_ratio(const char* aspect_str, int *width, int *height) 
{
    if (sscanf(aspect_str, "%d/%d", width, height) == 2) 
    {
        return 0;
    }
    return -1;  
}

int main(int argc, char** argv) 
{
    if (argc < 3) 
    {
        printf("Usage: %s <input-cam-param.json> <output-cam-matrix.json>\n", argv[0]);
        return 1;
    }

    
    char* json_buff = read_file(argv[1]);
    if (!json_buff) 
    {
        return 1;
    }

    cJSON* json = cJSON_Parse(json_buff);
    free(json_buff);
    if (!json) 
    {
        printf("Error parsing JSON file\n");
        return 1;
    }

    cJSON* camera = cJSON_GetObjectItem(json, "camera");
    if (!camera) 
    {
        printf("No 'camera' object found in JSON\n");
        cJSON_Delete(json);
        return 1;
    }

    
    float fovy = (float)cJSON_GetObjectItem(camera, "fovy")->valuedouble;
    int width, height;
    parse_aspect_ratio(cJSON_GetObjectItem(camera, "aspect")->valuestring, &width, &height);
    float near = (float)cJSON_GetObjectItem(camera, "near")->valuedouble;
    float far = (float)cJSON_GetObjectItem(camera, "far")->valuedouble;

    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(fovy), (float)width / (float)height, near, far);
    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    cJSON* trajectory = cJSON_GetObjectItem(camera, "trajectory");
    if (!trajectory || !cJSON_IsArray(trajectory)) 
    {
        printf("No valid 'trajectory' array found in JSON\n");
        cJSON_Delete(json);
        return 1;
    }

    
    cJSON* output_json = cJSON_CreateObject();
    cJSON* output_camera = cJSON_CreateObject();
    cJSON* output_screen = cJSON_CreateObject();
    cJSON* output_width = cJSON_AddNumberToObject(output_screen, "width", width);
    cJSON* output_height = cJSON_AddNumberToObject(output_screen, "height", height);
    cJSON* output_mvp_array = cJSON_CreateArray();


    cJSON_AddItemToObject(output_json, "camera", output_camera);
    cJSON_AddItemToObject(output_camera, "screen", output_screen);
    cJSON_AddItemToObject(output_camera, "mvp", output_mvp_array);



    int count = cJSON_GetArraySize(trajectory);
    for (int i = 0; i < count; i++) 
    {
        cJSON* point = cJSON_GetArrayItem(trajectory, i);
        if (!point) continue;

        
        cJSON* pos = cJSON_GetObjectItem(point, "position");
        cJSON* look = cJSON_GetObjectItem(point, "lookAt");
        cJSON* up = cJSON_GetObjectItem(point, "up");

        if (!pos || !look || !up) 
        {
            printf("Invalid trajectory data at index %d\n", i);
            continue;
        }

        glm::vec3 cam_position(
            (float)cJSON_GetArrayItem(pos, 0)->valuedouble,
            (float)cJSON_GetArrayItem(pos, 1)->valuedouble,
            (float)cJSON_GetArrayItem(pos, 2)->valuedouble
        );

        glm::vec3 look_at_point(
            (float)cJSON_GetArrayItem(look, 0)->valuedouble,
            (float)cJSON_GetArrayItem(look, 1)->valuedouble,
            (float)cJSON_GetArrayItem(look, 2)->valuedouble
        );

        glm::vec3 up_vector(
            (float)cJSON_GetArrayItem(up, 0)->valuedouble,
            (float)cJSON_GetArrayItem(up, 1)->valuedouble,
            (float)cJSON_GetArrayItem(up, 2)->valuedouble
        );

        
        glm::mat4 ViewMatrix = glm::lookAt(cam_position, look_at_point, up_vector);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        
        cJSON* mvp_matrix = cJSON_CreateArray();
        for (int row = 0; row < 4; row++) 
        {
            cJSON* row_array = cJSON_CreateArray();
            for (int col = 0; col < 4; col++) 
            {
                cJSON_AddItemToArray(row_array, cJSON_CreateNumber(MVP[row][col]));
            }
            cJSON_AddItemToArray(mvp_matrix, row_array);
        }
        cJSON_AddItemToArray(output_mvp_array, mvp_matrix);
    }

    
    json_write_to_file(argv[2], output_json);

    
    cJSON_Delete(json);
    cJSON_Delete(output_json);

    printf("MVP matrices successfully written to %s\n", argv[2]);
    return 0;
}
