#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

out vec4 color;

uniform sampler2D texture1;
uniform int tex;

uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform vec3 lightDiff;
uniform vec3 ligthSpec;

void main() {
	if (tex == 0) {
        vec4 objectColor = texture(texture1, TexCoord);

	    // ambient
        vec3 ambient = lightColor * materialAmbient;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * materialDiffuse;
    
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        //vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        //vec3 specular = ligthSpec * (spec * materialSpecular);
        vec3 specular = spec * lightColor * materialSpecular;
        //vec3 specular = ligthSpec * lightColor * (spec * materialSpecular);
        //vec3 specular = ligthSpec * spec * lightColor;  

        vec3 result = (ambient + diffuse + specular);
        color = vec4(result, 1.0) * objectColor;
    }
	if (tex == 1) {
	    // ambient
        float ambientStrength = 0.5;
        vec3 ambient = ambientStrength * lightColor;
    
         // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(LightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // specular
        float specularStrength = 1.0;
        vec3 viewDir = normalize(viewPos - FragPos);
        //vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;
    
        vec3 result = (ambient + diffuse + specular) * objectColor;
        color = vec4(result, 1.0);
    }
	if (tex == 2)
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}