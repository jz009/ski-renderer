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
	//out.position = uUniforms.projectionMatrix * uUniforms.viewMatrix * uUniforms.modelMatrix * vec4f(in.position, 1.0);
	out.position =  uUniforms.modelMatrix * vec4f(in.position, 1.0);
	out.color = uUniforms.color.rgb;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let color = in.color;
	// Gamma-correction
	let corrected_color = pow(color, vec3f(2.2));
	return vec4f(corrected_color, uUniforms.color.a);
}