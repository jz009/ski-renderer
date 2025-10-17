struct VertexInput {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) color: vec3f,
    @location(3) uv: vec2f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec3f,
	@location(1) normal: vec3f,
	@location(2) uv: vec2f,
};

struct VisualUniforms
{
    color: vec4f,
};

struct TransformUniforms
{
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
};

@group(0) @binding(0) var<uniform> visuals: VisualUniforms;
@group(0) @binding(1) var<uniform> transforms: TransformUniforms;
@group(0) @binding(2) var texture: texture_2d<f32>;
@group(0) @binding(3) var s: sampler;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = transforms.projectionMatrix * transforms.viewMatrix * transforms.modelMatrix * vec4f(in.position, 1.0);
    out.normal = (transforms.modelMatrix * vec4f(in.normal, 0.0)).xyz;
	out.color = visuals.color.xyz;
	out.uv = in.uv;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let normal = normalize(in.normal);

	let lightColor1 = vec3f(1.0, 0.7, 0.6);
	let lightColor2 = vec3f(0.6, 0.4, 0.3);
	let lightDirection1 = vec3f(0.5, 0.2, 0.1);
	let lightDirection2 = vec3f(-0.2, -1.4, -1.3);
	let shading1 = max(0.0, dot(lightDirection1, normal));
	let shading2 = max(0.0, dot(lightDirection2, normal));
	let shading = shading1 * lightColor1 + shading2 * lightColor2;
	// let texelCoords = vec2i(in.uv * vec2f(textureDimensions(gradientTexture)));
	// let color = textureSample(gradientTexture, sampler, in.uv);
	let color = in.color * shading;
	// let color = textureLoad(texture, vec2i(in.uv), 0).rgb;
	// let texelCoords = vec2i(in.uv * vec2f(textureDimensions(texture)));
    // let color = textureLoad(texture, texelCoords, 0).rgb;
	// Gamma-correction
	let corrected_color = pow(color, vec3f(2.2));
	return vec4f(corrected_color, visuals.color.a);
	//return vec4f(color, uUniforms.color.a);
	//return textureSample(texture, s, in.uv).bgra;
}