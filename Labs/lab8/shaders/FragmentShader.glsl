#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform vec3 object_color;
uniform int spotlight;
uniform float cut_off_radians;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 N = normalize(world_normal);
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);
	vec3 R = normalize(reflect(L, world_normal));

    // TODO(student): Define ambient, diffuse and specular light components
    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(normalize(N), L), 0.f);
    float specular_light = 0;
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
	float light = 0.f;
    if(spotlight == 1)
	{
		float spot_light = dot(-L, light_direction);
		float spot_light_lim = cos(cut_off_radians);

		if(spot_light > spot_light_lim)
		{
			float att = (spot_light - spot_light_lim) / (1.f - spot_light_lim);
			float light_att = att * att;
			light = ambient_light + light_att * (diffuse_light + specular_light);
		}
		else
		{
			light = ambient_light;
		}
	} else {
		float d = distance(light_position, world_position);
		float att_fact = 1.f / max(d * d, 1.f);
		light = ambient_light + att_fact * (diffuse_light + specular_light);
	}
    
    vec3 c = object_color * light;
	out_color = vec4(c, 1.f);

}
