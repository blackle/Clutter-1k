void main(){
	vec2 uv = (cogl_tex_coord_in[0].xy*2.0-1.0)*vec2(1.0, 0.56);
	cogl_color_out.w=1.0; //the alpha value actually matters here
	cogl_color_out.xyz=sqrt(mix(vec3(0.0,0.3,0.7),vec3(0.8,0.1,0.0),0.5+0.5*sin(length(uv)*132.0)))*cogl_tex_coord_in[0].y;
}