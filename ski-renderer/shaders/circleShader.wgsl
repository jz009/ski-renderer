fn circle(vec2f p, float r, vec2f uv) -> float
{
    float d = distance(p, uv);
    return d;
}

struct VertexInput {
	@location(0) position: vec3f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec3f,
};

struct Uniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: vec4f,
    time: f32,
};

@group(0) @binding(0) var<uniform> uUniforms: Uniforms;

const pi = 3.14159265359;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uMyUniforms.projectionMatrix * uMyUniforms.viewMatrix * uMyUniforms.modelMatrix * vec4f(in.position, 1.0);
	out.color = in.color;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	vec4f uv = in.position;
    float dist = circle(vec2(0.5, 0.5), 0.5, uv);
    vec3 white = vec3f(1.0, 1.0, 1.0);

    float tp = 0.5;
    float f = 0.005;
    
    vec3 col = smoothstep(tp - f, tp + f, dist) *  white;

    return vec4(col,1.0);
}