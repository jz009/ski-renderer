struct VertexInput {
	@location(0) position: vec3f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
};

struct Uniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: vec4f,
    time: f32,
};

@group(0) @binding(0) var<uniform> uUniforms: Uniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	//out.position = uUniforms.projectionMatrix * uUniforms.viewMatrix * uUniforms.modelMatrix * vec4f(in.position, 1.0);
    out.position = vec4f(in.position, 1.0);
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	var uv: vec2f = in.position.xy / vec2f(640.0, 640.0);
    // return vec4f(uv.xy / vec2f(640.0, 480.0), 0.0, 1.0);
    var dist: f32 =  distance(vec2f(0.5, 0.5), uv);
    if (dist > .13) {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }

    return vec4(1.0, 1.0, 1.0, 1.0);
}