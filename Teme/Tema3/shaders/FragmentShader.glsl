#version 330 core

in vec3 world_position; 
in vec3 world_normal; 

uniform vec3 eye_position;   	
uniform vec3 light_position; 	
uniform vec3 light_color; 		
uniform vec3 object_color; 		

uniform vec3 material_kd;
uniform vec3 material_ks;
uniform float material_shininess;
uniform int elem;
uniform float time;

uniform bool spotlight;
uniform float cut_off_radians;
uniform float spotlight_radians;
uniform vec3 light_direction;
uniform vec3 light_positions_spotlights[4];
uniform vec3 light_colors_spotlights[4];

uniform vec3 light_positions_floor[9];
uniform vec3 light_colors_floor[9];

uniform sampler2D texture;
uniform int mode;

layout (location = 0) out vec4 out_color;

vec3 computeColor(vec3 light_position, vec3 light_color, bool spot, vec3 dir, float radians) {
	vec3 Emissive = light_color;
	vec3 N = normalize( world_normal );
	vec3 L = normalize( light_position - world_position );
	float NdotL = abs(dot( N, L ));
	vec3 Diffuse =  NdotL * material_kd * light_color * 1.2;

	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	vec3 R = reflect( -L, N );
	float RdotV = max( dot( R, V ), 0 );
	float NdotH = max( dot( N, H ), 0 );

	float receive_light = 0.5;
    if(dot(N, L) > 0) {
        receive_light = 1;
    }

	vec3 Specular = pow(RdotV, material_shininess) * material_ks * 2.5 * receive_light;

	float d = distance(light_position, world_position);
	float att = 0;
	if(elem == 3 && !spot && mode == 0) {
		float light_radius = 2.02f;
		if(d < light_radius) {
            att = pow(light_radius - d, 2);
        }
	} 
	
	if(elem != 3 && !spot) {
		att = 1.f / max(d * d, 1.f);
	}
	if(spot) {
		float spot_light = abs(dot(-L, dir));
		float spot_light_lim = cos(radians);
		
		if(spot_light > spot_light_lim) {
			float light_att = (spot_light - spot_light_lim) / (1.f - spot_light_lim);
			att = light_att * light_att;
		} else {
			if(elem == 3 && mode != 0) {
				att = 1.f / max(d * d, 1.f);
			}
			return light_color * att;
		}
	}
	return att * (Diffuse + Specular);
}

void main() {
	if(elem == 4) {
		out_color = vec4(light_color , 0.6f);
		return;
	}
	vec3 color = vec3(0, 0, 0);

	if(mode == 0) {
		if(elem == 1 && !spotlight) {
			out_color = vec4(light_color , 1.f);
			return;
		}
		if(elem != 3) {
			color = computeColor(light_position, light_color, false, light_direction, 0.f);
		}
		if(elem == 2) {
			out_color = vec4(object_color * color , 1.f);
			return;
		}

		if(elem == 3) {
			for(int i = 0; i < 9; i++) {
				color += 0.5 * computeColor(light_positions_floor[i], light_colors_floor[i], false, light_direction, 0.f);
			}
		}	
	} else if(mode == 1) {
		vec3 light_dir = world_position - light_position;
		vec2 texcoord;
		texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z);
		texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));

		texcoord = vec2(texcoord.x - time / 10.f, texcoord.y);

		vec4 c = texture2D (texture, texcoord);
		color = computeColor(light_position, c.xyz, true, light_direction, cut_off_radians);
		if(elem == 5) {
			out_color = c;
			return;
		} 
	}
	if(spotlight && elem != 6) {
		vec3 aux = color;
		for(int i = 0; i < 4; i++) {
			color += 1.5 * computeColor(light_positions_spotlights[i], light_colors_spotlights[i], true, vec3(0, -1, 0), spotlight_radians);
		}	
		if(aux == color && elem == 1 && mode == 0) {
			out_color = vec4(object_color, 1.f);
			return;
		} 
	}
	out_color = vec4(color * object_color, 1.f);
}
