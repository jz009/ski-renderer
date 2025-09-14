struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) color: vec3f,
};

struct VertexOutput {
    @builtin(position) pos : vec4<f32>,
    @location(0) worldPos : vec3<f32>,
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
fn vs_main(@location(0) position : vec3<f32>) -> VertexOutput {
  var out : VertexOutput;
  out.pos = uUniforms.projectionMatrix * uUniforms.viewMatrix * uUniforms.modelMatrix * vec4<f32>(position, 1.0);
  out.worldPos = position;
  return out;
}

@fragment
fn fs_main(@location(0) worldPos : vec3<f32>) -> @location(0) vec4<f32> {
    let cellSize = 1.0; // spacing between grid lines
    let coord = worldPos.xz / cellSize;

    // distance to nearest grid line
    let fx = abs(fract(coord.x) - 0.5);
    let fz = abs(fract(coord.y) - 0.5);
    let line = min(fx, fz);

    // thickness of grid lines
    let thickness = 0.02;

    // smooth blend between line and ground
    let lineMask = smoothstep(thickness, 0.0, line);

    // colors
    let groundColor = vec3<f32>(0.1, 0.1, 0.1);
    let lineColor   = vec3<f32>(0.8, 0.8, 0.8);

    let color = mix(groundColor, lineColor, lineMask);
    return vec4<f32>(color, 1.0);
}