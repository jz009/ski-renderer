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
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    let coord = in.pos.xyz / 100.0;

    let fx = abs(coord.x - round(coord.x));
    let fz = abs(coord.y - round(coord.y));
    let line = min(fx, fz);

    let aa = fwidth(coord); 
    let pixelWidth = min(aa.x, aa.y);

    let thickness = 0.02;

    let lineMask =  1.0 - smoothstep(thickness - pixelWidth, thickness + pixelWidth, line);

    let groundColor = vec3<f32>(0.1, 0.1, 0.1);
    let lineColor   = vec3<f32>(0.8, 0.8, 0.8);

    let color = mix(groundColor, lineColor, lineMask);
    return vec4<f32>(color, 1.0);
}

// fn filteredGrid(p: vec2<f32>, dpdx: vec2<f32>, dpdy: vec2<f32>) -> f32 {
// 	let N: f32 = 10.;
// 	let w: vec2<f32> = max(abs(dpdx), abs(dpdy));
// 	let a: vec2<f32> = p + 0.5 * w;
// 	let b: vec2<f32> = p - 0.5 * w;
// 	let i: vec2<f32> = (floor(a) + min(fract(a).x * N, min(fract(a).y * N, 1.0)) - floor(b) - min(fract(b).x * N, min(fract(b).y * N, 1.0))) / (N * w);
// 	return (1.0 - i.x) * (1. - i.y);
// } 

// @fragment
// fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
//     let uv = in.pos.xy / vec2f(1200, 1200);

//     let scaledUV = uv * 24.0;
    
//     let dx = dpdx(uv);
//     let dy = dpdy(uv);


//     let c = filteredGrid(scaledUV, dx, dy);
//     let color = mix(vec3(1.0), vec3(0.0), c);
//     return vec4f(color, 1.0);
// }

// const N: f32 = 10.;
// fn gridTextureGradBox(p: vec2<f32>, ddx: vec2<f32>, ddy: vec2<f32>) -> f32 {
// 	let w: vec2<f32> = max(abs(ddx), abs(ddy)) + 0.01;
// 	let a: vec2<f32> = p + 0.5 * w;
// 	let b: vec2<f32> = p - 0.5 * w;
// 	var i: vec2<f32> = (floor(a) + min(fract(a).x * N, min(fract(a).y * N, 1.0)) - floor(b) - min(fract(b).x * N, min(fract(b).y * N, 1.0))) / (N * w);
// 	return (1. - i.x) * (1. - i.y);
// } 

// fn gridTexture(p: vec2<f32>) -> f32 {
// 	let i: vec2<f32> = step(fract(p), vec2<f32>(1. / N));
// 	return (1. - i.x) * (1. - i.y);
// } 

// fn intersect(ro: vec3<f32>, rd: vec3<f32>, pos: ptr<function, vec3<f32>>, nor: ptr<function, vec3<f32>>, matid: ptr<function, i32>) -> f32 {
// 	var tmin: f32 = 10000.;
// 	(*nor) = vec3<f32>(0.);
// 	(*pos) = vec3<f32>(0.);
// 	(*matid) = -1;
// 	let h: f32 = (0.01 - ro.y) / rd.y;
// 	if (h < 0.) {
// 		tmin = h;
// 		(*nor) = vec3<f32>(0., 1., 0.);
// 		(*pos) = ro + h * rd;
// 		(*matid) = 0;
// 	}
// 	return tmin;
// } 

// fn texCoords(pos: vec3<f32>, mid: i32) -> vec2<f32> {
// 	let matuv: vec2<f32> = pos.xz;
// 	return 8. * matuv;
// } 

// fn calcCamera(ro: ptr<function, vec3<f32>>, ta: ptr<function, vec3<f32>>)  {
// 	let an: f32 = 0.1 * sin(0.1 * 0);
// 	(*ro) = vec3<f32>(5. * cos(an), 0.5, 5. * sin(an));
// 	(*ta) = vec3<f32>(0., 1., 0.);
// } 

// fn calcRayForPixel(pix: vec2<f32>, resRo: ptr<function, vec3<f32>>, resRd: ptr<function, vec3<f32>>)  {
//  let R: vec2<f32> = vec2f(750, 1200);
// 	var p: vec2<f32> = (2. * pix - R.xy) / R.y;
// 	var ro: vec3<f32>;
// 	var ta: vec3<f32>;
// 	calcCamera(&ro, &ta);
// 	let ww: vec3<f32> = normalize(ta - ro);
// 	let uu: vec3<f32> = normalize(cross(ww, vec3<f32>(0., 1., 0.)));
// 	let vv: vec3<f32> = normalize(cross(uu, ww));
// 	let rd: vec3<f32> = normalize(p.x * uu + p.y * vv + 2. * ww);
// 	(*resRo) = ro;
// 	(*resRd) = rd;
// } 

// @fragment
// fn fs_main(in: VertexOutput) -> @location(0) vec4f {
//     let R: vec2<f32> = vec2f(1500, 1000);
//     let y_inverted_location = vec2<i32>(i32(in.pos.x), i32(R.y) - i32(in.pos.y));
//     let location = vec2<i32>(i32(in.pos.x), i32(in.pos.y));
    
// 	var fragColor: vec4<f32>;
// 	var fragCoord = vec2<f32>(f32(location.x), f32(location.y) );

// 	let p: vec2<f32> = (-R.xy + 2. * fragCoord) / R.y;
// 	var ro: vec3<f32>;
// 	var rd: vec3<f32>;
// 	var ddx_ro: vec3<f32>;
// 	var ddx_rd: vec3<f32>;
// 	var ddy_ro: vec3<f32>;
// 	var ddy_rd: vec3<f32>;
// 	calcRayForPixel(fragCoord + vec2<f32>(0., 0.), &ro, &rd);
// 	calcRayForPixel(fragCoord + vec2<f32>(1., 0.), &ddx_ro, &ddx_rd);
// 	calcRayForPixel(fragCoord + vec2<f32>(0., 1.), &ddy_ro, &ddy_rd);
// 	var pos: vec3<f32>;
// 	var nor: vec3<f32>;
// 	var mid: i32;
// 	var t: f32 = intersect(ro, rd, &pos, &nor, &mid);
// 	var col: vec3<f32> = vec3<f32>(0.9);
// 	if (mid != -1) {
// 		let ddx_pos: vec3<f32> = ddx_ro - ddx_rd * dot(ddx_ro - pos, nor) / dot(ddx_rd, nor);
// 		let ddy_pos: vec3<f32> = ddy_ro - ddy_rd * dot(ddy_ro - pos, nor) / dot(ddy_rd, nor);
// 		let uv: vec2<f32> = texCoords(pos, mid);
// 		let ddx_uv: vec2<f32> = texCoords(ddx_pos, mid) - uv;
// 		let ddy_uv: vec2<f32> = texCoords(ddy_pos, mid) - uv;
// 		let mate: vec3<f32> = vec3<f32>(1.) * gridTextureGradBox(uv, ddx_uv, ddy_uv);
// 		col = mate;
// 	}
// 	return vec4<f32>(col, 1.);
// } 
    