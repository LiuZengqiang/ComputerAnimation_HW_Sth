#### opengl learning

渲染管线(graphics pipeline):
vertex shader -> shape assembly -> geometry shader  
-> rasterization -> fragment shader -> alpha tests and blending  
vertex shader:可编程，**输入**:点数据，可以包括位置颜色等等数据。**输出**:处理后的点数据 shape assembly:不可编程，**输入**:点数据，**输出**:
图元形状。根据用户指定的图元primitive，例如:GL_POINTS,GL_TRIANGLES处理输入数据 geometry shader:可编程，**输入**:图元形状，**输出**:
图元形状。可以通过产生新的顶点构造出新的图元，即可以使用几何着色器对顶点信息进行增删。 rasterization:不可编程，**输入**:图元形状，**输出**:片段(像素)。将图元映射为屏幕上最终显示的像素，在进入fragment
shader 之前会进行裁剪(clip)，丢弃掉视图之外的像素提高效率。 fragment shader:可编程，**输入**:片段(像素)，**输出**:带有颜色信息的片段(像素)。计算一个像素的最终颜色(
可以根据3D场景的数据，比如光照、阴影 和光的颜色等数据计算像素的颜色值)  
alpha tests and blending:不可编程，检测像素深度值、alpha值(透明度)，对物体进行混合