#version 330 core

out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;


void main()
{
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * light.specular * vec3(texture(material.specular, TexCoords));

  vec3 result = (ambient + diffuse + specular + vec3(texture(material.emission, TexCoords)));
  FragColor = vec4(result, 1.0);
}
