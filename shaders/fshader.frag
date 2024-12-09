// #version 330 core
// out vec4 FragColor;

// in vec3 Normal;  
// in vec3 FragPos;  
  
// uniform vec3 objectColor;
// uniform vec3 viewPos;

// uniform float ambientStrength;
// uniform float specularStrength;
// uniform float diffuseStrength;
// uniform float shininess;

// struct Light {
//     vec3 position;
//     vec3 color;
// };

// uniform Light lights[2];

// void main()
// {
//     vec3 norm = normalize(Normal);
//     vec3 viewDir = normalize(viewPos - FragPos);

//     vec3 ambient = vec3(0.0);
//     vec3 diffuse = vec3(0.0);
//     vec3 specular = vec3(0.0);

//     for(int i=0; i<2; i++) {
//         // ambient
//         ambient += ambientStrength * lights[i].color;

//         // diffuse
//         vec3 lightDir = normalize(lights[i].position - FragPos);
//         float diff = max(dot(norm, lightDir), 0.0);
//         diffuse += diff * diffuseStrength * lights[i].color;

//         // specular
//         vec3 reflectDir = reflect(-lightDir, norm);
//         float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
//         specular += specularStrength * spec * lights[i].color;
//     }
//     vec3 result = (ambient + diffuse + specular) * objectColor;
//     FragColor = vec4(result, 1.0);
// } 

#version 330 core

in vec3 color;
out vec4 outColor;

void main() {
    outColor = vec4(color, 1.0);
}